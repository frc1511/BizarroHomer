#include <BizarroHomer/Hardware/DigitalIO/DigitalOutput.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::DIGITAL_OUT

DigitalOutput::DigitalOutput(int _channel, std::optional<bool> default_set)
: channel(_channel) {
  // Init.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, default_set ? (*default_set + 1) : 0);
}

DigitalOutput::~DigitalOutput() { }

void DigitalOutput::set(bool output) {
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::DIGITAL, output);
}
