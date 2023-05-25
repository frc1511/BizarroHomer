#include <BizarroHomerHardwareControl/IPCControlHandler.hpp>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <cerrno>

#define IPC_PATHNAME "/var/frc1511/BizarroHomer/ipc_msg_queue_key"

#define IPC_CONTROL_MSG_SIZE 27

struct IPCControlMessage {
  long mtype;
  struct Data {
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

IPCControlHandler::IPCControlHandler(std::mutex* _hardware_mut, PWMSparkMax* _drive_left, PWMSparkMax* _drive_right, TalonFX* _pivot_left, TalonFX* _pivot_right, TalonFX* _shooter_rot, SingleSolenoid* _fill_valve, SingleSolenoid* _shoot_valve)
: hardware_mut(_hardware_mut), drive_left(_drive_left), drive_right(_drive_right), pivot_left(_pivot_left), pivot_right(_pivot_right), shooter_rot(_shooter_rot), fill_valve(_fill_valve), shoot_valve(_shoot_valve) {
  stop();
  
  key_t key = ftok(IPC_PATHNAME, 'C');
  if (key < 0) {
    fmt::print("ftok() failed:\n\t{}\n", strerror(errno));
    return;
  }
  
  // Create or open the message queue.
  msqid = msgget(key, IPC_CREAT | 0666);
  if (msqid < 0) {
    fmt::print("msgget() failed:\n\t{}\n", strerror(errno));
    return;
  }
  
  is_open = true;
}

void IPCControlHandler::stop() {
  std::lock_guard<std::mutex> lk(*hardware_mut);
  // Stop motors from moving.
  drive_left->set(0.0);
  drive_right->set(0.0);
  pivot_left->Set(TalonFXControlMode::PercentOutput, 0.0);
  pivot_right->Set(TalonFXControlMode::PercentOutput, 0.0);
  shooter_rot->Set(TalonFXControlMode::PercentOutput, 0.0);
  // Close everything in Pneumatics system.
  fill_valve->set(true);
  shoot_valve->set(true);
}

void IPCControlHandler::shutdown() {
  is_shutdown = true;
}

void IPCControlHandler::handle_control_msg() {
  if (is_shutdown) return;
  
  if (!is_open) {
    stop();
    return;
  }
  
  IPCControlMessage msg;
  std::memset(&msg, 0, sizeof(IPCControlMessage));
  
  if (msgrcv(msqid, &msg, IPC_CONTROL_MSG_SIZE, 1, 0) < 0) {
    fmt::print("msgrcv() failed:\n\t{}\n", strerror(errno));
    return;
  }
  
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
  
  pivot_left->Set(TalonFXControlMode::Position, msg.data.pivot_left);
  pivot_right->Set(TalonFXControlMode::Position, msg.data.pivot_right);
  shooter_rot->Set(TalonFXControlMode::Position, msg.data.shooter_rot);
}
