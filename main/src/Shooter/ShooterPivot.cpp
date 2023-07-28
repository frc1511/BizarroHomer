#include <BizarroHomer/Shooter/ShooterPivot.hpp>
#include <BizarroHomer/Basic/FeedbackManager.hpp>

#define POSITION_TOLERANCE 0.5

#define PROPORTIONAL 0.015

#define MAX_OUTPUT .3

ShooterPivot::ShooterPivot() {
  // TODO: Set one to inverted...
}

ShooterPivot::~ShooterPivot() {

}

void ShooterPivot::process() {
  /* double current_pos = m_left_motor.get_position(); */
  /* double target_pos = m_preset_positions.at(preset); */
  
  /* double diff = target_pos - current_pos; */
  
  /* if (diff < POSITION_TOLERANCE) { */
    /* m_left_motor.set_percent(0.0); */
    /* m_right_motor.set_percent(0.0); */
    /* return; */
  /* } */
  
  /* double pct_out = PROPORTIONAL * diff; */
  /* pct_out = std::clamp(pct_out, -MAX_OUTPUT, +MAX_OUTPUT); */
  
  // Rotate!
  /* m_left_motor.set_percent(pct_out); */
  /* m_right_motor.set_percent(pct_out); */
}

void ShooterPivot::set_preset(Preset preset) {
  m_preset = preset;
}

ShooterPivot::Preset ShooterPivot::get_preset() {
  return m_preset;
}

void ShooterPivot::send_feedback() {
  FeedbackManager::get()->send_value("Pivot_Position_Left", std::to_string(-m_left_motor.get_position()));
  FeedbackManager::get()->send_value("Pivot_Position_Right", std::to_string(m_right_motor.get_position()));
}
