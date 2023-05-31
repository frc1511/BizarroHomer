#include <BizarroHomerHardwareControl/Hardware/DigitalInput.hpp>
#include <fmt/core.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

DigitalInput::DigitalInput(int _channel) 
: channel(_channel),
  channel_dir(fmt::format("/sys/class/gpio/gpio{}", channel)),
  dir_path(channel_dir / "direction"),
  val_path(channel_dir / "value") {
  
  std::string channel_str(fmt::format("{}", channel));
  int export_fd = open("/sys/class/gpio/export", O_WRONLY | O_CREAT | O_TRUNC);
  write(export_fd, channel_str.c_str(), channel_str.length());
  close(export_fd);
  
  int dir_fd = open(dir_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  write(dir_fd, "in", 2);
  close(dir_fd);
}

DigitalInput::~DigitalInput() { }

bool DigitalInput::get() {
  int val_fd = open(val_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  char buf[1];
  read(val_fd, buf, 1);
  close(val_fd);
  return *buf;
}

