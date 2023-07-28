#include <BizarroHomer/Hardware/PWM.hpp>
#include <fmt/core.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

thunder::PWM::PWM(int channel)
: m_channel(channel),
  m_channel_path(fmt::format("/sys/class/pwm/pwmchip0/pwm{}", channel)),
  m_enable_path(m_channel_path / "enable"),
  m_period_path(m_channel_path / "period"),
  m_duty_cycle_path(m_channel_path / "duty_cycle") {
  
  int export_fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
  write(export_fd, m_channel_str.c_str(), m_channel_str.length());
  close(export_fd);
}

thunder::PWM::~PWM() {
  int unexport_fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
  write(unexport_fd, m_channel_str.c_str(), m_channel_str.length());
  close(unexport_fd);
}

void thunder::PWM::set_duty_cycle(int duty_cycle) {
  std::string msg = std::to_string(duty_cycle);
  
  int duty_cycle_fd = open(m_duty_cycle_path.c_str(), O_WRONLY);
  write(duty_cycle_fd, msg.c_str(), msg.length());
  close(duty_cycle_fd);
}

void thunder::PWM::set_period(int period) {
  std::string msg = std::to_string(period);
  
  int period_fd = open(m_period_path.c_str(), O_WRONLY);
  write(period_fd, msg.c_str(), msg.length());
  close(period_fd);
}

void thunder::PWM::set_enabled(bool enabled) {
  std::string msg = std::to_string(static_cast<int>(enabled));
  
  int enable_fd = open(m_enable_path.c_str(), O_WRONLY);
  write(enable_fd, msg.c_str(), msg.length());
  close(enable_fd);
}
