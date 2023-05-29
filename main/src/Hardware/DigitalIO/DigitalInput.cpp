#include <BizarroHomer/Hardware/DigitalIO/DigitalInput.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::DIGITAL_IN

DigitalInput::DigitalInput(int _channel)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, 0.0);
  
  // Register status callback.
  HardwareManager::get()->register_status_callback(HARDWARE_TYPE, channel, HardwareManager::StatusProperty::DIGITAL, [&](double v) {
    std::lock_guard<std::mutex> lk(input_mut);
    input = static_cast<bool>(v);
  });
}

DigitalInput::~DigitalInput() { }

bool DigitalInput::get() {
  std::lock_guard<std::mutex> lk(input_mut);
  return input;
}
