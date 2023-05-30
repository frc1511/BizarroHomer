#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>

class Drive;

class Controls : public Mechanism {
public:
  Controls(Drive* drive);
  ~Controls();
  
  void process() override;
  
private:
  Drive* drive;
  
  bool was_conn = false;
  bool was_drive_toggling = false;
  
  enum class DriveMode {
    TANK = 0,
    ARCADE = 1,
  };
  DriveMode drive_mode = DriveMode::ARCADE;
};
