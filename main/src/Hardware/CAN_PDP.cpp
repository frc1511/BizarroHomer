#include <BizarroHomer/Hardware/CAN_PDP.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::CAN_PDP

CAN_PDP::CAN_PDP(int _can_id)
: can_id(_can_id) {
  // Init the PDP.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, can_id, HardwareManager::ControlProperty::INIT, 0.0);
  
  // Register status callback.
  HardwareManager::get()->register_status_callback(HARDWARE_TYPE, can_id, HardwareManager::StatusProperty::VALUE, [&](double v) {
    std::lock_guard<std::mutex> lk(voltage_mut);
    voltage = v;
  });
}

CAN_PDP::~CAN_PDP() { }

double CAN_PDP::get_voltage() {
  std::lock_guard<std::mutex> lk(voltage_mut);
  return voltage;
}
