#pragma once

#include <BizarroHomer/Hardware/PDP.hpp>
#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>

class Drive;
class Shooter;

class Controls : public Mechanism {
public:
  Controls(Drive* drive, Shooter* shooter, thunder::PDP* pdp);
  ~Controls();
  
  void process() override;
  
private:
  Drive* drive;
  Shooter* shooter;
  thunder::PDP* pdp;
  
  void handle_leds();
  void handle_drive();
  void handle_shooter();
  
  enum class DriveMode {
    TANK = 0,
    ARCADE = 1,
  };
  DriveMode drive_mode = DriveMode::ARCADE;

  bool was_pressurizing = false;
  
  DualShock4::InputFrame input, last_input;
  
  uint8_t colors = 0, last_colors = 0;
};
