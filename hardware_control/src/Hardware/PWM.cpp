#include <BizarroHomerHardwareControl/Hardware/PWM.hpp>
#include <fmt/core.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

PWM::PWM(int _channel)
: channel(_channel),
  channel_dir(fmt::format("/sys/class/pwm/pwmchip0/pwm{}", channel)),
  enable_path(channel_dir / "enable"),
  period_path(channel_dir / "period"),
  duty_cycle_path(channel_dir / "duty_cycle") { }

void PWM::set_duty_cycle(int duty_cycle) {
  std::string msg(std::to_string(duty_cycle));
  
  int duty_cycle_fd = open(duty_cycle_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  write(duty_cycle_fd, msg.c_str(), msg.length());
  close(duty_cycle_fd);
}

void PWM::set_period(int period) {
  std::string msg(std::to_string(period));
  
  int period_fd = open(period_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  write(period_fd, msg.c_str(), msg.length());
  close(period_fd);
}

void PWM::set_enabled(bool enabled) {
  std::string msg(std::to_string(static_cast<int>(enabled)));
  
  int enable_fd = open(enable_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  write(enable_fd, msg.c_str(), msg.length());
  close(enable_fd);
}
