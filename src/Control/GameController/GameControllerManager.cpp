#include <BizarroHomer/Control/GameController/GameControllerManager.hpp>
#include <SDL2/SDL.h>
#include <cstring>
#include <cassert>
#include <fmt/core.h>
#include <map>

GameControllerManager::GameControllerManager()
: thrd([&]() { this->thrd_main(); }) { }

GameControllerManager::~GameControllerManager() {
  terminate();
}

void GameControllerManager::register_controller(int id) {
  std::lock_guard<std::mutex> lk(thrd_mut);
  // Zero inputs for controller.
  all_buttons[id] = 0;
  std::memset(&all_axes[id * 6], 0, 6 * sizeof(double));
  
  reg_controllers |= (1 << id);
}

void GameControllerManager::get_input(int id, InputFrame* frame) {
  assert(frame);
  std::lock_guard<std::mutex> lk(thrd_mut);
  
  frame->buttons = all_buttons[id];
  std::memcpy(&frame->axes, &all_axes[id * 6], 6 * sizeof(double));
}

void GameControllerManager::terminate() {
  std::lock_guard<std::mutex> lk(thrd_mut);
  should_term = true;
}

void GameControllerManager::thrd_main() {
  SDL_Quit();
  SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
  SDL_Init(SDL_INIT_JOYSTICK);
  
  uint16_t reg = 0;
  std::map<int, SDL_Joystick*> reg_joysticks;
  while (true) {
    uint16_t should_reg;
    {
      std::lock_guard<std::mutex> lk(thrd_mut);
      should_reg = reg_controllers;
      if (should_term) break;
    }
    
    if (reg != should_reg) {
      for (int i = 0; i < MAX_CONTROLLERS; i++) {
        if (should_reg & (1 << i) && !(reg & (1 << i))) {
          // Open a Jostick at id.
          SDL_Joystick* j = SDL_JoystickOpen(i);
          if (!j) {
            fmt::print("SDL_JoystickOpen({}) failed.\n", i);
            continue;
          }
          // Set the bit.
          reg |= (1 << i);
          // Add to map.
          reg_joysticks[i] = j;
        }
      }
    }
    
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      // Handle quit.
      if (ev.type == SDL_QUIT) break;
      // Handle disconnection.
      if (ev.jdevice.type == SDL_JOYDEVICEREMOVED) {
        uint16_t which = ev.jdevice.which;
        // Close joystick.
        SDL_JoystickClose(reg_joysticks[which]);
        reg_joysticks.erase(which);
        
        // Zero inputs for controller.
        std::lock_guard<std::mutex> lk(thrd_mut);
        all_buttons[which] = 0;
        std::memset(&all_axes[which * 6], 0, 6 * sizeof(double));
      }
    }
    
    for (auto [i, j] : reg_joysticks) {
      {
        // Zero inputs for controller bc we're gonna read new inputs.
        std::lock_guard<std::mutex> lk(thrd_mut);
        all_buttons[i] = 0;
        std::memset(&all_axes[i * 6], 0, 6 * sizeof(double));
      }
      
      // Get buttons.
      size_t nb = SDL_JoystickNumButtons(j);
      nb = nb > 16 ? 16 : nb; // Max 16 buttons.
      for (size_t n = 0; n < nb; n++) {
        // Get button.
        bool v = SDL_JoystickGetButton(j, n);
        // Set button bit.
        std::lock_guard<std::mutex> lk(thrd_mut);
        all_buttons[i] |= (1 << n) * v;
      }
      
      // Get axes.
      size_t na = SDL_JoystickNumAxes(j);
      na = na > 6 ? 6 : na; // Max 6 axes.
      for (size_t n = 0; n < na; n++) {
        // Get axis.
        double v = ((double)SDL_JoystickGetAxis(j, n)) / 32767.0;
        // Set axis.
        std::lock_guard<std::mutex> lk(thrd_mut);
        all_axes[i * 6 + n] = v;
      }
    }
    
    // Sleep for 20ms.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
  
  for (auto [i, j] : reg_joysticks) {
    SDL_JoystickClose(j);
  }
  
  SDL_Quit();
}

GameControllerManager GameControllerManager::instance;
