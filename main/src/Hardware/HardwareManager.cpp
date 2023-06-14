#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>
#include <chrono>

enum ControlMessageType {
  MSG_CONTROL = 1,
  MSG_ENABLE  = 2,
  MSG_DISABLE = 3,
  MSG_RESET   = 4,
};

struct IPCControlMessage {
  long mtype;
  struct Data {
    uint8_t control_type;
    uint8_t hardware_type;
    uint8_t hardware_id;
    uint8_t hardware_prop;
    double value;
  } data;
};

struct IPCStatusMessage {
  long mtype = 1;
  struct Data {
    uint8_t hardware_type;
    uint8_t hardware_id;
    uint8_t hardware_prop;
    double value;
  } data;
};

HardwareManager::HardwareManager()
: s(IPC_PATHNAME, 'C'), status_thread([this]() { this->status_thread_main(); }) { }

HardwareManager::~HardwareManager() { }

void HardwareManager::set_enabled(bool enabled) {
  IPCControlMessage msg;
  std::memset(&msg, 0, sizeof(IPCControlMessage));
  // Set the message type.
  msg.mtype = 1;
  msg.data.control_type = enabled ? MSG_ENABLE : MSG_DISABLE;
  
  // Send the message.
  std::lock_guard<std::mutex> lk(control_mutex);
  s.send_msg(msg);
}

void HardwareManager::reset_hardware() {
  IPCControlMessage msg;
  std::memset(&msg, 0, sizeof(IPCControlMessage));
  // Set the message type to reset.
  msg.mtype = 1;
  msg.data.control_type = MSG_RESET;
  
  // Send the message.
  std::lock_guard<std::mutex> lk(control_mutex);
  s.send_msg(msg);
}

void HardwareManager::send_ctrl_msg(HardwareType type, uint8_t id, ControlProperty prop, double value) {
  IPCControlMessage msg;
  msg.mtype = 1;
  msg.data.control_type = MSG_CONTROL;
  msg.data.hardware_type = static_cast<uint8_t>(type);
  msg.data.hardware_id = id;
  msg.data.hardware_prop = static_cast<uint8_t>(prop);
  msg.data.value = value;
  
  // Send the message.
  std::lock_guard<std::mutex> lk(control_mutex);
  s.send_msg(msg);
}

void HardwareManager::register_status_callback(HardwareType type, uint8_t id, StatusProperty prop, StatusCallbackFunc callback) {
  std::lock_guard<std::mutex> lk(status_mutex);
  status_callbacks.emplace_back(StatusCallbackID { type, id, prop }, callback);
}

void HardwareManager::status_thread_main() {
  IPCReceiver r(IPC_PATHNAME, 'S');
  
  while (true) {
    IPCStatusMessage msg;
    if (!r.recv_msg(&msg)) break;
    
    // Find callback.
    std::lock_guard<std::mutex> lk(status_mutex);
    decltype(status_callbacks)::const_iterator it = std::find_if(status_callbacks.cbegin(), status_callbacks.cend(), [&msg](auto p) {
      return msg.data.hardware_type == static_cast<uint8_t>(p.first.type) &&
             msg.data.hardware_id   == p.first.id &&
             msg.data.hardware_prop == static_cast<uint8_t>(p.first.prop);
    });
    
    if (it != status_callbacks.cend()) {
      it->second(msg.data.value);
    }
  }
}

HardwareManager HardwareManager::instance;
