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
  
  void set_color(Color col);
  void set_alternating_colors(Color col_1, Color col_2);
  void update_controllers();
  
private:
  DualShock4_LEDManager();
  ~DualShock4_LEDManager();
  
  Color col_1 { 0, 0, 0 },
        col_2 { 0, 0, 0 };

  void send_msg(bool update_ctrl);
  
  IPCSender s;
  std::mutex led_mutex;
  
  static DualShock4_LEDManager instance;
};

