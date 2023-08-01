#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/TalonFX.hpp>
#include <BizarroHomer/Hardware/ThroughBore.hpp>
#include <chrono>

class ShooterBarrel : public Mechanism {
public:
  ShooterBarrel();
  ~ShooterBarrel();
  
  void process() override;
  void send_feedback(DashboardServer* dashboard) override;
  
  enum class RotationDirection {
    CLOCKWISE = +1,
    COUNTER_CLOCKWISE = -1,
  };
  
  //
  // Manually rotates the barrel one increment in the specified direction.
  //
  void rotate(RotationDirection direction);
  
  //
  // Returns whether the barrel is currently rotating.
  //
  bool is_rotating();
  
private:
  thunder::TalonFX m_motor { CAN_SHOOTER_ROTATION };
  thunder::ThroughBore m_encoder { GPIO_SHOOTER_ROTATION_ENCODER };
  
  // Increments of 60 degrees.
  int m_target_increment = 0;
  double m_target_position = 0.0;
  
  bool m_at_position = false;
  
  bool m_init = false;
  std::chrono::system_clock::time_point m_init_time_point;
  bool m_init_waiting = false;
};
