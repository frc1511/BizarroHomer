#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Drive/Drive.hpp>

#include <BizarroHomer/Control/GameController/GameControllerManager.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <fmt/core.h>
#include <cmath>

Controls::Controls(Drive* _drive)
: drive(_drive) {
  
  // Register game controller.
  GameControllerManager::get()->register_controller(GC_DRIVER);
}

Controls::~Controls() {

}

void Controls::process() {
  GameControllerManager::InputFrame frame;
  bool conn = GameControllerManager::get()->get_controller_connected(GC_DRIVER);
  if (!conn) {
    // Controller not connected...
    // TODO: Do something crazy.
    
    drive->control(0.0, 0.0);
    
    fmt::print("Controller {} not connected\n", GC_DRIVER);
    return;
  }
  
  // Get input from controller.
  GameControllerManager::get()->get_input(GC_DRIVER, &frame);
  
  auto improve_axis = [](double axis) -> double {
      return std::sin(axis * M_PI_2);
  };
  
  double left = improve_axis(frame.axes[DualShock4_Axis::LEFT_Y]);
  double right = improve_axis(frame.axes[DualShock4_Axis::RIGHT_Y]);
  
  // Deadzones.
  if (std::abs(left) < 0.1) {
    left = 0.0;
  }
  if (std::abs(right) < 0.1) {
    right = 0.0;
  }
  
  fmt::print("controlling drive: {} ---- {}\n", left, right);
  
  drive->control(left, right);
}
