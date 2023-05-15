#pragma once

#include <cstdint>
#include <thread>
#include <mutex>

class PS4Controller {
public:
  PS4Controller(int id);
  
  enum class Button {
    SQUARE        = 1 << 0,
    CROSS         = 1 << 1,
    CIRCLE        = 1 << 2,
    TRIANGLE      = 1 << 3,
    LEFT_BUMPER   = 1 << 4,
    RIGHT_BUMPER  = 1 << 5,
    LEFT_TRIGGER  = 1 << 6,
    RIGHT_TRIGGER = 1 << 7,
    SHARE         = 1 << 8,
    OPTIONS       = 1 << 9,
    LEFT_STICK    = 1 << 10,
    RIGHT_STICK   = 1 << 11,
    PLAYSTATION   = 1 << 12,
    TOUCHPAD      = 1 << 13,
  };
  
  bool get_button(Button button);
  
  enum class Axis {
    LEFT_X         = 0,
    LEFT_Y         = 1,
    RIGHT_X        = 2,
    RIGHT_Y        = 5,
    LEFT_TRIGGER   = 3,
    RIGHT_TRIGGER  = 4,
  };
  
  double get_axis(Axis axis);
  
private:
  void ctrl_thread_start(int id);
  
  std::thread ctrl_thread;
  std::mutex input_mutex;
  
  uint32_t buttons = 0;
  uint8_t axes[6] = { 0, 0, 0, 0, 0, 0 };
};
