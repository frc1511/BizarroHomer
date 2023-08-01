#include <BizarroHomer/Shooter/Shooter.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

using namespace std::literals::chrono_literals;

#define SHOOT_TIME 500ms

Shooter::Shooter() = default;
Shooter::~Shooter() = default;

void Shooter::process() {
  m_air_tank.process();
  m_pivot.process();
  m_barrel.process();
  
  AirTank::State desired_state = AirTank::State::IDLE;
  
  //
  // Shooting
  //
  if (m_should_shoot && !m_barrel.is_rotating()) {
    m_shooting = true;
    m_shoot_start_time_point = std::chrono::system_clock::now();
  }
  if (m_shooting) {
    // Shooting has finished.
    if (std::chrono::system_clock::now() - m_shoot_start_time_point >= SHOOT_TIME) {
      m_shooting = false;
      // Close the shoot valve.
      desired_state = AirTank::State::IDLE;
      // Rotate the barrel to the next position.
      m_barrel.rotate(m_barrel_dir);
    }
    // Shooting happening.
    else {
      desired_state = AirTank::State::SHOOTING;
    }
  }
  
  //
  // Pressurizing
  //
  if (m_should_pressurize && !m_should_shoot && !m_shooting && !is_at_pressure()) {
    desired_state = AirTank::State::PRESSURIZING;
  }
  
  m_should_shoot = false;
  m_should_pressurize = false;
  
  // Set the state of the air tank.
  m_air_tank.set_state(desired_state);
}

void Shooter::send_feedback(DashboardServer* dashboard) {
  m_air_tank.send_feedback(dashboard);
  m_pivot.send_feedback(dashboard);
  m_barrel.send_feedback(dashboard);
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
  return m_air_tank.get_pressure() > 2.0;
}

double Shooter::get_pressure() {
  return m_air_tank.get_pressure();
}

void Shooter::rotate_barrel(ShooterBarrel::RotationDirection dir) {
  // Rotate the barrel one increment in the given direction.
  m_barrel.rotate(dir);
  
  // That'll be the direction the barrel rotates next time it shoot.
  m_barrel_dir = dir;
}

void Shooter::set_pivot_preset(ShooterPivot::Preset preset) {
  if (m_air_tank.is_shooting() ||
      thunder::HardwareManager::get()->is_song_playing()) {
    return;
  }
  
  m_pivot.set_preset(preset);
}

void Shooter::manual_pivot_control(double speed) {
  if (m_air_tank.is_shooting() ||
      thunder::HardwareManager::get()->is_song_playing()) {
    return;
  }
  
  m_pivot.manual_control(speed);
}
