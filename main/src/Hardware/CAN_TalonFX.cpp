#include <BizarroHomer/Hardware/CAN_TalonFX.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#define HARDWARE_TYPE HardwareManager::HardwareType::CAN_TALON_FX

CAN_TalonFX::CAN_TalonFX(int _can_id)
: can_id(_can_id) {
  // Init the motor.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, can_id, HardwareManager::ControlProperty::INIT, 0.0);
  
  // Register status callback.
  HardwareManager::get()->register_status_callback(HARDWARE_TYPE, can_id, HardwareManager::StatusProperty::VALUE, [&](double v) {
    std::lock_guard<std::mutex> lk(pos_mut);
    pos = v;
  });
}

CAN_TalonFX::~CAN_TalonFX() { }

void CAN_TalonFX::set_percent(double percent) {
  // Send percent output.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, can_id, HardwareManager::ControlProperty::PERCENT, percent);
}

void CAN_TalonFX::set_position(double position) {
  // Send position.
  HardwareManager::get()->send_ctrl_msg(HARDWARE_TYPE, can_id, HardwareManager::ControlProperty::POSITION, position);
}

double CAN_TalonFX::get_position() {
  std::lock_guard<std::mutex> lk(pos_mut);
  return pos;
}
