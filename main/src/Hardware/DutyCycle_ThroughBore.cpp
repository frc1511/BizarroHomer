#include <BizarroHomer/Hardware/DutyCycle_ThroughBore.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::ABS_THROUGH_BORE

DutyCycle_ThroughBore::DutyCycle_ThroughBore(int _channel)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, 0.0);
  
  // Register status callback.
  HardwareManager::get()->register_status_callback(HARDWARE_TYPE, channel, HardwareManager::StatusProperty::VALUE, [&](double v) {
    std::lock_guard<std::mutex> lk(pos_mut);
    angle = v;
  });
}

DutyCycle_ThroughBore::~DutyCycle_ThroughBore() { }

double DutyCycle_ThroughBore::get_angle() {
  std::lock_guard<std::mutex> lk(pos_mut);
  return angle;
}
