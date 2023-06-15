#include <BizarroHomerDualShock4LEDControl/LEDHandler.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <csignal>
#include <cstring>
#include <unistd.h>

struct IPCMessage {
  long mtype = 1;
  struct Data {
    uint8_t update_ctrl;
    uint8_t colors;
  } data;
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
  
  {
    LEDHandler led_handler;
    
    while (!sig) {
      if (!r.recv_msg(&msg)) break;
      
      if (msg.data.update_ctrl) {
        led_handler.set_should_update_controllers();
      }
      led_handler.set_colors(msg.data.colors);
    }
  }
  
  return 0;
}
