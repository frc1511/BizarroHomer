#include <BizarroHomer/Hardware/DigitalIO/DigitalOutput.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::DIGITAL_OUT

DigitalOutput::DigitalOutput(int _channel, std::optional<bool> default_set)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, default_set ? (*default_set + 1) : 0);
  if (default_set) {
    output = *default_set;
  }
}

DigitalOutput::~DigitalOutput() { }

void DigitalOutput::set(bool _output) {
  output = _output;
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::DIGITAL, output);
}

bool DigitalOutput::get() {
  return output;
}
