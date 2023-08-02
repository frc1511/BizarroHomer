#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <filesystem>
#include <string>
#include <optional>

namespace thunder {

class DigitalOutput : public HardwareComponent {
public:
  //
  // Creates a new digital output on the given channel.
  //
  // If default_set is set, the output will be set to that value when the
  // DigitalOutput is created, destroyed, or set to disabled.
  //
  DigitalOutput(int channel, std::optional<bool> default_set = std::nullopt);
  ~DigitalOutput();
  
  void stop() override;
  void set_enabled(bool enabled) override;
  
  //
  // Sets the value of the digital output.
  //
  void set(bool output);
  
  //
  // Reads the current value of the digital output.
  //
  bool get();
  
private:
  int m_channel;
  std::optional<bool> m_default_set;
  
  std::string m_channel_str;
  std::filesystem::path m_channel_path;
  std::filesystem::path m_channel_dir_path, m_channel_value_path;
  
  bool m_enabled = true;
};

} // namespace thunder
