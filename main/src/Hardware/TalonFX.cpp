#include <BizarroHomer/Hardware/TalonFX.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

thunder::TalonFX::TalonFX(int can_id)
: m_motor(can_id) {
  
  TalonFXConfiguration configs;
  m_motor.GetAllConfigs(configs);
  
  // Config the motor to use the integrated encoder for PID loop.
  configs.primaryPID.selectedFeedbackSensor = FeedbackDevice::IntegratedSensor;
  
  m_motor.ConfigAllSettings(configs);
  
  
  set_position(0.0);
  
  HardwareManager::get()->register_hardware(this);
}

thunder::TalonFX::~TalonFX() = default;

void thunder::TalonFX::stop() {
  set(TalonFXControlMode::PercentOutput, 0.0);
}

void thunder::TalonFX::set_enabled(bool enabled) {
  m_enabled = enabled;
  stop();
}

void thunder::TalonFX::set(TalonFXControlMode mode, double value) {
  if (!m_enabled) {
    stop();
    return;
  }
  
  m_motor.Set(mode, value);
}

double thunder::TalonFX::get_position() {
  return m_motor.GetSelectedSensorPosition(0);
}

double thunder::TalonFX::set_position(double rotations) {
  return m_motor.SetSelectedSensorPosition(rotations, 0);
}

double thunder::TalonFX::get_velocity() {
  return m_motor.GetSelectedSensorVelocity(0);
}

double thunder::TalonFX::get_current() {
  return m_motor.GetOutputCurrent();
}

bool thunder::TalonFX::get_inverted() {
  return m_motor.GetInverted();
}

void thunder::TalonFX::set_inverted(bool inverted) {
  return m_motor.SetInverted(inverted);
}

void thunder::TalonFX::config_p(double gain) {
  m_motor.Config_kP(0, gain);
}

void thunder::TalonFX::config_i(double gain) {
  m_motor.Config_kI(0, gain);
}

void thunder::TalonFX::config_d(double gain) {
  m_motor.Config_kD(0, gain);
}

void thunder::TalonFX::config_ff(double gain) {
  m_motor.Config_kF(0, gain);
}

void thunder::TalonFX::config_izone(double gain) {
  m_motor.Config_IntegralZone(0, gain);
}

ctre::phoenix::motorcontrol::can::TalonFX* thunder::TalonFX::get_talon() {
  return &m_motor;
}
