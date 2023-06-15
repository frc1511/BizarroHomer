#include <BizarroHomer/Shooter/ShooterPivot.hpp>

#define POSITION_TOLERANCE 0.5

#define PROPORTIONAL 0.015

#define MAX_OUTPUT .3

ShooterPivot::ShooterPivot() {
  // TODO: Set one to inverted...
}

ShooterPivot::~ShooterPivot() {

}

void ShooterPivot::process() {
  double current_pos = left_motor.get_position();
  double target_pos = preset_positions.at(preset);
  
  double diff = target_pos - current_pos;
  
  if (diff < POSITION_TOLERANCE) {
    left_motor.set_percent(0.0);
    right_motor.set_percent(0.0);
    return;
  }
  
  double pct_out = PROPORTIONAL * diff;
  pct_out = std::clamp(pct_out, -MAX_OUTPUT, +MAX_OUTPUT);
  
  // Rotate!
  left_motor.set_percent(pct_out);
  right_motor.set_percent(pct_out);
}

void ShooterPivot::set_preset(Preset _preset) {
  preset = _preset;
}

ShooterPivot::Preset ShooterPivot::get_preset() {
  return preset;
}
