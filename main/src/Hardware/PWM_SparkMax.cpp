#include <BizarroHomer/Hardware/PWM_SparkMax.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::PWM_SPARK_MAX

PWM_SparkMax::PWM_SparkMax(int _channel)
: channel(_channel) {
  // Init the motor.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, 0.0);
}

PWM_SparkMax::~PWM_SparkMax() { }

void PWM_SparkMax::set_percent(double percent) {
  // Send percent output.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::PERCENT, percent);
}
