#pragma once

#include <string>

class MotorControllerManager;

class MotorController {
public:
  /**
   * @brief Sends a percent output to the motor controller.
   *
   * @param percent The percent output (-1 to 1).
   */
  virtual void set(double percent) = 0;
  
  /**
   * @brief Sets whether the motor controller should be controlled inversely.
   *
   * @param inv Whether the motor is inverted.
   */
  constexpr void set_inverted(bool inv) { inverted = inv; }
  
  /**
   * @brief Returns whether the motor controller is being controlled inversely.
   *
   * @return Whether the motor is inverted.
   */
  constexpr bool get_inverted() const { return inverted; }
  
protected:
  MotorController(std::string key);
  ~MotorController();
  
  bool inverted = false;
  bool enabled = false;

private:
  friend class MotorControllerManager;
  
  /**
   * @brief Sets whether the motor controller is enabled.
   *
   * @return Whether the motor controller is enabled.
   */
  constexpr void set_enabled(bool en) { enabled = en; }
  
  /**
   * @brief Returns whether the motor controller is enabled.
   *
   * @return Whether the motor controller is enabled.
   */
  constexpr bool get_enabled() { return enabled; }
  
  std::string key;
};
