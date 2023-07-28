#pragma once

#include <string>
#include <filesystem>

namespace thunder {

// Represents a PWM device.
class PWM {
public:
  PWM(int channel);
  ~PWM();
  
  void set_duty_cycle(int duty_cycle);
  void set_period(int period);
  void set_enabled(bool enabled);
  
private:
  int m_channel;
  
  std::string m_channel_str;
  std::filesystem::path m_channel_path;
  std::filesystem::path m_enable_path, m_period_path, m_duty_cycle_path;
};

} // namespace thunder
