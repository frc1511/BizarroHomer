#pragma once

#include <BizarroHomer/Hardware/PWM.hpp>
#include <BizarroHomer/Hardware/MotorControl/MotorController.hpp>

class PWMMotorController : public MotorController {
public:
  /**
   * @brief Creates a PWMMotorController class instance.
   *
   * @param channel The PWM channel to use.
   * @param max The max pulse width (ns).
   * @param max_deadband The high end of the deadband range pulse width (ns).
   * @param center The center (off) pulse width (ns).
   * @param min_deadband The low end of the deadband range pulse width (ns).
   * @param min The min pulse width (ns).
   * @param freq The PWM frequency (Hz).
   */
  PWMMotorController(int channel, int max, int max_deadband, int center, int min_deadband, int min, int freq = 50);
  
  virtual ~PWMMotorController() = default;
  
  /**
   * @brief Sends a percent output to the motor controller, based on the
   *        configured pulse width range.
   *
   * @param percent The percent output (-1 to 1).
   */
  void set(double percent) override;
  
private:
  PWM pwm;
  
  int max, max_deadband, center, min_deadband, min;
};
