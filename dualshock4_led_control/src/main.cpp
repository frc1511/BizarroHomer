#include <BizarroHomerDualShock4LEDControl/LEDHandler.hpp>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <csignal>
#include <cstring>

#define IPC_PATHNAME "/var/frc1511/BizarroHomer/ipc_msg_queue_key"

#define IPC_MSG_SIZE 7

struct IPCMessage {
  long mtype;
  char mtext[IPC_MSG_SIZE]; // r1, g1, b1, r2, g2, b2, update_controllers
};

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
  
  key_t key = ftok(IPC_PATHNAME, 'D');
  if (key < 0) {
    fmt::print("ftok() failed:\n\t{}\n", strerror(errno));
    return 1;
  }
  
  // Create or open the message queue.
  int msqid = msgget(key, IPC_CREAT | 0666);
  if (msqid < 0) {
    fmt::print("msgget() failed:\n\t{}\n", strerror(errno));
    return 1;
  }
  
  IPCMessage msg;
  std::memset(&msg, 0, sizeof(IPCMessage));
  
  {
    LEDHandler led_handler;
  
    while (!sig) {
      if (msgrcv(msqid, &msg, IPC_MSG_SIZE, 1, 0) < 0) {
        fmt::print("msgrcv() failed:\n\t{}\n", strerror(errno));
        return 1;
      }
      if (msg.mtext[6]) {
        led_handler.set_should_update_controllers();
      }
      led_handler.set_colors(
          Color { msg.mtext[0], msg.mtext[1], msg.mtext[2] },
          Color { msg.mtext[3], msg.mtext[4], msg.mtext[5] }
      );
    }
  }
  
  return 0;
}
