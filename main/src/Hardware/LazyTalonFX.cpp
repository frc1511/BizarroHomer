#include <BizarroHomer/Hardware/LazyTalonFX.hpp>

LazyTalonFX::LazyTalonFX(int can_id)
: TalonFX(can_id) { }

LazyTalonFX::~LazyTalonFX() { }

void LazyTalonFX::set_percent(double percent) {
  if (last_control != ControlMode::PERCENT || percent != last_percent) {
    // Remember this control.
    last_control = ControlMode::PERCENT;
    last_percent = percent;
    
    // Actually set the percent.
    TalonFX::set_percent(percent);
  }
  // Already been set to desired percent, so no need to send control message again.
}

void LazyTalonFX::set_position(double position) {
  if (last_control != ControlMode::POSITION || position != last_position) {
    // Remember this control.
    last_control = ControlMode::POSITION;
    last_position = position;
    
    // Actually set the position.
    TalonFX::set_position(position);
  }
  // Already been set to desired position, so no need to send control message again.
}
