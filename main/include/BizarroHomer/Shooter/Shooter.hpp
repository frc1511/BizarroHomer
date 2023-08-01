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
  
  void shoot();
  void pressurize();
  
  bool is_at_pressure();
  bool has_pressure();
  double get_pressure();
  
  void rotate_barrel(ShooterBarrel::RotationDirection direction);
  
  void set_pivot_preset(ShooterPivot::Preset preset);
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
