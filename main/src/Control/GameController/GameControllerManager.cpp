#include <BizarroHomer/Control/GameController/GameControllerManager.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>
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

bool GameControllerManager::get_controller_connected(int id) {
  std::lock_guard<std::mutex> lk(thrd_mut);
  return conn_controllers & (1 << id);
}

void GameControllerManager::terminate() {
  std::lock_guard<std::mutex> lk(thrd_mut);
  should_term = true;
}

void GameControllerManager::thrd_main() {
  SDL_Quit();
  SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
  SDL_Init(SDL_INIT_GAMECONTROLLER);
  
  uint16_t reg = 0;
  std::map<int, SDL_GameController*> reg_sdl_controllers;
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
          SDL_GameController* g = SDL_GameControllerOpen(i);
          if (!g) {
            continue;
          }
          SDL_GameControllerRumble(g, 0xFFFF, 0xFFFF, 5000);

          DualShock4_LEDManager::get()->update_controllers();
          // Set the bit.
          reg |= (1 << i);
          // Add to map.
          reg_sdl_controllers[i] = g;
          std::lock_guard<std::mutex> lk(thrd_mut);
          conn_controllers = reg;
        }
      }
    }
    
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      // Handle quit.
      if (ev.type == SDL_QUIT) break;
      // Handle disconnection.
      else if (ev.type == SDL_CONTROLLERDEVICEREMOVED) {
        fmt::print(stderr, "Controller disconnected\n");
        uint16_t which = ev.cdevice.which;
        // Close joystick.
        SDL_GameControllerClose(reg_sdl_controllers[which]);
        reg_sdl_controllers.erase(which);
        
        // Un-set the bit.
        reg &= ~(1 << which);
        
        // Zero inputs for controller.
        std::lock_guard<std::mutex> lk(thrd_mut);
        conn_controllers = reg;
        all_buttons[which] = 0;
        std::memset(&all_axes[which * 6], 0, 6 * sizeof(double));
      }
    }
    
    for (auto [i, g] : reg_sdl_controllers) {
      {
        // Zero inputs for controller bc we're gonna read new inputs.
        std::lock_guard<std::mutex> lk(thrd_mut);
        all_buttons[i] = 0;
        std::memset(&all_axes[i * 6], 0, 6 * sizeof(double));
      }
      
      // Get buttons.
      /* size_t nb = SDL_GameControllerNumButtons(g); */
      /* nb = nb > 16 ? 16 : nb; // Max 16 buttons. */

      std::lock_guard<std::mutex> lk(thrd_mut);
      
      auto add_button = [&](auto ds4_button, auto sdl_button) {
        all_buttons[i] |= (int)ds4_button * SDL_GameControllerGetButton(g, sdl_button);
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

      /* size_t nb = 16; */
      /* for (size_t n = 0; n < nb; n++) { */
      /*   // Get button. */
      /*   bool v = SDL_GameControllerGetButton(g, n); */
      /*   // Set button bit. */
      /*   all_buttons[i] |= (1 << n) * v; */
      /* } */
      
      auto add_axis = [&](auto ds4_axis, auto sdl_axis) {
        double v = ((double)SDL_GameControllerGetAxis(g, sdl_axis)) / 32767.0;
        all_axes[i * 6 + (int)ds4_axis] = v;
      };
      
      add_axis(DualShock4_Axis::LEFT_X, SDL_CONTROLLER_AXIS_LEFTX);
      add_axis(DualShock4_Axis::LEFT_Y, SDL_CONTROLLER_AXIS_LEFTY);
      add_axis(DualShock4_Axis::RIGHT_X, SDL_CONTROLLER_AXIS_RIGHTX);
      add_axis(DualShock4_Axis::RIGHT_Y, SDL_CONTROLLER_AXIS_RIGHTY);
      add_axis(DualShock4_Axis::LEFT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
      add_axis(DualShock4_Axis::RIGHT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
      
      // Get axes.
      /* size_t na = SDL_JoystickNumAxes(j); */
      /* na = na > 6 ? 6 : na; // Max 6 axes. */
      /* for (size_t n = 0; n < na; n++) { */
      /*   // Get axis. */
      /*   double v = ((double)SDL_JoystickGetAxis(j, n)) / 32767.0; */
      /*   // Set axis. */
      /*   std::lock_guard<std::mutex> lk(thrd_mut); */
      /*   all_axes[i * 6 + n] = v; */
      /* } */
    }
    
    // Sleep for 20ms.
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
  
  for (auto [i, j] : reg_sdl_controllers) {
    SDL_GameControllerClose(j);
  }
  
  SDL_Quit();
}

GameControllerManager GameControllerManager::instance;
