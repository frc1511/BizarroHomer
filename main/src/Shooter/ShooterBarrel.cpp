#include <BizarroHomer/Shooter/ShooterBarrel.hpp>
#include <cmath>
#include <algorithm>

using namespace std::literals::chrono_literals;

#define POS_TOLERANCE 5 // +/- 5

ShooterBarrel::ShooterBarrel() = default;
ShooterBarrel::~ShooterBarrel() = default;

void ShooterBarrel::process() {
  if (!m_init) {
    if (!m_init_waiting) {
      if (m_encoder.get_angle() < 15.0) {
        m_motor.set(TalonFXControlMode::PercentOutput, 0.1);
      }
      else {
        m_motor.set(TalonFXControlMode::PercentOutput, 0.0);
        m_init_waiting = true;
        m_init_time_point = std::chrono::system_clock::now();
      }
    }
    else {
      m_motor.set(TalonFXControlMode::PercentOutput, 0.0);
      auto dur = std::chrono::system_clock::now() - m_init_time_point;
      
      if (dur > 500ms) {
        // Convert the angle to a position in encoder ticks.
        double position = (m_encoder.get_angle() / 360.0) * 2048.0;
        m_motor.set(TalonFXControlMode::Position, position);
        
        m_init = true;
      }
    }
    
    return;
  }
  
  
  
  double current_position = m_motor.get_position();
  current_position = std::fmod(current_position, 2048.0);
  
  double diff = m_target_position - current_position;
  
  if (diff > 1024.0) {
    diff -= 2048.0;
  }
  else if (diff < -1024.0) {
    diff += 2048.0;
  }
  
  m_at_position = (std::abs(diff) <= POS_TOLERANCE);
  
  double target_position = current_position + diff;
  m_motor.set(TalonFXControlMode::Position, target_position);
}

void ShooterBarrel::rotate(RotationDirection dir) {
  m_target_increment = m_target_increment + static_cast<int>(dir);
  m_target_increment %= 6;
  
  m_target_position = m_target_increment * (2048.0 / 60.0);
}

bool ShooterBarrel::is_rotating() {
  return !m_at_position;
}

void ShooterBarrel::send_feedback(DashboardServer* dashboard) {
  dashboard->update_value("Barrel_AtPosition", m_at_position);
  dashboard->update_value("Barrel_Angle", m_encoder.get_angle());
  dashboard->update_value("Barrel_TargetAngle", static_cast<int>(m_target_position));
  dashboard->update_value("Barrel_Position", m_motor.get_position() / 2048.0);
}
