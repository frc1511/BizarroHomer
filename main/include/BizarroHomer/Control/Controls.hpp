#pragma once

#include <BizarroHomer/Hardware/PDP.hpp>
#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>

class Drive;
class Shooter;
class BlinkyBlinky;

class Controls : public Mechanism {
public:
  Controls(Drive* drive, Shooter* shooter, thunder::PDP* pdp, BlinkyBlinky* blinky_blinky);
  ~Controls();
  
  void process() override;
  
private:
  Drive* m_drive;
  Shooter* m_shooter;
  thunder::PDP* m_pdp;
  BlinkyBlinky* m_blinky_blinky;
  
  //
  // Functions to handle the various subsystems.
  //
  void handle_leds();
  void handle_drive();
  void handle_shooter();
  
  enum class DriveMode {
    TANK = 0,
    ARCADE = 1,
  };
  DriveMode m_drive_mode = DriveMode::ARCADE;

  bool m_was_pressurizing = false;
  
  DualShock4::InputFrame m_input,
                         m_last_input;
  
  uint8_t m_colors = 0,
          m_last_colors = 0;
};
