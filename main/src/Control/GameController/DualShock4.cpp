#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <fmt/core.h>
#include <cstring>
#include <cerrno>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

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
  s.send_msg(msg);
}

DualShock4_LEDManager DualShock4_LEDManager::instance;

DualShock4_BatteryManager::DualShock4_BatteryManager() {
  rescan_controllers();
}

DualShock4_BatteryManager::~DualShock4_BatteryManager() = default;

double DualShock4_BatteryManager::get_percentage() {
  if (ctrl_paths.empty()) {
    return 1.0;
  }
  
  std::filesystem::path capacity_path = ctrl_paths.front() / "capacity";
  
  int fd = open(capacity_path.c_str(), O_RDONLY);
  
  if (fd == -1) {
    fmt::print(stderr, "Failed to open {}: {}\n", capacity_path.string(), strerror(errno));
    rescan_controllers();
    return 1.0;
  }
  
  char buf[16];
  ssize_t bytes_read = read(fd, buf, sizeof(buf));
  
  close(fd);
  
  // Convert to int.
  int cap = 0;
  for (int i = 0; i < bytes_read; i++) {
    if (!std::isdigit(buf[i])) {
      break;
    }
    cap *= 10;
    cap += buf[i] - '0';
  }
  
  return cap / 100.0;
}

void DualShock4_BatteryManager::rescan_controllers() {
  std::filesystem::path base_path = "/sys/class/power_supply/";
  
  ctrl_paths.clear();
  
  for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
    std::string dir_name = entry.path().filename().string();
    if (dir_name.find("sony_controller_battery") != std::string::npos) {
      ctrl_paths.push_back(entry.path());
    }
  }
}

DualShock4_BatteryManager DualShock4_BatteryManager::instance;
