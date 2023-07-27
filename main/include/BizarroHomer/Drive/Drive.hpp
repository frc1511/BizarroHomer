#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/PWM_SparkMax.hpp>
#include <BizarroHomer/Hardware/CAN_TalonFX.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/DutyCycle_ThroughBore.hpp>

class Drive : public Mechanism {
public:
  Drive();
  ~Drive();
  
  void process() override;
  void send_feedback() override;
  
  void tank_control(double left, double right);
  void arcade_control(double forwards, double turn);
  
private:
  PWM_SparkMax drive_left  { PWM_DRIVE_LEFT };
  PWM_SparkMax drive_right { PWM_DRIVE_RIGHT };
  
  double target_left = 0.0, target_right = 0.0;
};
