#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Shooter/ShooterBarrel.hpp>
#include <BizarroHomer/Shooter/ShooterPivot.hpp>
#include <BizarroHomer/Shooter/AirTank.hpp>
#include <chrono>

class Shooter : public Mechanism {
public:
  Shooter();
  ~Shooter();
  
  void process() override;
  void send_feedback(DashboardServer* dashboard) override;
  
  //
  // Tells the shooter to begin shooting.
  //
  void shoot();

  //
  // Tells the shooter to pressurize the fill tank this frame.
  //
  void pressurize();
  
  //
  // Returns whether the fill tank is at 120 PSI.
  //
  bool is_at_pressure();
  
  //
  // Returns whether the fill tank has any pressure.
  //
  bool has_pressure();
  
  //
  // Returns the current pressure of the fill tank, in PSI.
  //
  double get_pressure();
  
  //
  // Rotates the barrel of the shooter one increment in the given direction.
  //
  void rotate_barrel(ShooterBarrel::RotationDirection direction);
  
  //
  // Pivots the shooter to the given preset.
  //
  void set_pivot_preset(ShooterPivot::Preset preset);
  
  //
  // Manually controls the pivot of the shooter.
  //
  void manual_pivot_control(double speed);
  
private:
  AirTank m_air_tank;
  ShooterPivot m_pivot;
  ShooterBarrel m_barrel;
  
  ShooterBarrel::RotationDirection m_barrel_dir = ShooterBarrel::RotationDirection::CLOCKWISE;
  
  bool m_should_shoot = false;
  bool m_shooting = false;
  bool m_should_pressurize = false;
  std::chrono::system_clock::time_point m_shoot_start_time_point;
};
