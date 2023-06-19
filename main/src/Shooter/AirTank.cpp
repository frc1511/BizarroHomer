#include <BizarroHomer/Shooter/AirTank.hpp>

#define VALVE_CLOSED 1
#define VALVE_OPEN (!VALVE_CLOSED)

AirTank::AirTank()
: fill_valve(GPIO_FILL_VALVE, VALVE_CLOSED),
  shoot_valve(GPIO_SHOOT_VALVE, VALVE_CLOSED) {
  
  fill_close_time_point = std::chrono::steady_clock::now();
  shoot_close_time_point = std::chrono::steady_clock::now();
}

AirTank::~AirTank() {

}

void AirTank::process() {
  using namespace std::literals::chrono_literals;
  
  auto safe_to_open_valve = [](auto close_time_point) -> bool {
    auto now = std::chrono::steady_clock::now();
    
    auto closed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - close_time_point);
    return closed_time >= 500ms;
  };
  
  auto close_valve = [](DigitalOutput* valve, std::chrono::steady_clock::time_point* close_time_point) {
    if (valve->get() == VALVE_OPEN) {
      (*close_time_point) = std::chrono::steady_clock::now();
    }
    valve->set(VALVE_CLOSED);
  };
  
  switch (next_state) {
    case State::IDLE:
      // Close both valves.
      close_valve(&fill_valve, &fill_close_time_point);
      close_valve(&shoot_valve, &shoot_close_time_point);
      break;
    case State::PRESSURIZING:
      // Close the shoot valve.
      close_valve(&shoot_valve, &shoot_close_time_point);
      // Don't open fill valve if at pressure. Also, wait 0.5 seconds after closing the shoot valve to open the fill valve.
      if (is_at_pressure() || !safe_to_open_valve(shoot_close_time_point)) {
        close_valve(&fill_valve, &fill_close_time_point);
      }
      else {
        fill_valve.set(VALVE_OPEN);
      }
      break;
    case State::SHOOTING:
      // Close the fill valve.
      close_valve(&fill_valve, &fill_close_time_point);
      // Wait 0.5 seconds after closing the fill valve to open the shoot valve.
      if (safe_to_open_valve(fill_close_time_point)) {
        shoot_valve.set(VALVE_OPEN);
      }
      else {
        close_valve(&shoot_valve, &shoot_close_time_point);
      }
      break;
  }
}

void AirTank::set_state(State state) {
  if (state == State::PRESSURIZING && is_at_pressure()) {
    next_state = State::IDLE;
  }
  else {
    next_state = state;
  }
}

bool AirTank::is_at_pressure() {
  // TODO: Detect pressure...
  return false;
}

bool AirTank::has_pressure() {
  // TODO: Detect pressure...
  return false;
}

bool AirTank::is_shooting() {
  return fill_valve.get();
}
