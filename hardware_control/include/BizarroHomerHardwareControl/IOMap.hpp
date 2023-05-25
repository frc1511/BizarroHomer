#pragma once

class IOMap {
public:
  enum {
    PWM_DRIVE_LEFT = 0,
    PWM_DRIVE_RIGHT = 1,
    
    CAN_PIVOT_LEFT = 2,
    CAN_PIVOT_RIGHT = 3,
    CAN_SHOOTER_ROT = 4,
    
    DI_SHOOTER_ROT = 2,
    DO_FILL_VALVE = 3,
    DO_SHOOT_VALVE = 4,
  };
};
