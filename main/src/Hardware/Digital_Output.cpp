#include <BizarroHomer/Hardware/Digital_Output.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::DIGITAL_OUT

Digital_Output::Digital_Output(int _channel, std::optional<bool> default_set)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, default_set ? (*default_set + 1) : 0);
  if (default_set) {
    output = *default_set;
  }
}

Digital_Output::~Digital_Output() { }

void Digital_Output::set(bool _output) {
  output = _output;
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::DIGITAL, output);
}

bool Digital_Output::get() {
  return output;
}
