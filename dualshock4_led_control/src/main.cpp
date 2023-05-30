#include <BizarroHomerDualShock4LEDControl/LEDHandler.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <csignal>
#include <cstring>
#include <unistd.h>

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
  if (geteuid() != 0) {
    fmt::print("Must be run as root\n");
    return 1;
  }
  
  signal(SIGHUP,  sig_handler);
  signal(SIGINT,  sig_handler);
  signal(SIGQUIT, sig_handler);
  signal(SIGILL,  sig_handler);
  signal(SIGTRAP, sig_handler);
  signal(SIGABRT, sig_handler);
  
  IPCReceiver r(IPC_PATHNAME, 'D');
  
  IPCMessage msg;
  std::memset(&msg, 0, sizeof(IPCMessage));
  
  {
    LEDHandler led_handler;
    
    while (!sig) {
      if (!r.recv_msg(&msg)) break;
      
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
