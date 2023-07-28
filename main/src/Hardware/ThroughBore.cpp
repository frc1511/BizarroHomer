#include <BizarroHomer/Hardware/ThroughBore.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomer/Util/LinearFilter.hpp>
#include <fmt/core.h>
#include <chrono>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std::literals::chrono_literals;

#define TIMEOUT_US 102500us
#define PULSE_MAX 1025.0

thunder::ThroughBore::ThroughBore(int channel)
: m_channel_input(channel) {
  // Start the read thread.
  m_read_thread = std::thread([this]() {
    this->read_thread();
  });
}

thunder::ThroughBore::~ThroughBore() {
  {
    std::lock_guard<std::mutex> lk(m_read_mutex);
    m_should_term = true;
  }
  
  m_read_thread.join();
}

double thunder::ThroughBore::get_angle() {
  std::lock_guard<std::mutex> lk(m_read_mutex);
  return m_angle;
}

void thunder::ThroughBore::read_thread() {
  LinearFilter<double> filter = LinearFilter<double>::moving_average(20);
  
  while (true) {
    {
      std::lock_guard<std::mutex> lk(m_read_mutex);
      if (m_should_term) {
        break;
      }
    }
    
    auto timeout = std::chrono::high_resolution_clock::now();
    bool timed_out = false;
    
    // Rising edge.
    while (m_channel_input.get() == 0) {
      if (std::chrono::high_resolution_clock::now() - timeout > TIMEOUT_US) {
        timed_out = true;
        break;
      }
    }
    if (timed_out) {
      continue;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Falling edge.
    while (m_channel_input.get() == 1) {
      if (std::chrono::high_resolution_clock::now() - start > TIMEOUT_US) {
        timed_out = true;
        break;
      }
    }
    if (timed_out) {
      continue;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    // Get time high (pulse width).
    int duty_cycle = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    // Convert to 0-360 value.
    double read_angle = std::clamp(360.0 * (duty_cycle / PULSE_MAX), 0.0, 360.0);
    
    // Filter out bad values.
    double good_angle = filter.calculate(read_angle);
    
    std::lock_guard<std::mutex> lk(m_read_mutex);
    m_angle = good_angle;
  }
}
