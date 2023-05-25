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
  
  void set_colors(Color col_1, Color col_2);
  
  void set_should_update_controllers();
  
  void terminate();
  
private:
  void led_thread_main();
  void update_controllers();
  
  bool should_term = false;
  
  bool new_col = false;
  Color col_1, col_2;
  bool upd_ctrl = true;
  std::vector<std::string> ctrl_paths;
  
  std::thread led_thread;
  std::mutex led_mutex;
};
