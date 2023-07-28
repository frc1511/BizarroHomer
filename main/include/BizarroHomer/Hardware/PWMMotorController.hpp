#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <BizarroHomer/Hardware/PWM.hpp>

namespace thunder {

class PWMMotorController : public HardwareComponent {
public:
  PWMMotorController(int channel, int max, int max_deadband, int center, int min_deadband, int min, int freq = 50);
  
  void stop() override;
  void set_enabled(bool enabled) override;
  
  //
  // Sets the percent output of the motor controller (-1 to 1). Based on the
  // configured pulse width range.
  //
  void set(double percent);
  
private:
  PWM m_pwm;
  
  bool m_enabled = true;
  
  int m_max, m_max_deadband, m_center, m_min_deadband, m_min;
};

} // namespace thunder
