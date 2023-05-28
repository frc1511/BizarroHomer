#include <BizarroHomerHardwareControl/Hardware/MotorControl/PWMSparkMax.hpp>
#include <BizarroHomerHardwareControl/Hardware/Encoder/DutyCycleEncoder.hpp>
#include <BizarroHomerHardwareControl/Hardware/Solenoid.hpp>
#include <BizarroHomerHardwareControl/IPCStatusHandler.hpp>
#include <BizarroHomerHardwareControl/IPCControlHandler.hpp>
#include <BizarroHomerHardwareControl/IOMap.hpp>
#define Phoenix_No_WPI
#include <ctre/Phoenix.h>
#include <csignal>
#include <cstring>
#include <csignal>
#include <mutex>
#include <fmt/core.h>

std::sig_atomic_t sig = 0;
void sig_handler(int s) {
  sig = s;
}

int main() {
  signal(SIGHUP,  sig_handler);
  signal(SIGINT,  sig_handler);
  signal(SIGQUIT, sig_handler);
  signal(SIGILL,  sig_handler);
  signal(SIGTRAP, sig_handler);
  signal(SIGABRT, sig_handler);
  
  std::mutex hardware_mut;
  PWMSparkMax drive_left(IOMap::PWM_DRIVE_LEFT);
  PWMSparkMax drive_right(IOMap::PWM_DRIVE_RIGHT);
  /* TalonFX pivot_left(IOMap::CAN_PIVOT_LEFT); */
  /* TalonFX pivot_right(IOMap::CAN_PIVOT_RIGHT); */
  /* TalonFX shooter_rot(IOMap::CAN_SHOOTER_ROT); */
  DutyCycleEncoder shooter_rot_enc(IOMap::DI_SHOOTER_ROT);
  Solenoid fill_valve(IOMap::DO_FILL_VALVE);
  Solenoid shoot_valve(IOMap::DO_SHOOT_VALVE);
  
  IPCStatusHandler status_handler {
    &hardware_mut,
    /* &pivot_left, &pivot_right, */
    /* &shooter_rot, */
    nullptr, nullptr, nullptr,
    &shooter_rot_enc
  };
  IPCControlHandler control_handler {
    &hardware_mut,
    &drive_left, &drive_right,
    /* &pivot_left, &pivot_right, */
    /* &shooter_rot, */
    nullptr, nullptr, nullptr,
    &fill_valve, &shoot_valve
  };
  
  while (!sig) {
    // Receive control messages...
    control_handler.handle_control_msg();
  }
  
  return 0;
}
