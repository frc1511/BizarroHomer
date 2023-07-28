#include <BizarroHomer/Shooter/AirTank.hpp>
#include <BizarroHomer/Basic/FeedbackManager.hpp>

#define VALVE_CLOSED 0
#define VALVE_OPEN (!VALVE_CLOSED)

AirTank::AirTank()
: m_fill_valve(GPIO_FILL_VALVE, VALVE_CLOSED),
  m_shoot_valve(GPIO_SHOOT_VALVE, VALVE_CLOSED) {
  
  m_fill_close_time_point = std::chrono::steady_clock::now();
  m_shoot_close_time_point = std::chrono::steady_clock::now();
}

AirTank::~AirTank() = default;

void AirTank::process() {
  using namespace std::literals::chrono_literals;
  
  auto safe_to_open_valve = [](auto close_time_point) -> bool {
    auto now = std::chrono::steady_clock::now();
    
    auto closed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - close_time_point);
    return closed_time >= 500ms;
  };
  
  auto close_valve = [](thunder::DigitalOutput* valve, std::chrono::steady_clock::time_point* close_time_point) {
    if (valve->get() == VALVE_OPEN) {
      (*close_time_point) = std::chrono::steady_clock::now();
    }
    valve->set(VALVE_CLOSED);
  };
  
  switch (m_next_state) {
    case State::IDLE:
      // Close both valves.
      close_valve(&m_fill_valve, &m_fill_close_time_point);
      close_valve(&m_shoot_valve, &m_shoot_close_time_point);
      break;
    case State::PRESSURIZING:
      // Close the shoot valve.
      close_valve(&m_shoot_valve, &m_shoot_close_time_point);
      // Don't open fill valve if at pressure. Also, wait 0.5 seconds after closing the shoot valve to open the fill valve.
      if (is_at_pressure() || !safe_to_open_valve(m_shoot_close_time_point)) {
        close_valve(&m_fill_valve, &m_fill_close_time_point);
      }
      else {
        m_fill_valve.set(VALVE_OPEN);
      }
      break;
    case State::SHOOTING:
      // Close the fill valve.
      close_valve(&m_fill_valve, &m_fill_close_time_point);
      // Wait 0.5 seconds after closing the fill valve to open the shoot valve.
      if (safe_to_open_valve(m_fill_close_time_point)) {
        m_shoot_valve.set(VALVE_OPEN);
      }
      else {
        close_valve(&m_shoot_valve, &m_shoot_close_time_point);
      }
      break;
  }
}

void AirTank::set_state(State state) {
  if (state == State::PRESSURIZING && is_at_pressure()) {
    m_next_state = State::IDLE;
  }
  else {
    m_next_state = state;
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
  return m_fill_valve.get();
}

void AirTank::send_feedback() {
  std::string state_str;
  switch (m_next_state) {
    case State::IDLE:
      state_str = "Idle";
      break;
    case State::PRESSURIZING:
      state_str = "Pressurizing";
      break;
    case State::SHOOTING:
      state_str = "Shooting";
      break;
  }
  
  FeedbackManager::get()->send_value("AirTank_NextState", state_str);

  bool _fill = m_fill_valve.get();
  bool _shoot = m_shoot_valve.get();
  
  state_str = "Idle";
  if (_fill == VALVE_OPEN && _shoot == VALVE_CLOSED) {
    state_str = "Pressurizing";
  }
  else if (_fill == VALVE_CLOSED && _shoot == VALVE_OPEN) {
    state_str = "Shooting";
  }
  else if (_fill == VALVE_OPEN && _shoot == VALVE_OPEN) {
    state_str = "BAD THINGS!!";
  }
  
  FeedbackManager::get()->send_value("AirTank_CurrState", state_str);
}
