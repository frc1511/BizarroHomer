#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/CAN_TalonFX.hpp>
#include <BizarroHomer/Hardware/DutyCycle_ThroughBore.hpp>

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
  
  /**
   * @brief Starts rotating the barrel one increment in the specified direction.
   *
   * @param direction The direction to rotate the barrel.
   */
  void rotate(RotationDirection direction);
  
  /**
   * @brief Returns whether the barrel should be rotating (when it's not at its
   *        target position).
   *
   * @return Whether the barrel should be rotating.
   */
  bool is_rotating();
  
private:
  /* TalonFX rot_motor { CAN_SHOOTER_ROTATION }; */
  DutyCycle_ThroughBore rot_enc { GPIO_SHOOTER_ROTATION_ENCODER };
  
  // Increments of 60 degrees.
  enum class Position {
    _1 = 0,
    _2 = 60,
    _3 = 120,
    _4 = 180,
    _5 = 240,
    _6 = 300,
  };
  
  Position target_position = Position::_1;
  bool at_position = false;
};
