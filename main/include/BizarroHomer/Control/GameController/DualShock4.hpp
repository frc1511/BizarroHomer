#pragma once

#include <filesystem>
#include <set>
#include <vector>
#include <thread>
#include <mutex>

using namespace std::literals::chrono_literals;

typedef struct _SDL_GameController SDL_GameController;

class DualShock4_Button {
public:
  enum {
    CROSS         = 1 << 0,
    CIRCLE        = 1 << 1,
    TRIANGLE      = 1 << 2,
    SQUARE        = 1 << 3,
    LEFT_BUMPER   = 1 << 4,
    RIGHT_BUMPER  = 1 << 5,
    SHARE         = 1 << 6,
    OPTIONS       = 1 << 7,
    PLAYSTATION   = 1 << 8,
    LEFT_STICK    = 1 << 9,
    RIGHT_STICK   = 1 << 10,
    DPAD_LEFT     = 1 << 11,
    DPAD_RIGHT    = 1 << 12,
    DPAD_UP       = 1 << 13,
    DPAD_DOWN     = 1 << 14,
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

class DualShock4 {
public:
  static inline DualShock4* get() {
    return &instance;
  }
  
  DualShock4(DualShock4 const&) = delete;
  DualShock4& operator=(DualShock4 const&) = delete;
  
  bool is_connected();
  bool try_connect();
  
  struct InputFrame {
    uint16_t buttons = 0;
    double axes[6] = { 0, 0, 0, 0, 0, 0 };
  };
  
  void get_input(InputFrame* frame);
  void set_rumble(double low_freq, double high_freq, std::chrono::milliseconds duration = 50ms);
  
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
  
  double get_battery_percentage();
  
private:
  DualShock4();
  ~DualShock4();
  
  void rescan_leds();
  void rescan_batteries();
  
  std::set<std::filesystem::path> m_led_paths;
  std::set<std::filesystem::path> m_battery_paths;
  
  SDL_GameController* m_controller = nullptr;
  
  std::thread m_led_thread;
  std::mutex m_led_mutex;
  
  bool m_led_should_term = false;
  uint8_t m_led_colors = 0;

  void led_thread();
  
  static DualShock4 instance;
};
