#include <BizarroHomerHardwareControl/Hardware/DigitalOutput.hpp>
#include <fmt/core.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

DigitalOutput::DigitalOutput(int _channel)
: channel(_channel),
  channel_dir(fmt::format("/sys/class/gpio/gpio{}", channel)),
  dir_path(channel_dir / "direction"),
  val_path(channel_dir / "value") {
  
  std::string channel_str(fmt::format("{}", channel));
  int export_fd = open("/sys/class/gpio/export", O_WRONLY | O_CREAT | O_TRUNC);
  write(export_fd, channel_str.c_str(), channel_str.length());
  close(export_fd);
  
  int dir_fd = open(dir_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  write(dir_fd, "out", 3);
  close(dir_fd);
}

DigitalOutput::~DigitalOutput() { }

void DigitalOutput::set(bool output) {
  int val_fd = open(val_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  write(val_fd, output ? "1" : "0", 1);
  close(val_fd);
}

