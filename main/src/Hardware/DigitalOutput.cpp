#include <BizarroHomer/Hardware/DigitalOutput.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <fmt/core.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

thunder::DigitalOutput::DigitalOutput(int channel, std::optional<bool> default_set)
: m_channel(channel),
  m_default_set(default_set),
  m_channel_str(std::to_string(channel)),
  m_channel_path(fmt::format("/sys/class/gpio/gpio{}", channel)),
  m_channel_dir_path(m_channel_path / "direction"),
  m_channel_value_path(m_channel_path / "value") {
  
  // Write the channel number to the export file.
  int export_fd = open("/sys/class/gpio/export", O_WRONLY);
  write(export_fd, m_channel_str.c_str(), m_channel_str.size());
  close(export_fd);
  
  // Set the GPIO direction to input.
  int dir_fd = open(m_channel_dir_path.c_str(), O_WRONLY);
  write(dir_fd, "out", 3);
  close(dir_fd);
  
  stop();
  
  // Register this hardware component.
  HardwareManager::get()->register_hardware(this);
}

thunder::DigitalOutput::~DigitalOutput() {
  stop();
  
  // Write the channel number to the unexport file.
  int export_fd = open("/sys/class/gpio/unexport", O_WRONLY);
  write(export_fd, m_channel_str.c_str(), m_channel_str.size());
  close(export_fd);
}

void thunder::DigitalOutput::stop() {
  if (m_default_set) {
    set(*m_default_set);
  }
}

void thunder::DigitalOutput::set_enabled(bool enabled) {
  m_enabled = enabled;
  stop();
}

void thunder::DigitalOutput::set(bool output) {
  if (!m_enabled) {
    stop();
    return;
  }

  int fd = open(m_channel_value_path.c_str(), O_WRONLY);
  
  char value = output ? '1' : '0';
  write(fd, &value, 1);
  
  close(fd);
}

bool thunder::DigitalOutput::get() {
  int fd = open(m_channel_value_path.c_str(), O_RDONLY);
  
  char value = 0;
  read(fd, &value, 1);
  
  close(fd);
  
  return value == '1';
}
