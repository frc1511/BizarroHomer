#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Shooter/ShooterBarrel.hpp>
#include <BizarroHomer/Shooter/ShooterPivot.hpp>
#include <BizarroHomer/Shooter/AirTank.hpp>

class Shooter : public Mechanism {
public:
  Shooter();
  ~Shooter();
  
  void process() override;
  
  void set_pressurizing(bool pressurizing);
  bool has_pressure();
  bool is_at_pressure();
  
  void rotate_barrel(ShooterBarrel::RotationDirection direction);
  
  void shoot();
  
  void set_preset(ShooterPivot::Preset preset);
  
  ShooterPivot::Preset get_preset();
  
private:
  AirTank air_tank;
  ShooterPivot pivot;
  ShooterBarrel barrel;
};
