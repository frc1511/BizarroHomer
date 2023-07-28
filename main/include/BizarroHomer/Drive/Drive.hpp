#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/SparkMax.hpp>
#include <BizarroHomer/Hardware/TalonFX.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/ThroughBore.hpp>

class Drive : public Mechanism {
public:
  Drive();
  ~Drive();
  
  void process() override;
  void send_feedback() override;
  
  void tank_control(double left, double right);
  void arcade_control(double forwards, double turn);
  
private:
  thunder::SparkMax m_drive_left  { PWM_DRIVE_LEFT };
  thunder::SparkMax m_drive_right { PWM_DRIVE_RIGHT };
  
  double m_target_left = 0.0,
         m_target_right = 0.0;
};
