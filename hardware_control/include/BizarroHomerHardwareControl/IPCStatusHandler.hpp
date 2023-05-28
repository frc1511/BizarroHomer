#pragma once

#include <BizarroHomerHardwareControl/Hardware/Encoder/DutyCycleEncoder.hpp>
#define Phoenix_No_WPI
#include <ctre/Phoenix.h>
#include <mutex>
#include <thread>

class IPCStatusHandler {
public:
  IPCStatusHandler(std::mutex* hardware_mut, TalonFX* pivot_left, TalonFX* pivot_right, TalonFX* shooter_rot, DutyCycleEncoder* shooter_rot_enc);
  ~IPCStatusHandler();
  
  void terminate();
  
private:
  std::mutex* hardware_mut;
  TalonFX* pivot_left;
  TalonFX* pivot_right;
  TalonFX* shooter_rot;
  DutyCycleEncoder* shooter_rot_enc;
  
  std::thread status_thread;
  bool should_term = false;
  std::mutex status_mut;
  
  void status_thread_main();
};
