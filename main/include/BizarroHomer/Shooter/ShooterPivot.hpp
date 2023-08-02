#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/TalonFX.hpp>
#include <map>

class ShooterPivot : public Mechanism {
public:
  ShooterPivot();
  ~ShooterPivot();
  
  void process() override;
  void send_feedback(DashboardServer* dashboard) override;
  
  enum class Preset {
    LOW,
    MID,
    HIGH,
  };
  
  //
  // Pivot the shooter to a preset position.
  //
  void set_preset(Preset preset);
  
  //
  // Pivot the shooter manally using a speed (-1.0 to 1.0).
  //
  // A max speed of 1.0 in either direction will equate to 1 rotation/second of
  // the pivoting motors.
  //
  void manual_control(double speed);
  
  //
  // Returns the current position of the shooter, in rotations.
  //
  double get_position();
  
private:
  double m_target_position = 0.0;
  
  thunder::TalonFX m_left_motor  { CAN_SHOOTER_PIVOT_LEFT };
  thunder::TalonFX m_right_motor { CAN_SHOOTER_PIVOT_RIGHT };
  
  double m_output_percent = 0.0;
  
  const std::map<Preset, double> m_preset_positions {
    { Preset::LOW,  0.0 },
    { Preset::MID,  1.2 },
    { Preset::HIGH, 2.4 }, // MAX 2.8
  };
};
