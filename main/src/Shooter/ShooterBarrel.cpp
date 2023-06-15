#include <BizarroHomer/Shooter/ShooterBarrel.hpp>
#include <cmath>
#include <algorithm>

#define ANGLE_TOLERANCE 5 // +/- 5

#define PROPORTIONAL 0.002

#define MAX_OUTPUT .2

ShooterBarrel::ShooterBarrel() { }

ShooterBarrel::~ShooterBarrel() { }

void ShooterBarrel::process() {
  int target_angle = static_cast<int>(target_position);
  int current_angle = static_cast<int>(rot_enc.get_angle());
  
  int diff = target_angle - current_angle;
  
  if (diff > 180) {
    diff -= 360;
  }
  else if (diff < -180) {
    diff += 360;
  }
  
  if (std::abs(diff) <= ANGLE_TOLERANCE) {
    // At position, stop rotating.
    at_position = true;
    rot_motor.set_percent(0.0);
    return;
  }
  
  at_position = false;
  
  double pct_out = PROPORTIONAL * diff;
  pct_out = std::clamp(pct_out, -MAX_OUTPUT, +MAX_OUTPUT);
  
  // Rotate!
  rot_motor.set_percent(pct_out);
}

void ShooterBarrel::rotate_barrel(RotationDirection dir) {
  int pos = static_cast<int>(target_position) + 60 * static_cast<int>(dir);
  
  target_position = static_cast<Position>(pos);
}

bool ShooterBarrel::is_rotating() {
  return !at_position;
}
