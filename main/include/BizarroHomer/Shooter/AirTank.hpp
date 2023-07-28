#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <BizarroHomer/Hardware/DigitalOutput.hpp>
#include <chrono>

class AirTank : public Mechanism {
public:
  AirTank();
  ~AirTank();
  
  void process() override;
  void send_feedback() override;
  
  enum class State {
    IDLE,         // Both valves closed.
    PRESSURIZING, // Fill valve open, shoot valve closed.
    SHOOTING,     // Fill valve closed, shoot valve open.
  };
  
  void set_state(State state);
  
  //
  // Whether the fill tank has reached 120 PSI.
  //
  bool is_at_pressure();
  
  //
  // Whether the fill tank has any pressure.
  //
  bool has_pressure();
  
  //
  // Whether the shoot valve is open.
  //
  bool is_shooting();
  
private:
  State m_next_state = State::IDLE;
  
  // Time points of when the valves were closed.
  std::chrono::steady_clock::time_point m_fill_close_time_point,
                                        m_shoot_close_time_point;
  
  thunder::DigitalOutput m_fill_valve, m_shoot_valve;
};

