#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Shooter/Shooter.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

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

Controls::Controls(Drive* _drive, Shooter* _shooter, CAN_PDP* _pdp)
: drive(_drive), shooter(_shooter), pdp(_pdp) {
  
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
    DualShock4_BatteryManager::get()->rescan_controllers();
  }
  
  colors = 0;
  
  // Robot battery low.
  if (pdp->get_voltage() < 12.0) {
    colors |= DualShock4_LEDManager::ColorBits::RED;
  }
  // Controller battery low.
  if (DualShock4_BatteryManager::get()->get_percentage() <= 0.2) {
    colors |= DualShock4_LEDManager::ColorBits::YELLOW;
  }
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
  
  if (BUTTON_PRESSED(DualShock4_Button::PLAYSTATION)) {
    HardwareManager::get()->start_music();
  }
  
  // Toggle drive mode.
  if (BUTTON_PRESSED(DualShock4_Button::TRIANGLE)) {
    drive_mode = (drive_mode == DriveMode::ARCADE) ? DriveMode::TANK : DriveMode::ARCADE;
    fmt::print("Drive mode {}\n", drive_mode == DriveMode::TANK ? "TANK" : "ARCADE");
  }
  
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
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_UP)) {
    shooter->set_preset(ShooterPivot::Preset::HIGH);
    fmt::print("Preset HIGH\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_RIGHT)) {
    shooter->set_preset(ShooterPivot::Preset::MID);
    fmt::print("Preset MID\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_DOWN)) {
    shooter->set_preset(ShooterPivot::Preset::LOW);
    fmt::print("Preset LOW\n");
  }
  
  // Barrel rotation.
  if (BUTTON_PRESSED(DualShock4_Button::SHARE)) {
    shooter->rotate_barrel(ShooterBarrel::RotationDirection::CLOCKWISE);
    fmt::print("Barrel CLOCKWISE\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::OPTIONS)) {
    shooter->rotate_barrel(ShooterBarrel::RotationDirection::COUNTER_CLOCKWISE);
    fmt::print("Barrel COUNTER CLOCKWISE\n");
  }
  
  bool is_pressuzizing = false;
  
  // Pressurizing & Shooting.
  if (BUTTON_DOWN(DualShock4_Button::CROSS)) {
    if (BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER) && !BUTTON_DOWN(DualShock4_Button::RIGHT_BUMPER)) {
      shooter->pressurize();
      is_pressuzizing = true;
    }
    if (BUTTON_PRESSED(DualShock4_Button::RIGHT_BUMPER) && !BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER)) {
      shooter->shoot();
      fmt::print(stdout, "Shoot\n");
    }
  }
  
  if (is_pressuzizing != was_pressurizing) {
    fmt::print(stdout, "Pressurizing {}\n", is_pressuzizing ? "START" : "STOP");
  }
  was_pressurizing = is_pressuzizing;
}
