#include <BizarroHomerHardwareControl/Hardware/Encoder/DutyCycleThroughBore.hpp>
#include <BizarroHomerHardwareControl/Util/LinearFilter.hpp>
#include <fmt/core.h>
#include <chrono>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#define TIMEOUT_US 102500
#define PULSE_MAX 1025.0

DutyCycleThroughBore::DutyCycleThroughBore(int _channel)
: channel(_channel),
  channel_dir(fmt::format("/sys/class/gpio/gpio{}", channel)),
  dir_path(channel_dir / "direction"),
  value_path(channel_dir / "value") {
  
  std::string channel_str(fmt::format("{}", channel));
  int export_fd = open("/sys/class/gpio/export", O_WRONLY | O_CREAT | O_TRUNC);
  write(export_fd, channel_str.c_str(), channel_str.length());
  close(export_fd);
  
  read_thread = std::thread([this]() {
    this->read_thread_main();
  });
}

DutyCycleThroughBore::~DutyCycleThroughBore() {
  {
    std::lock_guard<std::mutex> lk(read_mut);
    should_term = true;
  }
  read_thread.join();
}

double DutyCycleThroughBore::get_angle() {
  std::lock_guard<std::mutex> lk(read_mut);
  return angle;
}

bool DutyCycleThroughBore::read_channel() {
  int fd = open(value_path.c_str(), O_RDONLY);
  
  char value = 0;
  read(fd, &value, 1);
  
  close(fd);
  
  return value == '1' ? 1 : 0;
}

void DutyCycleThroughBore::read_thread_main() {
  LinearFilter<double> filter = LinearFilter<double>::moving_average(20);
  
  while (true) {
    {
      std::lock_guard<std::mutex> lk(read_mut);
      if (should_term) break;
    }
    
    while (read_channel() == 0) {
      // TODO: See if timeout was surpassed...
    }
    auto start = std::chrono::high_resolution_clock::now();
    
    while (read_channel() == 1) {
      // TODO: See if timeout was surpassed...
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // Get time high (pulse width).
    int duty_cycle = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    // Convert to 0-360 value.
    double read_angle = std::clamp(360.0 * (duty_cycle / PULSE_MAX), 0.0, 360.0);
    
    // Filter out bad values.
    double good_angle = filter.calculate(read_angle);
    
    std::lock_guard<std::mutex> lk(read_mut);
    angle = good_angle;
  }
}
