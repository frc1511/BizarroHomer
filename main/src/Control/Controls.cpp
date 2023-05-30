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
  if (!GameControllerManager::get()->get_controller_connected(GC_DRIVER)) {
    // Controller not connected... FREAK OUT!!!!
    
    drive->tank_control(0.0, 0.0);
    
    fmt::print("Controller {} not connected\n", GC_DRIVER);
    was_conn = false;
    
    return;
  }
  
  if (!was_conn) {
    fmt::print("green!!\n");
    DualShock4_LEDManager::get()->set_color(Color(0, 255, 0));
    was_conn = true;
  }
  
  // Get input from controller.
  GameControllerManager::get()->get_input(GC_DRIVER, &frame);
  
  // Toggle drive.
  {
    bool toggle_drive = false;
    bool should_toggle_drive = frame.buttons & DualShock4_Button::TRIANGLE;
    
    if (should_toggle_drive) toggle_drive = !was_drive_toggling;
    was_drive_toggling = should_toggle_drive;
    
    if (toggle_drive) {
      drive_mode = (drive_mode == DriveMode::ARCADE) ? DriveMode::TANK : DriveMode::ARCADE;
    }
  }
  
  auto improve_axis = [](double axis) -> double {
    // Deadzone.
    if (std::abs(axis) < 0.1) return 0.0;
    // Model sine curve.
    return std::sin(axis * M_PI_2);
  };

  if (drive_mode == DriveMode::ARCADE) {
    double forwards = -improve_axis(frame.axes[DualShock4_Axis::LEFT_Y]);
    double turn = improve_axis(frame.axes[DualShock4_Axis::RIGHT_X]);
    
    drive->arcade_control(forwards, turn);
  }
  else {
    double left = -improve_axis(frame.axes[DualShock4_Axis::LEFT_Y]);
    double right = -improve_axis(frame.axes[DualShock4_Axis::RIGHT_Y]);
    
    drive->tank_control(left, right);
  }
}
