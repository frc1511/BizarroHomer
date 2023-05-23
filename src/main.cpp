#include <iostream>

#include <fmt/core.h>
#include <thread>
#include <chrono>

#include <BizarroHomer/Control/GameController/GameControllerManager.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>

int main() {
  GameControllerManager::get()->register_controller(0);
  GameControllerManager::InputFrame frame;
  
  while (true) {
    GameControllerManager::get()->get_input(0, &frame);
    fmt::print("{}, {}\n", frame.buttons & DualShock4_Button::TRIANGLE, frame.axes[DualShock4_Axis::LEFT_X]);
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }

  return 0;
}
