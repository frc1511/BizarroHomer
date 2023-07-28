#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#define Phoenix_No_WPI
#include <ctre/Phoenix.h>

namespace thunder {

class TalonFX : public HardwareComponent {
public:
  TalonFX(int can_id);
  ~TalonFX();
  
  void stop() override;
  void set_enabled(bool enabled) override;
  
  void set(TalonFXControlMode mode, double value);
  
  //
  // Encoder position. 2048 ticks per rotation.
  //
  double get_position();
  double set_position(double rotations);
  double get_velocity();
  
  //
  // Current draw, in Amps.
  //
  double get_current();
  
  bool get_inverted();
  void set_inverted(bool inverted);
  
  //
  // Closed loop control values.
  //
  void config_p(double gain);
  void config_i(double gain);
  void config_d(double gain);
  void config_ff(double gain);
  void config_izone(double gain);
  
  //
  // Returns the base TalonFX object.
  //
  ctre::phoenix::motorcontrol::can::TalonFX* get_talon();
  
private:
  ctre::phoenix::motorcontrol::can::TalonFX m_motor;
  
  bool m_enabled = true;
};

} // namespace thunder
