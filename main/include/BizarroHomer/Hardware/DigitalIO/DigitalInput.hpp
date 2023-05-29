#pragma once

#include <mutex>

class DigitalInput {
public:
  DigitalInput(int channel);
  ~DigitalInput();
  
  bool get();
  
private:
  int channel;
  
  bool input = 0;
  std::mutex input_mut;
};
