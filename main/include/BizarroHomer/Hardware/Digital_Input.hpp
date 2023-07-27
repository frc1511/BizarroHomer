#pragma once

#include <mutex>

class Digital_Input {
public:
  Digital_Input(int channel);
  ~Digital_Input();
  
  bool get();
  
private:
  int channel;
  
  bool input = 0;
  std::mutex input_mut;
};
