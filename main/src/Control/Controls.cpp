#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Shooter/Shooter.hpp>

#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <fmt/core.h>
#include <cmath>

// Button is currently held down.
#define BUTTON_DOWN(b) \
  static_cast<bool>(input.buttons & b)

// First frame that button is pressed.
#define BUTTON_PRESSED(b) \
  static_cast<bool>(input.buttons & b) && !static_cast<bool>(last_input.buttons & b)

// First frame that button wasn't pressed.
#define BUTTON_RELEASED(b) \
  !static_cast<bool>(input.buttons & b) && static_cast<bool>(last_input.buttons & b)

Controls::Controls(Drive* _drive, Shooter* _shooter)
: drive(_drive), shooter(_shooter) {
  
  // Register game controller.
  GameControllerManager::get()->register_controller(GC_DRIVER);
}

Controls::~Controls() {

}

void Controls::process() {
  last_input = input;
  input = GameControllerManager::InputFrame{};
  
  if (!GameControllerManager::get()->get_controller_connected(GC_DRIVER)) {
    // Controller not connected... FREAK OUT!!!!
    
    drive->tank_control(0.0, 0.0);
    
    was_conn = false;
    
    return;
  }

  if (!was_conn) {
    DualShock4_LEDManager::get()->update_controllers();
  }
  
  colors = 0;
  /*
  if (pdp_voltage < 11.8) {
    // red
  }
  if (controller_voltage < 40) {
    // yellow
  }
  */
  
  // Warn about shooter pressure.
  if (shooter->is_at_pressure()) {
    colors |= DualShock4_LEDManager::ColorBits::ORANGE;
  }
  else if (shooter->has_pressure()) {
    colors |= DualShock4_LEDManager::ColorBits::PURPLE;
  }
  
  // Green if everything is good.
  if (!colors) {
    colors = DualShock4_LEDManager::ColorBits::GREEN;
  }
  
  if (colors != last_colors || !was_conn) {
    DualShock4_LEDManager::get()->set_colors(colors);
    last_colors = colors;
    was_conn = true;
  }
  
  // Get input from controller.
  GameControllerManager::get()->get_input(GC_DRIVER, &input);
  
  auto improve_axis = [](double axis) -> double {
    // Deadzone.
    if (std::abs(axis) < 0.1) return 0.0;
    // Model sine curve.
    return std::sin(axis * M_PI_2);
  };
  
  // Drive.
  if (drive_mode == DriveMode::ARCADE) {
    double forwards = -improve_axis(input.axes[DualShock4_Axis::LEFT_Y]);
    double turn = improve_axis(input.axes[DualShock4_Axis::RIGHT_X]);
    
    drive->arcade_control(forwards, turn);
  }
  else {
    double left = -improve_axis(input.axes[DualShock4_Axis::LEFT_Y]);
    double right = -improve_axis(input.axes[DualShock4_Axis::RIGHT_Y]);
    
    drive->tank_control(left, right);
  }
  
  // Pivot presets.
  if (BUTTON_PRESSED(DualShock4_Button::TRIANGLE)) {
    shooter->set_preset(ShooterPivot::Preset::HIGH);
  }
  if (BUTTON_PRESSED(DualShock4_Button::SQUARE)) {
    shooter->set_preset(ShooterPivot::Preset::MID);
  }
  if (BUTTON_PRESSED(DualShock4_Button::CIRCLE)) {
    shooter->set_preset(ShooterPivot::Preset::LOW);
  }
  
  // Barrel rotation.
  if (BUTTON_PRESSED(DualShock4_Button::SHARE)) {
    shooter->rotate_barrel(ShooterBarrel::RotationDirection::CLOCKWISE);
  }
  if (BUTTON_PRESSED(DualShock4_Button::OPTIONS)) {
    shooter->rotate_barrel(ShooterBarrel::RotationDirection::COUNTER_CLOCKWISE);
  }
  
  // Pressurizing & Shooting.
  if (BUTTON_DOWN(DualShock4_Button::CROSS)) {
    if (BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER) && !BUTTON_DOWN(DualShock4_Button::RIGHT_BUMPER)) {
      shooter->pressurize();
    }
    if (BUTTON_PRESSED(DualShock4_Button::RIGHT_BUMPER) && !BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER)) {
      shooter->shoot();
    }
  }
}
/**
 *
 *
    // Toggle drive mode.
    drive_mode = (drive_mode == DriveMode::ARCADE) ? DriveMode::TANK : DriveMode::ARCADE;
 *
 */
