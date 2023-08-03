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
  thunder::TalonFX m_motor { CAN_BARREL_ROTATION };
  thunder::ThroughBore m_encoder { GPIO_BARREL_ENCODER };
  
  double angle_to_position(double angle) const;
  double position_to_angle(double position) const;
  
  void handle_init();
  
  // Increments of 60 degrees.
  int m_target_increment = 0;
  
  double m_output_percent = 0.0;
  
  bool m_at_position = false;
  
  bool m_init = false;
  std::chrono::system_clock::time_point m_init_time_point;
  bool m_init_waiting = true;
};
