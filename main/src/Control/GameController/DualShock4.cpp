#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <SDL2/SDL.h>
#include <cstring>
#include <cassert>
#include <fmt/core.h>
#include <map>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define SYSFS_LED_PATH "/sys/class/leds"
#define SYSFS_POWER_SUPPLY_PATH "/sys/class/power_supply"

DualShock4::DualShock4() {
  SDL_Quit();
  SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
  SDL_Init(SDL_INIT_GAMECONTROLLER);
  
  m_controller = SDL_GameControllerOpen(0);
  
  rescan_leds();
  rescan_batteries();
  
  m_led_thread = std::thread([this]() {
    this->led_thread();
  });
}

DualShock4::~DualShock4() {
  if (m_controller) {
    SDL_GameControllerClose(m_controller);
  }
  
  SDL_Quit();
  
  std::lock_guard<std::mutex> lk(m_led_mutex);
  m_led_should_term = true;
}

bool DualShock4::is_connected() {
  return m_controller != nullptr;
}

bool DualShock4::try_connect() {
  if (m_controller) {
    return true;
  }
  
  SDL_Quit();
  SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
  SDL_Init(SDL_INIT_GAMECONTROLLER);
  
  m_controller = SDL_GameControllerOpen(0);
  if (!m_controller) {
    return false;
  }
  m_controller_id = 0;
  
  rescan_leds();
  rescan_batteries();
  
  return true;
}

void DualShock4::get_input(InputFrame* frame) {
  // Clear input.
  frame->buttons = 0;
  std::memset(frame->axes, 0, 6 * sizeof(double));
  
  if (!try_connect()) {
    return;
  }
  
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    // Handle quit.
    if (event.type == SDL_QUIT) {
      return;
    }
    // Handle disconnection.
    else if (event.type == SDL_CONTROLLERDEVICEADDED) {
      fmt::print("Controller connected\n");
      
      if (m_controller) {
        SDL_GameControllerClose(m_controller);
        m_controller = nullptr;
      }
      m_controller_id = event.cdevice.which;
      m_controller = SDL_GameControllerOpen(m_controller_id);
    }
    else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
      fmt::print("Controller disconnected\n");
      
      // Make sure it's the right controller.
      uint16_t which = event.cdevice.which;
      if (which == m_controller_id) {
        SDL_GameControllerClose(m_controller);
        m_controller = nullptr;
        return;
      }
    }
  }
  
  auto add_button = [&](auto ds4_button, auto sdl_button) {
    frame->buttons |= (int)ds4_button * SDL_GameControllerGetButton(m_controller, sdl_button);
  };
  
  add_button(DualShock4_Button::CROSS,        SDL_CONTROLLER_BUTTON_A);
  add_button(DualShock4_Button::CIRCLE,       SDL_CONTROLLER_BUTTON_B);
  add_button(DualShock4_Button::TRIANGLE,     SDL_CONTROLLER_BUTTON_Y);
  add_button(DualShock4_Button::SQUARE,       SDL_CONTROLLER_BUTTON_X);
  add_button(DualShock4_Button::LEFT_BUMPER,  SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
  add_button(DualShock4_Button::RIGHT_BUMPER, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
  add_button(DualShock4_Button::SHARE,        SDL_CONTROLLER_BUTTON_BACK);
  add_button(DualShock4_Button::OPTIONS,      SDL_CONTROLLER_BUTTON_START);
  add_button(DualShock4_Button::PLAYSTATION,  SDL_CONTROLLER_BUTTON_GUIDE);
  add_button(DualShock4_Button::LEFT_STICK,   SDL_CONTROLLER_BUTTON_LEFTSTICK);
  add_button(DualShock4_Button::RIGHT_STICK,  SDL_CONTROLLER_BUTTON_RIGHTSTICK);
  add_button(DualShock4_Button::DPAD_LEFT,    SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  add_button(DualShock4_Button::DPAD_RIGHT,   SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
  add_button(DualShock4_Button::DPAD_UP,      SDL_CONTROLLER_BUTTON_DPAD_UP);
  add_button(DualShock4_Button::DPAD_DOWN,    SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  
  auto add_axis = [&](auto ds4_axis, auto sdl_axis) {
    double v = ((double)SDL_GameControllerGetAxis(m_controller, sdl_axis)) / 32767.0;
    frame->axes[(int)ds4_axis] = v;
  };
  
  add_axis(DualShock4_Axis::LEFT_X,        SDL_CONTROLLER_AXIS_LEFTX);
  add_axis(DualShock4_Axis::LEFT_Y,        SDL_CONTROLLER_AXIS_LEFTY);
  add_axis(DualShock4_Axis::RIGHT_X,       SDL_CONTROLLER_AXIS_RIGHTX);
  add_axis(DualShock4_Axis::RIGHT_Y,       SDL_CONTROLLER_AXIS_RIGHTY);
  add_axis(DualShock4_Axis::LEFT_TRIGGER,  SDL_CONTROLLER_AXIS_TRIGGERLEFT);
  add_axis(DualShock4_Axis::RIGHT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
}

void DualShock4::set_rumble(double low_freq, double high_freq, std::chrono::milliseconds duration) {
  assert(low_freq >= 0.0 && low_freq <= 1.0);
  assert(high_freq >= 0.0 && high_freq <= 1.0);
  
  SDL_GameControllerRumble(m_controller,
      (uint16_t)(low_freq * 0xFFFF),
      (uint16_t)(high_freq * 0xFFFF),
      duration.count()
  );
}

void DualShock4::set_colors(uint8_t colors) {
  std::lock_guard<std::mutex> lk(m_led_mutex);
  m_led_colors = colors;
}

double DualShock4::get_battery_percentage() {
  if (m_battery_paths.empty()) {
    return 1.0;
  }
  
  std::filesystem::path capacity_path = *m_battery_paths.cbegin() / "capacity";
  
  int fd = open(capacity_path.c_str(), O_RDONLY);
  
  if (fd == -1) {
    fmt::print(stderr, "Failed to open {}: {}\n", capacity_path.string(), strerror(errno));
    rescan_batteries();
    return 1.0;
  }
  
  char buf[3];
  ssize_t bytes_read = read(fd, buf, 3);
  
  close(fd);
  
  // Convert to int.
  int cap = 0;
  for (int i = 0; i < bytes_read; i++) {
    if (!std::isdigit(buf[i])) {
      break;
    }
    cap *= 10;
    cap += buf[i] - '0';
  }
  
  return cap / 100.0;
}

void DualShock4::rescan_leds() {
  std::lock_guard<std::mutex> lk(m_led_mutex);
  
  m_led_paths.clear();
  
  for (const auto& entry : std::filesystem::directory_iterator(SYSFS_LED_PATH)) {
    std::string dir_name = entry.path().filename().string();
    
    if (dir_name.find("054C:09CC") != std::string::npos && dir_name.find(":red") != std::string::npos) {
      std::filesystem::path brightness_path = entry.path() / "brightness";
      
      if (std::filesystem::exists(brightness_path)) {
        m_led_paths.insert(
          std::filesystem::path(SYSFS_LED_PATH) / dir_name.substr(0, dir_name.find(":red") + 1)
        );
      }
    }
  }
  
  if (m_led_paths.size() > 1) {
    fmt::print("Warning: multiple DualShock 4 LEDs found\n");
  }
}

void DualShock4::rescan_batteries() {
  m_battery_paths.clear();
  
  for (const auto& entry : std::filesystem::directory_iterator(SYSFS_POWER_SUPPLY_PATH)) {
    std::string dir_name = entry.path().filename().string();
    
    if (dir_name.find("sony_controller_battery") != std::string::npos) {
      m_battery_paths.insert(entry.path());
    }
  }
  
  if (m_battery_paths.size() > 1) {
    fmt::print("Warning: multiple DualShock 4 batteries found\n");
  }
}

void DualShock4::led_thread() {
  uint8_t colors = 0;
  while (true) {
    {
      std::lock_guard<std::mutex> lk(m_led_mutex);
      colors = m_led_colors;
      
      if (m_led_should_term) {
        break;
      }
    }
    
    uint8_t r, g, b;
    
    for (int i = 0; i < 7; i++) {
      uint8_t bit = 1 << i;
      
      if (colors & bit) {
        switch (bit) {
          case RED:
            r = 255, g = 0, b = 0;
            break;
          case ORANGE:
            r = 255, g = 128, b = 0;
            break;
          case YELLOW:
            r = 255, g = 255, b = 0;
            break;
          case GREEN:
            r = 0, g = 255, b = 0;
            break;
          case BLUE:
            r = 0, g = 0, b = 255;
            break;
          case PURPLE:
            r = 255, g = 0, b = 255;
            break;
          case OFF:
          default:
            r = g = b = 0;
            break;
        }
        
        std::string r_str = std::to_string(r);
        std::string g_str = std::to_string(g);
        std::string b_str = std::to_string(b);
        
        m_led_mutex.lock();
        
        int fd;
        for (const std::filesystem::path& path : m_led_paths) {
          std::string path_str = path.string();
          
          m_led_mutex.unlock();
          
          std::string r_path = fmt::format("{}red/brightness",   path_str);
          std::string g_path = fmt::format("{}green/brightness", path_str);
          std::string b_path = fmt::format("{}blue/brightness",  path_str);
          
          fd = open(r_path.c_str(), O_WRONLY | O_TRUNC);
          write(fd, r_str.c_str(), r_str.length());
          close(fd);
          
          fd = open(g_path.c_str(), O_WRONLY | O_TRUNC);
          write(fd, g_str.c_str(), g_str.length());
          close(fd);
          
          fd = open(b_path.c_str(), O_WRONLY | O_TRUNC);
          write(fd, b_str.c_str(), b_str.length());
          close(fd);
          
          m_led_mutex.lock();
        }
        
        m_led_mutex.unlock();
        
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(0.25s);
      }
    }
  }
}

DualShock4 DualShock4::instance;
