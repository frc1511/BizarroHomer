#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/MotorControl/PWMSparkMax.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/Encoder/DutyCycleThroughBore.hpp>

class Drive : public Mechanism {
public:
  Drive();
  ~Drive();
  
  void process() override;
  void send_feedback() override;
  
  void tank_control(double left, double right);
  void arcade_control(double forwards, double turn);
  
private:
  /* PWMSparkMax drive_left  { PWM_DRIVE_LEFT }; */
  /* PWMSparkMax drive_right { PWM_DRIVE_RIGHT }; */

  /* TalonFX talon { 21 }; */
  DutyCycleThroughBore encoder { 3 };
  
  double target_left = 0.0, target_right = 0.0;
};
