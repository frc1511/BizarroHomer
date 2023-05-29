#include <BizarroHomer/Hardware/MotorControl/PWMSparkMax.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::PWM_SPARK_MAX

PWMSparkMax::PWMSparkMax(int _channel)
: channel(_channel) {
  // Init the motor.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::INIT, 0.0);
}

PWMSparkMax::~PWMSparkMax() { }

void PWMSparkMax::set_percent(double percent) {
  // Send percent output.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, channel, HardwareManager::ControlProperty::PERCENT, percent);
}
