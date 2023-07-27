#pragma once

#include <mutex>

class CAN_PDP {
public:
  CAN_PDP(int can_id = 0);
  ~CAN_PDP();
  
  double get_voltage();
  
private:
  int can_id;
  
  double voltage = 0;
  std::mutex voltage_mut;
};
