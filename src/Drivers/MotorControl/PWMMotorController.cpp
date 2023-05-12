#include <BizarroHomer/Drivers/MotorControl/PWMMotorController.hpp>
#include <fmt/core.h>
#include <cassert>

PWMMotorController::PWMMotorController(int channel, int _max, int _neutral, int _min)
: PWM(channel), max_width(_max), neutral_width(_neutral), min_width(_min) {
  set_period(2e7); // 20ms
  set(0.0);
  set_enabled(true);

  assert(max_width > neutral_width);
  assert(neutral_width > min_width);
  assert(max_width > min_width);
}

void PWMMotorController::set(double percent) {
  percent = std::clamp(percent, -1.0, 1.0);

  if (inv) percent *= -1.0;

  int dc = neutral_width;

  if (percent >= 0) {
    dc += static_cast<int>(percent * (max_width - neutral_width));
  }
  else {
    dc -= static_cast<int>(percent * -1.0 * (neutral_width - min_width));
  }

  set_duty_cycle(dc);
}

