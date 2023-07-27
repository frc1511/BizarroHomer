#pragma once

#include <mutex>

class CAN_TalonFX {
public:
  CAN_TalonFX(int can_id);
  ~CAN_TalonFX();
  
  virtual void set_percent(double percent);
  virtual void set_position(double position);
  
  double get_position();
  
private:
  int can_id;
  
  double pos = 0;
  std::mutex pos_mut;
};
