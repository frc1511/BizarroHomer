#pragma once

#include <BizarroHomerHardwareControl/Hardware/MotorControl/PWMSparkMax.hpp>
#include <BizarroHomerHardwareControl/Hardware/Solenoid.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#define Phoenix_No_WPI
#include <ctre/Phoenix.h>
#include <mutex>

class ControlHandler {
public:
  ControlHandler(std::mutex* hardware_mut, PWMSparkMax* drive_left, PWMSparkMax* drive_right, TalonFX* pivot_left, TalonFX* pivot_right, TalonFX* shooter_rot, Solenoid* fill_valve, Solenoid* shoot_valve);
  
  /**
   * @brief Stops all hardware from moving.
   */
  void stop();
  
  /**
   * @brief Shuts down the control handler by stopping all hardware from moving and ignoring all future messages.
   */
  void shutdown();
  
  void handle_control_msg();
  
private:
  bool is_shutdown = false;
  
  std::mutex* hardware_mut;
  PWMSparkMax* drive_left;
  PWMSparkMax* drive_right;
  TalonFX* pivot_left;
  TalonFX* pivot_right;
  TalonFX* shooter_rot;
  Solenoid* fill_valve;
  Solenoid* shoot_valve;
  
  IPCReceiver r;
};
