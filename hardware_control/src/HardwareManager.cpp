#include <BizarroHomerHardwareControl/HardwareManager.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <ctre/phoenix/unmanaged/Unmanaged.h>
#include <ctre/phoenix/cci/PDP_CCI.h>
#include <fmt/core.h>

struct IPCStatusMessage {
  long mtype = 1;
  struct Data {
    uint8_t hardware_type;
    uint8_t hardware_id;
    uint8_t hardware_prop;
    double value;
  } data;
};

enum StatusProperty {
  SPROP_VAL = 0,
};

HardwareManager::HardwareManager()
: s(IPC_PATHNAME, 'S'), recv_thread([&]() { this->recv_thread_main(); }) { }

HardwareManager::~HardwareManager() {
  terminate();
  stop();
}

void HardwareManager::send_status_msgs() {
  IPCStatusMessage msg;
  msg.mtype = 1;
  // TalonFXs.
  msg.data.hardware_type = HTYPE_CAN_TALON_FX;
  msg.data.hardware_prop = SPROP_VAL;
  for (const auto& [id, t] : talon_fxs) {
    msg.data.hardware_id = id;
    
    // 2048 ticks per rotation.
    double rotations = t->GetSelectedSensorPosition(0) / 2048.0;
    
    msg.data.value = rotations;
    s.send_msg(msg);
  }
  // Encoders.
  msg.data.hardware_type = HTYPE_ABS_THROUGH_BORE;
  msg.data.hardware_prop = SPROP_VAL;
  for (const auto& [id, through_bore] : through_bores) {
    msg.data.hardware_id = id;
    msg.data.value = through_bore->get_angle();
    s.send_msg(msg);
  }
  // Digital Inputs.
  msg.data.hardware_type = HTYPE_DIG_IN;
  msg.data.hardware_prop = SPROP_VAL;
  for (const auto& [id, input] : digital_inputs) {
    msg.data.hardware_id = id;
    msg.data.value = static_cast<double>(input->get());
    s.send_msg(msg);
  }
  // PDP Voltage.
  if (pdp_can_id != -1) {
    double voltage = -1;
    double* currents = {};
    int filled = 0;
    
    int err = (int)c_PDP_GetValues(pdp_can_id, &voltage, currents, 0, &filled);
    if (err == 0) {
      msg.data.hardware_type = HTYPE_CAN_PDP;
      msg.data.hardware_prop = SPROP_VAL;
      msg.data.hardware_id = 0;
      msg.data.value = voltage;
      s.send_msg(msg);
    }
  }
}

struct Song {
  std::string primary;
  std::string secondary;
  
  inline Song(std::string _primary, std::string _secondary)
  : primary(_primary), secondary(_secondary) { }
  
  inline Song(std::string _primary)
  : primary(_primary), secondary(_primary) { }
};

static const std::map<uint8_t, Song> all_songs = {
  { 0, Song("home_depot_beat_high.chrp", "home_depot_beat_low.chrp") },
  { 1, Song("thunderstruck.chrp") },
};

void HardwareManager::process_hardware() {
  std::lock_guard<std::mutex> lk(hardware_mut);
  if (!enabled) {
    return;
  }
  
  // Feed CTRE enabled status.
  ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
  
  if (playing_music) {
    // Starting from the beginning.
    if (!orchestra_primary.IsPlaying()) {
      const Song& song = all_songs.at(which_song);
      orchestra_primary.LoadMusic(song.primary);
      orchestra_secondary.LoadMusic(song.secondary);
      
      orchestra_primary.Play();
      orchestra_secondary.Play();
    }
    // Finished the song.
    if (orchestra_primary.GetCurrentTime() > 30000) {
      playing_music = false;
      
      orchestra_primary.Stop();
      orchestra_secondary.Stop();
    }
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
  for (const auto& [id, digital_ouput] : digital_outputs) {
    const auto& [output, default_pos] = digital_ouput;
    // Set to default position, if it has a default position.
    if (default_pos) {
      output->set(static_cast<bool>(default_pos - 1));
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
    
    handle_control_msg(msg);
  }
}

void HardwareManager::handle_control_msg(IPCControlMessage& msg) {
  // Message type.
  if (msg.data.control_type != MSG_CONTROL) {
    if (msg.data.control_type == MSG_RESET) {
      stop();
      std::lock_guard<std::mutex> lk(hardware_mut);
      spark_maxes.clear();
      talon_fxs.clear();
      through_bores.clear();
      digital_inputs.clear();
      digital_outputs.clear();
      fmt::print("Reset message received.\n");
    }
    else {
      if (msg.data.control_type == MSG_DISABLE) {
        stop();
      }
      std::lock_guard<std::mutex> lk(hardware_mut);
      enabled = (msg.data.control_type == MSG_ENABLE);
      fmt::print("{} message received.\n", enabled ? "Enable" : "Disable");
    }
    return;
  }
  
  // Init hardware.
  auto [control_type, hardware_type, hardware_id, hardware_prop, value] = msg.data;
  if (hardware_prop == CPROP_INIT) {
    init_hardware(static_cast<HardwareType>(hardware_type), hardware_id, value);
  }
  else if (hardware_prop == CPROP_MUSIC) {
    start_music(value);
  }
  
  if (!enabled) {
    stop();
    return;
  }
  
  control_hardware(static_cast<ControlProperty>(hardware_prop), static_cast<HardwareType>(hardware_type), hardware_id, value);
}

void HardwareManager::init_hardware(HardwareType type, uint8_t id, double value) {
  std::lock_guard<std::mutex> lk(hardware_mut);
  
  if (type == HTYPE_DIG_IN) {
    digital_inputs[id] = std::make_unique<DigitalInput>(id);
    fmt::print("Intialized DigitalInput at channel: {}\n", id);
  }
  else if (type == HTYPE_DIG_OUT) {
    digital_outputs[id] = std::make_pair(std::make_unique<DigitalOutput>(id), static_cast<int>(value));
    
    auto& [output, default_pos] = digital_outputs[id];
    fmt::print("Intialized DigitalOutput at channel: {}\n", id);
    
    // Set to default position, if has default position.
    if (default_pos) {
      output->set(static_cast<bool>(default_pos - 1));
    }
  }
  else if (type == HTYPE_CAN_TALON_FX) {
    talon_fxs[id] = std::make_unique<TalonFX>(id);
    
    auto& talon = talon_fxs[id];
    fmt::print("Intialized CAN TalonFX at CAN ID: {}\n", id);
    
    // Get config...
    TalonFXConfiguration configs;
    talon->GetAllConfigs(configs);
    
    // Set feedback sensor as the integrated sensor.
    configs.primaryPID.selectedFeedbackSensor = FeedbackDevice::IntegratedSensor;
    
    // Apply settings.
    talon->ConfigAllSettings(configs);
    
    // Set position to 0.
    talon->SetSelectedSensorPosition(0);
    
    // Set to stopped.
    talon->Set(TalonFXControlMode::PercentOutput, 0.0);
    
    // Add orchestra instrument!
    auto talon_num = talon_fxs.size();
    if (talon_num == 2) {
      orchestra_secondary.AddInstrument(*talon);
    }
    else {
      orchestra_primary.AddInstrument(*talon);
    }
  }
  else if (type == HTYPE_PWM_SPARK_MAX) {
    spark_maxes[id] = std::make_unique<PWMSparkMax>(id);
    
    auto& spark_max = spark_maxes[id];
    fmt::print("Intialized PWM SparkMax at pwm{}\n", id);
    
    // Set to stopped.
    spark_max->set(0.0);
  }
  else if (type == HTYPE_ABS_THROUGH_BORE) {
    through_bores[id] = std::make_unique<DutyCycleThroughBore>(id);
    fmt::print("Intialized Through Bore Encoder at channel: {}\n", id);
  }
  else if (type == HTYPE_CAN_PDP) {
    if (pdp_can_id != -1) {
      fmt::print("Init error: PDP already initialized!\n");
    }
    pdp_can_id = id;
  }
}

void HardwareManager::start_music(double value) {
  std::lock_guard<std::mutex> lk(hardware_mut);
  
  playing_music = true;
  which_song = static_cast<uint8_t>(value);
  
  // Stop all Falcons from moving since they need to play music.
  for (auto& [id, talon_fx] : talon_fxs) {
    talon_fxs.at(id)->Set(TalonFXControlMode::PercentOutput, 0.0);
  }
}

void HardwareManager::control_hardware(ControlProperty prop, HardwareType type, uint8_t id, double value) {
  auto warn_invalid_prop = [&prop, &type]() {
    fmt::print("Control error: Invalid hardware property of type {} used on hardware of type {}.\n", static_cast<uint8_t>(prop), static_cast<uint8_t>(type));
  };
  auto check_init = [&id, &type](const auto& m) -> bool {
    if (m.count(id)) return true;
    fmt::print("Control error: Hardware id of {} of type {} was never initialized.", id, static_cast<uint8_t>(type));
    return false;
  };
  
  std::lock_guard<std::mutex> lk(hardware_mut);
  
  if (type == HTYPE_DIG_IN ||
      type == HTYPE_ABS_THROUGH_BORE ||
      type == HTYPE_CAN_PDP) {
    
    // Sensors that cannot be controlled.
    warn_invalid_prop();
  }
  else if (type == HTYPE_DIG_OUT) {
    if (prop != CPROP_DIG) {
      warn_invalid_prop();
    }
    else if (check_init(digital_outputs)) {
      digital_outputs.at(id).first->set(static_cast<bool>(value));
    }
  }
  else if (type == HTYPE_CAN_TALON_FX) {
    if (!check_init(talon_fxs)) return;
    
    // Don't move the motors when playing music!
    if (playing_music) {
      prop = CPROP_PCT;
      value = 0.0;
    }
    
    if (prop == CPROP_PCT) {
      talon_fxs.at(id)->Set(TalonFXControlMode::PercentOutput, value);
    }
    else if (prop == CPROP_POS) {
      talon_fxs.at(id)->Set(TalonFXControlMode::Position, value);
    }
    else {
      warn_invalid_prop();
    }
  }
  else if (type == HTYPE_PWM_SPARK_MAX) {
    if (prop != CPROP_PCT) {
      warn_invalid_prop();
    }
    else if (check_init(spark_maxes)) {
      spark_maxes.at(id)->set(value);
    }
  }
}

HardwareManager HardwareManager::instance;
