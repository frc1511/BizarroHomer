#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/TalonFX.hpp>
#include <BizarroHomer/Hardware/ThroughBore.hpp>

class ShooterBarrel : public Mechanism {
public:
  ShooterBarrel();
  ~ShooterBarrel();
  
  void process() override;
  void send_feedback() override;
  
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
  /* TalonFX m_rot_motor { CAN_SHOOTER_ROTATION }; */
  thunder::ThroughBore m_rot_enc { GPIO_SHOOTER_ROTATION_ENCODER };
  
  // Increments of 60 degrees.
  enum class Position {
    _1 = 0,
    _2 = 60,
    _3 = 120,
    _4 = 180,
    _5 = 240,
    _6 = 300,
  };
  
  Position m_target_position = Position::_1;
  bool m_at_position = false;
};
