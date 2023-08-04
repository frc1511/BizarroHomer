#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Illumination/BlinkyBlinky.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Shooter/Shooter.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>

#include <BizarroHomer/Hardware/IOMap.hpp>
#include <fmt/core.h>
#include <cmath>

// Button is currently held down.
#define BUTTON_DOWN(b) \
  static_cast<bool>(m_input.buttons & b)

// First frame that button is pressed.
#define BUTTON_PRESSED(b) \
  (static_cast<bool>(m_input.buttons & b) && !static_cast<bool>(m_last_input.buttons & b))

// First frame that button wasn't pressed.
#define BUTTON_RELEASED(b) \
  (!static_cast<bool>(m_input.buttons & b) && static_cast<bool>(m_last_input.buttons & b))

#define TRIGGER_DOWN(t) \
  (m_input.axes[t] > 0.75)

#define TRIGGER_PRESSED(t) \
  (m_input.axes[t] > 0.9 && m_last_input.axes[t] < 0.9)

Controls::Controls(Drive* drive, Shooter* shooter, thunder::PDP* pdp, BlinkyBlinky* blinky_blinky)
: m_drive(drive), m_shooter(shooter), m_pdp(pdp), m_blinky_blinky(blinky_blinky) { }

Controls::~Controls() = default;

void Controls::process() {
  m_last_input = m_input;
  m_input = DualShock4::InputFrame{};
  
  handle_leds();
  
  // Get input from controller.
  DualShock4::get()->get_input(&m_input);
  if (!DualShock4::get()->is_connected()) {
    return;
  }
  
  handle_drive();
  handle_shooter();
}

void Controls::handle_leds() {
  m_colors = 0;
  
  // KITT by default.
  /* BlinkyBlinky::LEDMode led_mode = BlinkyBlinky::LEDMode::KITT; */
  
  // Robot battery low.
  if (m_pdp->get_voltage() < 12.0) {
    m_colors |= DualShock4::ColorBits::RED;
    // Turn off the LEDs when the battery is low.
    /* led_mode = BlinkyBlinky::LEDMode::OFF; */
  }
  
  // Controller battery low.
  if (DualShock4::get()->get_battery_percentage() <= 0.2) {
    m_colors |= DualShock4::ColorBits::YELLOW;
  }
  // Controller batter getting low.
  else if (DualShock4::get()->get_battery_percentage() <= 0.5) {
    m_colors |= DualShock4::ColorBits::ORANGE;
  }
  
  // Warn about shooter pressure.
  if (m_shooter->has_pressure()) {
    m_colors |= DualShock4::ColorBits::PURPLE;
    
    // Rumble the controller when pressurized.
    double pressure_percentage = m_shooter->get_pressure() / 120.0;
    pressure_percentage = std::min(pressure_percentage, 1.0);
    
    DualShock4::get()->set_rumble(pressure_percentage, pressure_percentage);
    
    /* m_blinky_blinky->set_pressure(pressure_percentage); */
    /* led_mode = BlinkyBlinky::LEDMode::PRESSURE; */
  }
  
  // Green if everything is good.
  if (!m_colors) {
    m_colors = DualShock4::ColorBits::GREEN;
  }
  
  if (m_colors != m_last_colors) {
    DualShock4::get()->set_colors(m_colors);
    m_last_colors = m_colors;
  }
   
  /* m_blinky_blinky->set_mode(led_mode); */
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
    m_drive_mode = (m_drive_mode == DriveMode::ARCADE) ? DriveMode::TANK : DriveMode::ARCADE;
    fmt::print("Drive mode {}\n", m_drive_mode == DriveMode::TANK ? "TANK" : "ARCADE");
  }
  
  // Drive.
  if (m_drive_mode == DriveMode::ARCADE) {
    double forwards = -improve_axis(m_input.axes[DualShock4_Axis::LEFT_Y]);
    double turn = improve_axis(m_input.axes[DualShock4_Axis::RIGHT_X]);
    
    m_drive->arcade_control(forwards, turn);
  }
  else {
    double left = -improve_axis(m_input.axes[DualShock4_Axis::LEFT_Y]);
    double right = -improve_axis(m_input.axes[DualShock4_Axis::RIGHT_Y]);
    
    m_drive->tank_control(left, right);
  }
}

void Controls::handle_shooter() {
  // Pivot presets.
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_UP)) {
    m_shooter->set_pivot_preset(ShooterPivot::Preset::HIGH);
    fmt::print("Pivot preset HIGH\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::DPAD_DOWN)) {
    m_shooter->set_pivot_preset(ShooterPivot::Preset::LOW);
    fmt::print("Pivot preset LOW\n");
  }
  
  // Pivot manual speed.
  if (BUTTON_DOWN(DualShock4_Button::DPAD_LEFT)) {
    m_shooter->manual_pivot_control(-1.0);
  }
  if (BUTTON_DOWN(DualShock4_Button::DPAD_RIGHT)) {
    m_shooter->manual_pivot_control(+1.0);
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
    m_shooter->rotate_barrel(ShooterBarrel::RotationDirection::CLOCKWISE);
    fmt::print("Barrel rotate CLOCKWISE\n");
  }
  if (BUTTON_PRESSED(DualShock4_Button::OPTIONS)) {
    m_shooter->rotate_barrel(ShooterBarrel::RotationDirection::COUNTER_CLOCKWISE);
    fmt::print("Barrel rotate COUNTER CLOCKWISE\n");
  }
  
  bool is_pressuzizing = false;
  
  bool left_bumper_down      = BUTTON_DOWN(DualShock4_Button::LEFT_BUMPER);
  bool left_trigger_down     = TRIGGER_DOWN(DualShock4_Axis::LEFT_TRIGGER);
  bool right_bumper_down     = BUTTON_DOWN(DualShock4_Button::RIGHT_BUMPER);
  bool right_bumper_pressed  = BUTTON_PRESSED(DualShock4_Button::RIGHT_BUMPER);
  bool right_trigger_down    = TRIGGER_DOWN(DualShock4_Axis::RIGHT_TRIGGER);
  bool right_trigger_pressed = TRIGGER_PRESSED(DualShock4_Axis::RIGHT_TRIGGER);
  
  // Pressurizing.
  if (left_bumper_down && left_trigger_down && !right_bumper_down) {
    m_shooter->pressurize();
    is_pressuzizing = true;
  }
  // Shooting.
  else if (right_bumper_down && right_trigger_down &&
           (right_bumper_pressed || right_trigger_pressed) &&
           !left_bumper_down) {
    m_shooter->shoot();
    fmt::print(stdout, "Shoot\n");
  }
  
  if (is_pressuzizing != m_was_pressurizing) {
    fmt::print(stdout, "Pressurizing {}\n", is_pressuzizing ? "START" : "STOP");
  }
  m_was_pressurizing = is_pressuzizing;
}
