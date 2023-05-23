#pragma once

#include <cstdint>
#include <thread>
#include <mutex>

class DualShock4_Button {
public:
  enum {
    CROSS         = 1 << 0,
    CIRCLE        = 1 << 1,
    TRIANGLE      = 1 << 2,
    SQUARE        = 1 << 3,
    LEFT_BUMPER   = 1 << 4,
    RIGHT_BUMPER  = 1 << 5,
    LEFT_TRIGGER  = 1 << 6,
    RIGHT_TRIGGER = 1 << 7,
    SHARE         = 1 << 8,
    OPTIONS       = 1 << 9,
    PLAYSTATION   = 1 << 10,
    LEFT_STICK    = 1 << 11,
    RIGHT_STICK   = 1 << 12,
  };
};

class DualShock4_Axis {
public:
  enum {
    LEFT_X = 0,
    LEFT_Y = 1,
    RIGHT_X = 3,
    RIGHT_Y = 4,
    LEFT_TRIGGER = 2,
    RIGHT_TRIGGER = 5,
  };
};
