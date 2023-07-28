#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <BizarroHomer/Hardware/DigitalInput.hpp>
#include <filesystem>
#include <string>
#include <thread>
#include <mutex>

namespace thunder {

class ThroughBore : public HardwareComponent {
public:
  ThroughBore(int channel);
  ~ThroughBore();
  
  double get_angle();
  
private:
  DigitalInput m_channel_input;
  
  double m_angle = 0;
  bool m_should_term = false;
  
  std::thread m_read_thread;
  std::mutex m_read_mutex;
  
  void read_thread();
};

} // namespace thunder
