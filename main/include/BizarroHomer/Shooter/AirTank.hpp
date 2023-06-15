#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/DigitalIO/DigitalOutput.hpp>
#include <chrono>

class AirTank : public Mechanism {
public:
  AirTank();
  ~AirTank();
  
  void process() override;

  enum class State {
    IDLE, // Both valves closed.
    PRESSURIZING, // Fill valve open, shoot valve closed.
    SHOOTING, // Fill valve closed, shoot valve open.
  };
  
  void set_state(State state);
  
  bool is_at_pressure();
  bool has_pressure();
  
private:
  State next_state = State::IDLE;
  
  // Time points of when the valves were closed.
  std::chrono::steady_clock::time_point fill_close_time_point,
                                        shoot_close_time_point;
  
  DigitalOutput fill_valve, shoot_valve;
};
