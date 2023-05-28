#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>
#include <chrono>

#define IPC_PATHNAME "/var/frc1511/BizarroHomer/ipc_msg_queue_key"

struct IPCControlMessage {
  long mtype;
  HardwareManager::ControlData data;
};

#define IPC_STATUS_MSG_SIZE 33

struct IPCStatusMessage {
  long mtype;
  HardwareManager::StatusData data;
};

HardwareManager::HardwareManager()
: control_thread([&]() { this->control_thread_main(); }), status_thread([&]() { this->status_thread_main(); }) { }

HardwareManager::~HardwareManager() { }

void HardwareManager::set_enabled(bool enabled) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.enabled = enabled;
}

void HardwareManager::set_fill_valve(bool closed) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.fill_valve = closed;
}

void HardwareManager::set_shoot_valve(bool closed) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.shoot_valve = closed;
}

void HardwareManager::set_drive_left(double percent) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.drive_left = percent;
}

void HardwareManager::set_drive_right(double percent) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.drive_right = percent;
}

void HardwareManager::set_pivot_left(double position) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.pivot_left = position;
}

void HardwareManager::set_pivot_right(double position) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.pivot_right = position;
}

void HardwareManager::set_shooter_rotation(double position) {
  std::lock_guard<std::mutex> lk(control_mutex);
  control_data.shooter_rot = position;
}

bool HardwareManager::get_enabled() {
  std::lock_guard<std::mutex> lk(status_mutex);
  return status_data.enabled;
}

bool HardwareManager::get_pressure_switch() {
  std::lock_guard<std::mutex> lk(status_mutex);
  return status_data.pressure_switch;
}

double HardwareManager::get_pivot_left_encoder() {
  std::lock_guard<std::mutex> lk(status_mutex);
  return status_data.pivot_left_enc;
}

double HardwareManager::get_pivot_right_encoder() {
  std::lock_guard<std::mutex> lk(status_mutex);
  return status_data.pivot_right_enc;
}

double HardwareManager::get_shooter_rotation_encoder() {
  std::lock_guard<std::mutex> lk(status_mutex);
  return status_data.shooter_rot_enc;
}

double HardwareManager::get_shooter_rotation_absolute_encoder() {
  std::lock_guard<std::mutex> lk(status_mutex);
  return status_data.shooter_rot_abs_enc;
}

void HardwareManager::control_thread_main() {
  return;
  std::memset(&control_data, 0, sizeof(ControlData));
  control_data.fill_valve = true;
  control_data.shoot_valve = true;
  control_data.drive_left = 0x80;
  control_data.drive_right = 0x80;
  
  IPCSender s(IPC_PATHNAME, 'C');
  
  while (true) {
    IPCControlMessage msg;
    msg.mtype = 1;
    {
      std::lock_guard<std::mutex> lk(control_mutex);
      msg.data = control_data;
    }
    
    if (!s.send_msg(msg)) return;
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
}

void HardwareManager::status_thread_main() {
  return;
  std::memset(&status_data, 0, sizeof(StatusData));
  
  IPCReceiver r(IPC_PATHNAME, 'S');
  
  while (true) {
    IPCStatusMessage msg;
    std::memset(&msg, 0, sizeof(IPCControlMessage));
    
    if (!r.recv_msg(&msg)) return;
    
    {
      std::lock_guard<std::mutex> lk(status_mutex);
      status_data = msg.data;
    }
  }
}

HardwareManager HardwareManager::instance;
