#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <filesystem>
#include <string>
#include <optional>

namespace thunder {

class DigitalOutput : public HardwareComponent {
public:
  DigitalOutput(int channel, std::optional<bool> default_set = std::nullopt);
  ~DigitalOutput();
  
  void set(bool output);
  bool get();
  
private:
  int m_channel;
  std::optional<bool> m_default_set;
  
  std::string m_channel_str;
  std::filesystem::path m_channel_path;
  std::filesystem::path m_channel_dir_path, m_channel_value_path;
};

} // namespace thunder
