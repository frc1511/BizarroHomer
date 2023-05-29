#pragma once

#include <mutex>

class TalonFX {
public:
  TalonFX(int can_id);
  ~TalonFX();
  
  virtual void set_percent(double percent);
  virtual void set_position(double position);
  
  double get_position();
  
private:
  int can_id;
  
  double pos = 0;
  std::mutex pos_mut;
};
