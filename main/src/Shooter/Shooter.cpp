#include <BizarroHomer/Shooter/Shooter.hpp>

using namespace std::literals::chrono_literals;

#define SHOOT_TIME 500ms

Shooter::Shooter() { }

Shooter::~Shooter() { }

void Shooter::process() {
  m_air_tank.process();
  m_pivot.process();
  m_barrel.process();
  
  AirTank::State desired_state = AirTank::State::IDLE;
  // TODO: Make sure barrel is at correct rotation for at least 0.5 seconds before shooting - weed out any bad data. Also, Also move from moving average to single pole iir.
  if (m_should_shoot && !m_barrel.is_rotating()) {
    m_shooting = true;
    m_should_shoot = false;
    m_shoot_start_time_point = std::chrono::system_clock::now();
  }
  if (m_shooting) {
    if (std::chrono::system_clock::now() - m_shoot_start_time_point >= SHOOT_TIME) {
      m_shooting = false;
      desired_state = AirTank::State::IDLE;
      m_barrel.rotate(m_barrel_dir);
    }
    else {
      desired_state = AirTank::State::SHOOTING;
    }
  }
  if (m_should_pressurize && !m_should_shoot && !m_shooting && !is_at_pressure()) {
    desired_state = AirTank::State::PRESSURIZING;
    m_should_pressurize = false;
  }
  
  m_air_tank.set_state(desired_state);
}

void Shooter::send_feedback() {
  m_air_tank.send_feedback();
  m_pivot.send_feedback();
  m_barrel.send_feedback();
}

void Shooter::shoot() {
  m_should_shoot = true;
}

void Shooter::pressurize() {
  m_should_pressurize = true;
}

bool Shooter::is_at_pressure() {
  return m_air_tank.is_at_pressure();
}

bool Shooter::has_pressure() {
  return m_air_tank.has_pressure();
}

void Shooter::rotate_barrel(ShooterBarrel::RotationDirection dir) {
  m_barrel.rotate(dir);
  m_barrel_dir = dir;
}

void Shooter::set_preset(ShooterPivot::Preset preset) {
  if (m_air_tank.is_shooting())
    return;
  
  m_pivot.set_preset(preset);
}

ShooterPivot::Preset Shooter::get_preset() {
  return m_pivot.get_preset();
}
