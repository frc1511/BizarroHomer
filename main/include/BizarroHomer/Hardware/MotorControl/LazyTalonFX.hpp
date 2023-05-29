#pragma once

#include <BizarroHomer/Hardware/MotorControl/TalonFX.hpp>

class LazyTalonFX : public TalonFX {
public:
  LazyTalonFX(int can_id);
  ~LazyTalonFX();
  
  void set_percent(double percent) override;
  void set_position(double position) override;
  
private:
  enum class ControlMode { NONE, PERCENT, POSITION };
  
  ControlMode last_control = ControlMode::NONE;
  double last_percent = 0.0;
  double last_position = 0.0;
};
