#include <BizarroHomerHardwareControl/HardwareManager.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <ctre/phoenix/unmanaged/Unmanaged.h>
#include <fmt/core.h>

enum ControlMessageType {
  MSG_CONTROL = 1,
  MSG_ENABLE = 2,
  MSG_DISABLE = 3,
  MSG_RESET = 4,
};

enum HardwareType {
  HTYPE_DIG_IN = 0,
  HTYPE_DIG_OUT = 1,
  HTYPE_CAN_TALON_FX = 2,
  HTYPE_PWM_SPARK_MAX = 3,
  HTYPE_ENC = 4,
};

struct IPCControlMessage {
  long mtype;
  struct Data {
    uint8_t hardware_type;
    uint8_t hardware_id;
    uint8_t hardware_prop;
    double value;
  } data;
};

enum ControlProperty {
  CPROP_INIT = 0,
  CPROP_DIG = 1,
  CPROP_PCT = 2,
  CPROP_POS = 3,
};

struct IPCStatusMessage {
  long mtype;
  struct Data {
    uint8_t hardware_type;
    uint8_t hardware_id;
    uint8_t hardware_prop;
    double value;
  } data;
};

enum StatusProperty {
  SPROP_ENC = 0,
  SPROP_DIG = 1,
};

HardwareManager::HardwareManager()
: recv_thread([&]() { this->recv_thread_main(); }), s(IPC_PATHNAME, 'S') { }

HardwareManager::~HardwareManager() {
  terminate();
  stop();
}

void HardwareManager::send_status_msgs() {
  IPCStatusMessage msg;
  msg.mtype = 1;
  // TalonFXs.
  msg.data.hardware_type = HTYPE_CAN_TALON_FX;
  msg.data.hardware_prop = SPROP_ENC;
  for (const auto& [id, t] : talon_fxs) {
    msg.data.hardware_id = id;
    msg.data.value = t->GetSelectedSensorPosition(0);
    s.send_msg(msg);
  }
  // Encoders.
  msg.data.hardware_type = HTYPE_ENC;
  for (const auto& [id, e] : encoders) {
    msg.data.hardware_id = id;
    msg.data.value = e->get();
    s.send_msg(msg);
  }
  // Digital Inputs.
  msg.data.hardware_type = HTYPE_DIG_IN;
  msg.data.hardware_prop = SPROP_DIG;
  for (const auto& [id, i] : digital_inputs) {
    msg.data.hardware_id = id;
    msg.data.value = static_cast<double>(i->get());
    s.send_msg(msg);
  }
}

void HardwareManager::process_hardware() {
  std::lock_guard<std::mutex> lk(hardware_mut);
  if (enabled) {
    // Feed CTRE enabled status.
    ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
  }
}

void HardwareManager::terminate() {
  std::lock_guard<std::mutex> lk(manager_mut);
  should_term = true;
}

void HardwareManager::stop() {
  std::lock_guard<std::mutex> lk(hardware_mut);
  for (const auto& [id, t] : talon_fxs) {
    t->Set(TalonFXControlMode::PercentOutput, 0.0);
  }
  for (const auto& [id, s] : spark_maxes) {
    s->set(0.0);
  }
  for (const auto& [id, p] : digital_outputs) {
    const auto& [o, s] = p;
    // Set to default position, if has default position.
    if (s) {
      o->set(static_cast<bool>(s - 1));
    }
  }
}

void HardwareManager::recv_thread_main() {
  IPCReceiver r(IPC_PATHNAME, 'C');
  
  {
    std::lock_guard<std::mutex> lk(hardware_mut);
    // Start Phoenix background tasks.
    ctre::phoenix::unmanaged::Unmanaged::LoadPhoenix();
    // Start Phoenix diagnostic server immediately.
    ctre::phoenix::unmanaged::Unmanaged::SetPhoenixDiagnosticsStartTime(0);
  }
  
  while (true) {
    {
      std::lock_guard<std::mutex> lk(manager_mut);
      if (should_term) return;
    }
    
    IPCControlMessage msg;
    if (!r.recv_msg(&msg)) break;
    
    // Message type.
    if (msg.mtype != MSG_CONTROL) {
      if (msg.mtype == MSG_RESET) {
        stop();
        std::lock_guard<std::mutex> lk(hardware_mut);
        spark_maxes.clear();
        talon_fxs.clear();
        encoders.clear();
        digital_inputs.clear();
        digital_outputs.clear();
      }
      else {
        if (msg.mtype == MSG_DISABLE) {
          stop();
        }
        std::lock_guard<std::mutex> lk(hardware_mut);
        enabled = (msg.mtype == MSG_ENABLE);
      }
      return;
    }
    
    // Init hardware.
    const auto& [t, id, p, v] = msg.data;
    if (p == CPROP_INIT) {
      std::lock_guard<std::mutex> lk(hardware_mut);
      switch (t) {
        case HTYPE_DIG_IN:
          digital_inputs[id] = std::make_unique<DigitalInput>(id);
          break;
        case HTYPE_DIG_OUT:
          {
            auto& [o, s] = (digital_outputs[id] = std::make_pair(std::make_unique<DigitalOutput>(id), static_cast<int>(v)));
            // Set to default position, if has default position.
            if (s) {
              o->set(static_cast<bool>(s - 1));
            }
          }
          break;
        case HTYPE_CAN_TALON_FX:
          {
            auto& t = (talon_fxs[id] = std::make_unique<TalonFX>(id));
            // Set to stopped.
            t->Set(TalonFXControlMode::PercentOutput, 0.0);
          }
          break;
        case HTYPE_PWM_SPARK_MAX:
          {
            auto& s = (spark_maxes[id] = std::make_unique<PWMSparkMax>(id));
            // Set to stopped.
            s->set(0.0);
          }
          break;
        case HTYPE_ENC:
          encoders[id] = std::make_unique<DutyCycleEncoder>(id);
          break;
      }
      continue;
    }
    
    auto warn_invalid_prop = [&p, &t]() {
      fmt::print("Control error: Invalid hardware property of type {} used on hardware of type {}.\n", p, t);
    };
    auto check_init = [&id, &t](const auto& m) -> bool {
      if (m.count(id)) return true;
      fmt::print("Control error: Hardware id of {} of type {} was never initialized.", id, t);
      return false;
    };
    
    // Control hardware.
    std::lock_guard<std::mutex> lk(hardware_mut);
    switch (t) {
      case HTYPE_DIG_IN:
      case HTYPE_ENC:
        // Sensors that cannot be controlled.
        warn_invalid_prop();
        break;
      case HTYPE_DIG_OUT:
        if (p != CPROP_DIG) {
          warn_invalid_prop();
          break;
        }
        if (check_init(digital_outputs)) {
          digital_outputs.at(id).first->set(static_cast<bool>(v));
        }
        break;
      case HTYPE_CAN_TALON_FX:
        if (!check_init(talon_fxs)) break;
        if (p == CPROP_PCT) {
          talon_fxs.at(id)->Set(TalonFXControlMode::PercentOutput, v);
        }
        else if (p == CPROP_POS) {
          talon_fxs.at(id)->Set(TalonFXControlMode::Position, v);
        }
        else warn_invalid_prop();
        break;
      case HTYPE_PWM_SPARK_MAX:
        if (p != CPROP_PCT) {
          warn_invalid_prop();
          break;
        }
        if (check_init(spark_maxes)) {
          spark_maxes.at(id)->set(v);
        }
        break;
    }
  }
}

HardwareManager HardwareManager::instance;
