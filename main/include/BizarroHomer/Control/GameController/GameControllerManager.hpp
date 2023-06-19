#pragma once

#include <cstdint>
#include <thread>
#include <mutex>

class GameControllerManager {
public:
  static inline GameControllerManager* get() {
    return &instance;
  }
  
  GameControllerManager(GameControllerManager const&) = delete;
  GameControllerManager& operator=(GameControllerManager const&) = delete;
  
  void register_controller(int id);
  
  struct InputFrame {
    uint16_t buttons = 0;
    double axes[6] = { 0, 0, 0, 0, 0, 0 };
  };
  
  void get_input(int id, InputFrame* frame);

  bool get_controller_connected(int id);

  void terminate();
  
private:
  GameControllerManager();
  ~GameControllerManager();
  
  std::thread thrd;
  std::mutex thrd_mut;
  
  void thrd_main();
  
  enum {
    MAX_CONTROLLERS = 16,
  };
  
  uint16_t reg_controllers = 0;
  uint16_t conn_controllers = 0;
  
  uint16_t all_buttons[MAX_CONTROLLERS];
  double all_axes[6 * MAX_CONTROLLERS];

  bool should_term = false;
  
  static GameControllerManager instance;
};
