#pragma once

#include <string>
#include <filesystem>

// Represents a PWM device.
class PWM {
public:
  /**
   * @brief Creates a PWM class instance.
   *
   * @param channel The PWM channel to use.
   */
  PWM(int channel);
  
  /**
   * @brief Sets the duty cycle of the PWM channel.
   *
   * @param duty_cycle The duty cycle (ns).
   */
  void set_duty_cycle(int duty_cycle);
  
  /**
   * @brief Sets the period of the PWM channel.
   *
   * @param period The period (ns).
   */
  void set_period(int period);

  /**
   * @brief Enables/Disables the PWM channel.
   *
   * @param enbaled Whether the PWM channel is enabled.
   */
  void set_enabled(bool enabled);
  
private:
  int channel;
  
  std::filesystem::path channel_dir;
  std::filesystem::path enable_path, period_path, duty_cycle_path;
};
