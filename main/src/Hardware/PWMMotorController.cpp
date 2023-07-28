#include <BizarroHomer/Hardware/PWMMotorController.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <fmt/core.h>

#define PERCENT_TOLERANCE 0.001

thunder::PWMMotorController::PWMMotorController(int channel, int max, int max_deadband, int center, int min_deadband, int min, int freq)
: m_pwm(channel), m_max(max), m_max_deadband(max_deadband), m_center(center), m_min_deadband(min_deadband), m_min(min) {
  
  // Set period.
  int period = 1e9 / freq;
  m_pwm.set_period(period);
  
  // Enable PWM.
  m_pwm.set_enabled(true);
  
  stop();
  
  HardwareManager::get()->register_hardware(this);
}

void thunder::PWMMotorController::stop() {
  set(0.0);
}

void thunder::PWMMotorController::set_enabled(bool enabled) {
  m_enabled = enabled;
  stop();
}

void thunder::PWMMotorController::set(double percent) {
  if (!m_enabled) {
    stop();
    return;
  }
  
  percent = std::clamp(percent, -1.0, 1.0);
  
  int duty_cycle = m_center;
  
  if (percent >= PERCENT_TOLERANCE) {
    duty_cycle = m_max_deadband + static_cast<int>(percent * (m_max - m_max_deadband));
  }
  else if (percent <= -PERCENT_TOLERANCE) {
    duty_cycle = m_min_deadband - static_cast<int>(percent * -1.0 * (m_min_deadband - m_min));
  }
  
  m_pwm.set_duty_cycle(duty_cycle);
}
