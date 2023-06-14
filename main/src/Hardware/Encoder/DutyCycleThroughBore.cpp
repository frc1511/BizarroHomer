#include <BizarroHomer/Hardware/Encoder/DutyCycleThroughBore.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::ABS_THROUGH_BORE

DutyCycleThroughBore::DutyCycleThroughBore(int _channel)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, 0.0);
  
  // Register status callback.
  HardwareManager::get()->register_status_callback(HARDWARE_TYPE, channel, HardwareManager::StatusProperty::ANGLE, [&](double v) {
    std::lock_guard<std::mutex> lk(pos_mut);
    angle = v;
  });
}

DutyCycleThroughBore::~DutyCycleThroughBore() { }

double DutyCycleThroughBore::get_angle() {
  std::lock_guard<std::mutex> lk(pos_mut);
  return angle;
}
