#include <BizarroHomerHardwareControl/ControlHandler.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>

struct IPCControlMessage {
  long mtype;
  struct Data {
    uint64_t timestamp;
    unsigned enabled : 1;
    unsigned fill_valve : 1;
    unsigned shoot_valve : 1;
    unsigned pad : 5;
    // Motor percent outputs (0x00 - 0xFF).
    uint8_t drive_left;
    uint8_t drive_right;
    // Motor PID positions.
    double pivot_left;
    double pivot_right;
    double shooter_rot;
  } data;
};

ControlHandler::ControlHandler(std::mutex* _hardware_mut, PWMSparkMax* _drive_left, PWMSparkMax* _drive_right, TalonFX* _pivot_left, TalonFX* _pivot_right, TalonFX* _shooter_rot, Solenoid* _fill_valve, Solenoid* _shoot_valve)
: hardware_mut(_hardware_mut), drive_left(_drive_left), drive_right(_drive_right), pivot_left(_pivot_left), pivot_right(_pivot_right), shooter_rot(_shooter_rot), fill_valve(_fill_valve), shoot_valve(_shoot_valve), r(IPC_PATHNAME, 'C') {
  stop();
}

void ControlHandler::stop() {
  std::lock_guard<std::mutex> lk(*hardware_mut);
  // Stop motors from moving.
  drive_left->set(0.0);
  drive_right->set(0.0);
  /* pivot_left->Set(TalonFXControlMode::PercentOutput, 0.0); */
  /* pivot_right->Set(TalonFXControlMode::PercentOutput, 0.0); */
  /* shooter_rot->Set(TalonFXControlMode::PercentOutput, 0.0); */
  // Close everything in Pneumatics system.
  fill_valve->set(true);
  shoot_valve->set(true);
}

void ControlHandler::shutdown() {
  is_shutdown = true;
}

void ControlHandler::handle_control_msg() {
  if (is_shutdown) return;
  
  IPCControlMessage msg;
  std::memset(&msg, 0, sizeof(IPCControlMessage));
  
  if (!r.recv_msg(&msg)) {
    stop();
    return;
  }
  
  fmt::print("Received control message\n");
  
  // Enabled.
  if (!msg.data.enabled) {
    stop();
    return;
  }
  
  std::lock_guard<std::mutex> lk(*hardware_mut);
  
  fill_valve->set(msg.data.fill_valve);
  shoot_valve->set(msg.data.shoot_valve);
  
  auto to_pct = [](uint8_t byte) -> double { 
    return static_cast<double>(byte) / 0x80 - 1.0;
  };
  drive_left->set(to_pct(msg.data.drive_left));
  drive_right->set(to_pct(msg.data.drive_right));
  
  /* pivot_left->Set(TalonFXControlMode::Position, msg.data.pivot_left); */
  /* pivot_right->Set(TalonFXControlMode::Position, msg.data.pivot_right); */
  /* shooter_rot->Set(TalonFXControlMode::Position, msg.data.shooter_rot); */
}
