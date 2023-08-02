#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Shooter/Shooter.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#include <BizarroHomer/Hardware/IOMap.hpp>
#include <fmt/core.h>
#include <cmath>

// Button is currently held down.
#define BUTTON_DOWN(b) \
  static_cast<bool>(input.buttons & b)

// First frame that button is pressed.
#define BUTTON_PRESSED(b) \
  (static_cast<bool>(input.buttons & b) && !static_cast<bool>(last_input.buttons & b))

// First frame that button wasn't pressed.
#define BUTTON_RELEASED(b) \
  (!static_cast<bool>(input.buttons & b) && static_cast<bool>(last_input.buttons & b))

#define TRIGGER_DOWN(t) \
  (input.axes[t] > 0.75)


Controls::Controls(Drive* _drive, Shooter* _shooter, thunder::PDP* _pdp)
: drive(_drive), shooter(_shooter), pdp(_pdp) { }

Controls::~Controls() = default;

void Controls::process() {
  last_input = input;
  input = DualShock4::InputFrame{};
  
  handle_leds();
  
  // Get input from controller.
  DualShock4::get()->get_input(&input);
  if (!DualShock4::get()->is_connected()) {
    return;
  }
  
  handle_drive();
  handle_shooter();
}

void Controls::handle_leds() {
  colors = 0;
  
  // Robot battery low.
  if (pdp->get_voltage() < 12.0) {
    colors |= DualShock4::ColorBits::RED;
  }
  // Controller battery low.
  if (DualShock4::get()->get_battery_percentage() <= 0.2) {
    colors |= DualShock4::ColorBits::YELLOW;
  }
  // Warn about shooter pressure.
  if (shooter->is_at_pressure()) {
    colors |= DualShock4::ColorBits::ORANGE;
  }
  else if (shooter->has_pressure()) {
    colors |= DualShock4::ColorBits::PURPLE;
    
    // Rumble the controller when pressurized.
    double pressure_percentage = shooter->get_pressure() / 120.0;
    pressure_percentage = std::min(pressure_percentage, 1.0);
    
    DualShock4::get()->set_rumble(pressure_percentage, pressure_percentage);
  }
  
  // Green if everything is good.
  if (!colors) {
    colors = DualShock4::ColorBits::GREEN;
  }
  
  if (colors != last_colors) {
    DualShock4::get()->set_colors(colors);
    last_colors = colors;
  }
}

void Controls::handle_drive() {
  auto improve_axis = [](double axis) -> double {
    // Deadzone.
    if (std::abs(axis) < 0.1) return 0.0;
    // Model sine curve.
    return std::sin(axis * M_PI_2);
  };
  
  if (BUTTON_PRESSED(DualShock4_Button::PLAYSTATION)) {
    thunder::HardwareManager::get()->start_song(thunder::HardwareManager::HOME_DEPOT_BEAT);
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
}

void Controls::handle_shooter() {
  // Pivot presets.
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_UP)) {
    shooter->set_pivot_preset(ShooterPivot::Preset::HIGH);
    fmt::print("Pivot preset HIGH\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_DOWN)) {
    shooter->set_pivot_preset(ShooterPivot::Preset::LOW);
    fmt::print("Pivot preset LOW\n");
  }
  
  // Pivot manual speed.
  if (BUTTON_DOWN(DualShock4_Button::DPAD_LEFT)) {
    shooter->manual_pivot_control(-1.0);
  }
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_RIGHT)) {
    shooter->manual_pivot_control(+1.0);
  }
  
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_LEFT)) {
    fmt::print("Pivoting DOWN START\n");
  }
  if (BUTTON_RELEASED(DualShock4_Button::DPAD_LEFT)) {
    fmt::print("Pivoting DOWN END\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_RIGHT)) {
    fmt::print("Pivoting UP START\n");
  }
  if (BUTTON_RELEASED(DualShock4_Button::DPAD_RIGHT)) {
    fmt::print("Pivoting UP END\n");
  }
  
  // Barrel rotation.
  if (BUTTON_PRESSED(DualShock4_Button::SHARE)) {
    shooter->rotate_barrel(ShooterBarrel::RotationDirection::CLOCKWISE);
    fmt::print("Barrel rotate CLOCKWISE\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::OPTIONS)) {
    shooter->rotate_barrel(ShooterBarrel::RotationDirection::COUNTER_CLOCKWISE);
    fmt::print("Barrel rotate COUNTER CLOCKWISE\n");
  }
  
  bool is_pressuzizing = false;

  // Pressurizing.
  if (BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER) &&
      TRIGGER_DOWN(DualShock4_Axis::LEFT_TRIGGER) &&
      !BUTTON_DOWN(DualShock4_Button::RIGHT_BUMPER)) {
    
    shooter->pressurize();
    is_pressuzizing = true;
  }
  // Shooting.
  else if (BUTTON_DOWN(DualShock4_Button::RIGHT_BUMPER) &&
           TRIGGER_DOWN(DualShock4_Axis::RIGHT_TRIGGER) &&
           !BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER)) {
    
    shooter->shoot();
    fmt::print(stdout, "Shoot\n");
  }
  
  if (is_pressuzizing != was_pressurizing) {
    fmt::print(stdout, "Pressurizing {}\n", is_pressuzizing ? "START" : "STOP");
  }
  was_pressurizing = is_pressuzizing;
}
