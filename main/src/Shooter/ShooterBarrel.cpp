#include <BizarroHomer/Shooter/ShooterBarrel.hpp>
#include <cmath>
#include <algorithm>

using namespace std::literals::chrono_literals;

//
// Simple P control loop.
//
#define PROP_GAIN 0.005

#define ANGLE_TOLERANCE 5.0

//
// Maximum percent output of the barrel motor.
//
#define MAX_OUTPUT 0.4

ShooterBarrel::ShooterBarrel() {
  m_init_time_point = std::chrono::system_clock::now();
}

ShooterBarrel::~ShooterBarrel() = default;

void ShooterBarrel::process() {
  if (!m_init) {
    handle_init();
    return;
  }
  
  double current_angle = position_to_angle(m_motor.get_position());
  double target_angle = std::fmod(m_target_increment * 60.0, 360.0);
  
  double error = target_angle - current_angle;
  
  m_at_position = (std::abs(error) <= ANGLE_TOLERANCE);
  
  m_output_percent = PROP_GAIN * error;
  m_output_percent = std::clamp(m_output_percent, -MAX_OUTPUT, MAX_OUTPUT);
  
  m_motor.set(TalonFXControlMode::PercentOutput, m_output_percent);
}

void ShooterBarrel::handle_init() {
  if (m_init_waiting) {
    m_motor.set(TalonFXControlMode::PercentOutput, 0.0);
    
    auto dur = std::chrono::system_clock::now() - m_init_time_point;
    if (dur > 500ms) {
      double angle = m_encoder.get_angle();
      m_motor.set_position(angle_to_position(angle));
      
      m_init = true;
      
      // Determine the closest increment to the current position.
      m_target_increment = std::round(angle / 60.0);
    }
  }
  else {
    if (m_encoder.get_angle() < 15.0) {
      m_motor.set(TalonFXControlMode::PercentOutput, 0.1);
    }
    else {
      m_motor.set(TalonFXControlMode::PercentOutput, 0.0);
      m_init_waiting = true;
      m_init_time_point = std::chrono::system_clock::now();
    }
  }
}

void ShooterBarrel::rotate(RotationDirection dir) {
  m_target_increment += static_cast<int>(dir);
}

bool ShooterBarrel::is_rotating() {
  return !m_at_position;
}

double ShooterBarrel::angle_to_position(double angle) const {
  double position = (angle / 360.0) * 2048.0;
  return position;
}

double ShooterBarrel::position_to_angle(double position) const {
  position = std::fmod(position, 2048.0);
  double angle = (position / 2048.0) * 360.0;
  return angle;
}

void ShooterBarrel::send_feedback(DashboardServer* dashboard) {
  dashboard->update_value("Barrel_AtPosition",      m_at_position);
  dashboard->update_value("Barrel_EncoderAngle",    m_encoder.get_angle());
  dashboard->update_value("Barrel_TargetIncrement", m_target_increment);
  dashboard->update_value("Barrel_TargetAngle",     static_cast<int>(m_target_increment * 60.0));
  dashboard->update_value("Barrel_MotorAngle",      static_cast<int>(position_to_angle(m_motor.get_position())));
  dashboard->update_value("Barrel_MotorPosition",   m_motor.get_position() / 2048.0);
  dashboard->update_value("Barrel_PercentOutput",   m_output_percent);
}
