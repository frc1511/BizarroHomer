#pragma once

#include <mutex>

class DutyCycleEncoder {
public:
  DutyCycleEncoder(int channel);
  ~DutyCycleEncoder();
  
  double get();
  
private:
  int channel;
  
  double pos = 0;
  std::mutex pos_mut;
};
