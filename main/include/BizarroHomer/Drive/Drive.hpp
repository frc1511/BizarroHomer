#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/MotorControl/PWMSparkMax.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>

class Drive : public Mechanism {
public:
  Drive();
  ~Drive();
  
  void process() override;
  
  void control(double left_pct, double right_pct);
  
private:
  PWMSparkMax drive_left  { PWM_DRIVE_LEFT };
  PWMSparkMax drive_right { PWM_DRIVE_RIGHT };
  
  double target_left = 0.0, target_right = 0.0;
};
