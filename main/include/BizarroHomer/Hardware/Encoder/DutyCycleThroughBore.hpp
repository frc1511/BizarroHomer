#pragma once

#include <mutex>

class DutyCycleThroughBore {
public:
  DutyCycleThroughBore(int channel);
  ~DutyCycleThroughBore();
  
  double get_angle();
  
private:
  int channel;
  
  double angle = 0;
  std::mutex pos_mut;
};
