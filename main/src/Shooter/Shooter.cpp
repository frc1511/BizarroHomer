#include <BizarroHomer/Shooter/Shooter.hpp>

using namespace std::literals::chrono_literals;

#define SHOOT_TIME 500ms

Shooter::Shooter() {

}

Shooter::~Shooter() {

}

void Shooter::process() {
  air_tank.process();
  pivot.process();
  barrel.process();
  
  AirTank::State desired_state = AirTank::State::IDLE;
  if (should_shoot) {
    if (std::chrono::system_clock::now() - shoot_start_time_point >= SHOOT_TIME) {
      should_shoot = false;
      desired_state = AirTank::State::IDLE;
    }
    else {
      desired_state = AirTank::State::SHOOTING;
    }
  }
  if (should_pressurize && !should_shoot && !is_at_pressure()) {
    desired_state = AirTank::State::PRESSURIZING;
    should_pressurize = false;
  }
  
  air_tank.set_state(desired_state);
}

void Shooter::send_feedback() {
  air_tank.send_feedback();
  pivot.send_feedback();
  barrel.send_feedback();
}

void Shooter::shoot() {
  should_shoot = true;
  shoot_start_time_point = std::chrono::system_clock::now();
}

void Shooter::pressurize() {
  should_pressurize = true;
}

bool Shooter::is_at_pressure() {
  return air_tank.is_at_pressure();
}

bool Shooter::has_pressure() {
  return air_tank.has_pressure();
}

void Shooter::rotate_barrel(ShooterBarrel::RotationDirection dir) {
  barrel.rotate(dir);
}

void Shooter::set_preset(ShooterPivot::Preset preset) {
  if (air_tank.is_shooting())
    return;
  
  pivot.set_preset(preset);
}

ShooterPivot::Preset Shooter::get_preset() {
  return pivot.get_preset();
}
