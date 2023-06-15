#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdint>

struct Color {
  uint8_t r, g, b;
};

class LEDHandler {
public:
  LEDHandler();
  ~LEDHandler();
  
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
  
  void set_should_update_controllers();
  
  void terminate();
  
private:
  void led_thread_main();
  void update_controllers();
  
  bool should_term = false;
  
  bool new_cols = false;
  uint8_t colors = 0;
  bool upd_ctrl = true;
  std::vector<std::string> ctrl_paths;
  
  std::thread led_thread;
  std::mutex led_mutex;
};
