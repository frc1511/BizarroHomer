#include <BizarroHomer/Hardware/MotorControl/PWMMotorController.hpp>
#include <fmt/core.h>

#define PERCENT_TOLERANCE 0.001

PWMMotorController::PWMMotorController(int channel, int _max, int _max_deadband, int _center, int _min_deadband, int _min, int freq)
: PWM(channel), max(_max), max_deadband(_max_deadband), center(_center), min_deadband(_min_deadband), min(_min) {
  
  // Set period.
  int period = 1e9 / freq;
  set_period(period);
  
  // Set neutral.
  set(0.0);

  // Enable PWM.
  set_enabled(true);
}

void PWMMotorController::set(double percent) {
  percent = std::clamp(percent, -1.0, 1.0);
  
  if (inv) percent *= -1.0;
  
  int dc = center;
  
  if (percent >= PERCENT_TOLERANCE) {
    dc = max_deadband + static_cast<int>(percent * (max - max_deadband));
  }
  else if (percent <= -PERCENT_TOLERANCE) {
    dc = min_deadband - static_cast<int>(percent * -1.0 * (min_deadband - min));
  }
  
  set_duty_cycle(dc);
}

