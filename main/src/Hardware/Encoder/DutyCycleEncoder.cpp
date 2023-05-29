#include <BizarroHomer/Hardware/Encoder/DutyCycleEncoder.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::ENCODER

DutyCycleEncoder::DutyCycleEncoder(int _channel)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, 0.0);
  
  // Register status callback.
  HardwareManager::get()->register_status_callback(HARDWARE_TYPE, channel, HardwareManager::StatusProperty::ENCODER, [&](double v) {
    std::lock_guard<std::mutex> lk(pos_mut);
    pos = v;
  });
}

DutyCycleEncoder::~DutyCycleEncoder() { }

double DutyCycleEncoder::get() {
  std::lock_guard<std::mutex> lk(pos_mut);
  return pos;
}
