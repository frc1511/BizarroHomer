#include <BizarroHomer/Shooter/ShooterPivot.hpp>
#include <cassert>

//
// The number of encoder ticks in one rotation of the motor.
//
#define ROTATION_TICKS 2048.0

//
// Simple P control loop.
//
#define PROP_GAIN 0.15 // 0.2

//
// Some feed-forward gain since the shooter is pretty heavy.
//
#define FEED_FORWARD_GAIN 0.0

//
// Maximum percent output of the barrel motor.
//
#define MAX_OUTPUT 0.4

ShooterPivot::ShooterPivot() = default;
ShooterPivot::~ShooterPivot() = default;

void ShooterPivot::process() {
  // The current position in rotations.
  double current_position = m_left_motor.get_position() / ROTATION_TICKS;
  
  double error = m_target_position - current_position;
  
  m_output_percent = (PROP_GAIN * error) + FEED_FORWARD_GAIN;
  m_output_percent = std::clamp(m_output_percent, -MAX_OUTPUT, MAX_OUTPUT);
  
  // Control the motors.
  m_left_motor.set(TalonFXControlMode::PercentOutput, m_output_percent);
  m_right_motor.set(TalonFXControlMode::PercentOutput, -m_output_percent);
}

void ShooterPivot::set_preset(Preset preset) {
  m_target_position = m_preset_positions.at(preset);
}

void ShooterPivot::manual_control(double speed) {
  // About 1 rotation per second since the main loop runs at 50Hz.
  m_target_position += 0.02 * speed;
  
  double low_preset = m_preset_positions.at(Preset::LOW);
  double high_preset = m_preset_positions.at(Preset::HIGH);
  
  // Clamp the target position to the range of the pivot.
  m_target_position = std::clamp(m_target_position, low_preset, high_preset);
}

double ShooterPivot::get_position() {
  return m_left_motor.get_position() / ROTATION_TICKS;
}

void ShooterPivot::send_feedback(DashboardServer* dashboard) {
  dashboard->update_value("Pivot_Position_Left",  m_left_motor.get_position() / 2048.0);
  dashboard->update_value("Pivot_Position_Right", -m_right_motor.get_position() / 2048.0);
  dashboard->update_value("Pivot_TargetPosition", m_target_position);
  dashboard->update_value("Pivot_PercentOutput", m_output_percent);
}
