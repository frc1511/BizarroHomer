#include <BizarroHomerHardwareControl/IPCStatusHandler.hpp>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <cerrno>
#include <chrono>

#define IPC_PATHNAME "/var/frc1511/BizarroHomer/ipc_msg_queue_key"

#define IPC_STATUS_MSG_SIZE 33

struct IPCStatusMessage {
  long mtype;
  struct Data {
    unsigned enabled : 1;
    unsigned pad : 7;
    double pivot_left_enc;
    double pivot_right_enc;
    double shooter_rot_enc;
    double shooter_rot_abs_enc;
  } data;
};

IPCStatusHandler::IPCStatusHandler(std::mutex* _hardware_mut, TalonFX* _pivot_left, TalonFX* _pivot_right, TalonFX* _shooter_rot, DutyCycleEncoder* _shooter_rot_enc)
: hardware_mut(_hardware_mut), pivot_left(_pivot_left), pivot_right(_pivot_right), shooter_rot(_shooter_rot), shooter_rot_enc(_shooter_rot_enc),
  status_thread([&]() { this->status_thread_main(); }) {

}

IPCStatusHandler::~IPCStatusHandler() {
  terminate();
  status_thread.join();
}

void IPCStatusHandler::terminate() {
  std::lock_guard<std::mutex> lk(status_mut);
  should_term = true;
}
  
void IPCStatusHandler::status_thread_main() {
  key_t key = ftok(IPC_PATHNAME, 'S');
  if (key < 0) {
    fmt::print("ftok() failed:\n\t{}\n", strerror(errno));
    return;
  }
  
  // Create or open the message queue.
  int msqid = msgget(key, IPC_CREAT | 0666);
  if (msqid < 0) {
    fmt::print("msgget() failed:\n\t{}\n", strerror(errno));
    return;
  }

  IPCStatusMessage status;
  while (true) {
    {
      std::lock_guard<std::mutex> lk(status_mut);
      if (should_term) return;
    }
    
    {
      std::lock_guard<std::mutex> lk(*hardware_mut);
      status.mtype = 1;
      status.data.enabled = 1;
      status.data.pivot_left_enc = pivot_left->GetSelectedSensorPosition(0);
      status.data.pivot_right_enc = pivot_right->GetSelectedSensorPosition(0);
      status.data.shooter_rot_enc = shooter_rot->GetSelectedSensorPosition(0);
      status.data.shooter_rot_abs_enc = shooter_rot_enc->get();
    }
    
    // Send the status message.
    if (msgrcv(msqid, &status, IPC_STATUS_MSG_SIZE, 1, 0) < 0) {
      fmt::print("msgrcv() failed:\n\t{}\n", strerror(errno));
      return;
    }
    
    // Send status messages every 15ms, because why not?
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(15ms);
  }
}
