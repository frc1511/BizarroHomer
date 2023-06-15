#pragma once

#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <BizarroHomer/Util/Color.hpp>
#include <cstdint>
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

class DualShock4_LEDManager {
public:
  static inline DualShock4_LEDManager* get() {
    return &instance;
  }
  
  DualShock4_LEDManager(DualShock4_LEDManager const&) = delete;
  DualShock4_LEDManager& operator=(DualShock4_LEDManager const&) = delete;
  
  enum ColorBits {
    RED    = 1 << 0,
    ORANGE = 1 << 1,
    YELLOW = 1 << 2,
    GREEN  = 1 << 3,
    BLUE   = 1 << 4,
    PURPLE = 1 << 5,
    OFF    = 1 << 6,
  };
  
  void set_colors(uint8_t colors);
  void update_controllers();
  
private:
  DualShock4_LEDManager();
  ~DualShock4_LEDManager();
  
  uint8_t colors = 0;

  void send_msg(bool update_ctrl);
  
  IPCSender s;
  std::mutex led_mutex;
  
  static DualShock4_LEDManager instance;
};

