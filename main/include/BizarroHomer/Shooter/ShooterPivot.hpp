#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/MotorControl/TalonFX.hpp>
#include <map>

class ShooterPivot : public Mechanism {
public:
  ShooterPivot();
  ~ShooterPivot();
  
  void process() override;
  void send_feedback() override;
  
  enum class Preset {
    LOW,
    MID,
    HIGH,
  };
  
  void set_preset(Preset preset);
  
  Preset get_preset();
  
private:
  Preset preset = Preset::LOW;
  
  TalonFX left_motor  { CAN_SHOOTER_PIVOT_LEFT };
  TalonFX right_motor { CAN_SHOOTER_PIVOT_RIGHT };
  
  const std::map<Preset, double> preset_positions {
    { Preset::LOW,  0.0 },
    { Preset::MID,  1.2 },
    { Preset::HIGH, 2.4 }, // MAX 2.8
  };
};
