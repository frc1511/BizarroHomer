#include <BizarroHomer/Shooter/ShooterBarrel.hpp>
#include <BizarroHomer/Basic/FeedbackManager.hpp>
#include <cmath>
#include <algorithm>

#define ANGLE_TOLERANCE 5 // +/- 5

#define PROPORTIONAL 0.002

#define MAX_OUTPUT .2

ShooterBarrel::ShooterBarrel() { }

ShooterBarrel::~ShooterBarrel() { }

void ShooterBarrel::process() {
  int target_angle = static_cast<int>(m_target_position);
  int current_angle = static_cast<int>(m_rot_enc.get_angle());
  
  int diff = target_angle - current_angle;
  
  if (diff > 180) {
    diff -= 360;
  }
  else if (diff < -180) {
    diff += 360;
  }
  
  if (std::abs(diff) <= ANGLE_TOLERANCE) {
    // At position, stop rotating.
    m_at_position = true;
    /* m_rot_motor.set_percent(0.0); */
    return;
  }
  
  m_at_position = false;
  
  double pct_out = PROPORTIONAL * diff;
  pct_out = std::clamp(pct_out, -MAX_OUTPUT, +MAX_OUTPUT);
  
  // Rotate!
  /* m_rot_motor.set_percent(pct_out); */
}

void ShooterBarrel::rotate(RotationDirection dir) {
  int pos = static_cast<int>(m_target_position) + 60 * static_cast<int>(dir);
  
  m_target_position = static_cast<Position>(pos);
}

bool ShooterBarrel::is_rotating() {
  return !m_at_position;
}

void ShooterBarrel::send_feedback() {
  FeedbackManager::get()->send_value("Barrel_AtPosition", m_at_position ? "True" : "False");
  FeedbackManager::get()->send_value("Barrel_Angle", std::to_string(m_rot_enc.get_angle()));
  FeedbackManager::get()->send_value("Barrel_TargetAngle", std::to_string(static_cast<int>(m_target_position)));
  /* FeedbackManager::get()->send_value("Barrel_Position", std::to_string(m_rot_motor.get_position())); */
}
