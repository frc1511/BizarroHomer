#pragma once

#include <BizarroHomer/Drivers/PWM.hpp>

class PWMMotorController : public PWM {
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
  void set(double percent);
  
  /**
   * @brief Sets whether the motor controller should be controlled inversely.
   *
   * @param inverted Whether the motor is inverted.
   */
  constexpr void set_inverted(bool inverted) { inv = inverted; }
  
  /**
   * @brief Returns whether the motor controller is being controlled inversely.
   *
   * @return Whether the motor is inverted.
   */
  constexpr bool get_inverted() const { return inv; }
  
private:
  int channel;
  std::string dir;
  
  int max, max_deadband, center, min_deadband, min;
  
  bool inv = false;
};
