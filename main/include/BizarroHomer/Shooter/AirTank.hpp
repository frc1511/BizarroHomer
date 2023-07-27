#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/Digital_Output.hpp>
#include <chrono>

class AirTank : public Mechanism {
public:
  AirTank();
  ~AirTank();
  
  void process() override;
  void send_feedback() override;
  
  enum class State {
    IDLE, // Both valves closed.
    PRESSURIZING, // Fill valve open, shoot valve closed.
    SHOOTING, // Fill valve closed, shoot valve open.
  };
  
  void set_state(State state);
  
  bool is_at_pressure();
  bool has_pressure();
  
  bool is_shooting();
  
private:
  State next_state = State::IDLE;
  
  // Time points of when the valves were closed.
  std::chrono::steady_clock::time_point fill_close_time_point,
                                        shoot_close_time_point;
  
  Digital_Output fill_valve, shoot_valve;
};

