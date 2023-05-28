#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>

#define IPC_PATHNAME "/var/frc1511/BizarroHomer/ipc_msg_queue_key"

#define IPC_MSG_SIZE 7

struct IPCMessage {
  long mtype;
  char mtext[IPC_MSG_SIZE]; // r1, g1, b1, r2, g2, b2, update_controllers
};

DualShock4_LEDManager::DualShock4_LEDManager()
: s(IPC_PATHNAME, 'D') {
  send_msg(true);
}

DualShock4_LEDManager::~DualShock4_LEDManager() { }

void DualShock4_LEDManager::set_color(Color col) {
  col_1 = col;
  col_2 = col;
  send_msg(false);
}

void DualShock4_LEDManager::set_alternating_colors(Color _col_1, Color _col_2) {
  col_1 = _col_1;
  col_2 = _col_2;
  send_msg(false);
}

void DualShock4_LEDManager::update_controllers() {
  send_msg(true);
}

void DualShock4_LEDManager::send_msg(bool update_ctrl) {
  IPCMessage msg;
  msg.mtext[0] = col_1.get_r();
  msg.mtext[1] = col_1.get_g();
  msg.mtext[2] = col_1.get_b();
  msg.mtext[3] = col_2.get_r();
  msg.mtext[4] = col_2.get_g();
  msg.mtext[5] = col_2.get_b();
  msg.mtext[6] = static_cast<char>(update_ctrl);
  
  msg.mtype = 1;
  s.send_msg(msg);
}

DualShock4_LEDManager DualShock4_LEDManager::instance;
