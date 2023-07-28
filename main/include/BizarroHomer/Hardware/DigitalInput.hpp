#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <filesystem>
#include <string>

namespace thunder {

class DigitalInput : public HardwareComponent {
public:
  DigitalInput(int channel);
  ~DigitalInput();
  
  bool get();
  
private:
  int m_channel;
  
  std::string m_channel_str;
  std::filesystem::path m_channel_path;
  std::filesystem::path m_channel_dir_path, m_channel_value_path;
};

} // namespace thunder
