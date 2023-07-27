#pragma once

#include <BizarroHomer/Hardware/CAN_PDP.hpp>
#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Control/GameController/GameControllerManager.hpp>

class Drive;
class Shooter;

class Controls : public Mechanism {
public:
  Controls(Drive* drive, Shooter* shooter, CAN_PDP* pdp);
  ~Controls();
  
  void process() override;
  
private:
  Drive* drive;
  Shooter* shooter;
  CAN_PDP* pdp;
  
  bool was_conn = false;
  
  enum class DriveMode {
    TANK = 0,
    ARCADE = 1,
  };
  DriveMode drive_mode = DriveMode::ARCADE;

  bool was_pressurizing = false;
  
  GameControllerManager::InputFrame input, last_input;
  
  uint8_t colors = 0, last_colors = 0;
};
