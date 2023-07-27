#pragma once

#include <mutex>

class DutyCycle_ThroughBore {
public:
  DutyCycle_ThroughBore(int channel);
  ~DutyCycle_ThroughBore();
  
  double get_angle();
  
private:
  int channel;
  
  double angle = 0;
  std::mutex pos_mut;
};
