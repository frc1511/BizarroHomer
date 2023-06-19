#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>

struct IPCLEDMessage {
  long mtype = 1;
  struct Data {
    uint8_t update_ctrl;
    uint8_t colors;
  } data;
};

DualShock4_LEDManager::DualShock4_LEDManager()
: s(IPC_PATHNAME, 'D') {
  std::lock_guard<std::mutex> lk(led_mutex);
  send_msg(true);
}

DualShock4_LEDManager::~DualShock4_LEDManager() { }

void DualShock4_LEDManager::set_colors(uint8_t _colors) {
  std::lock_guard<std::mutex> lk(led_mutex);
  colors = _colors;
  send_msg(false);
}

void DualShock4_LEDManager::update_controllers() {
  std::lock_guard<std::mutex> lk(led_mutex);
  send_msg(true);
}

void DualShock4_LEDManager::send_msg(bool update_ctrl) {
  IPCLEDMessage msg;
  msg.data.update_ctrl = static_cast<uint8_t>(update_ctrl);
  msg.data.colors = colors;
  fmt::print("sending...\n");
  s.send_msg(msg);
  fmt::print("sent\n");
}

DualShock4_LEDManager DualShock4_LEDManager::instance;
