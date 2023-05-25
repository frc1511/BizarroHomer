#include <BizarroHomerDualShock4LEDControl/LEDHandler.hpp>
#include <fmt/core.h>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

LEDHandler::LEDHandler()
: led_thread([&]() { this->led_thread_main(); }) { }

LEDHandler::~LEDHandler() {
  terminate();
}

void LEDHandler::set_colors(Color _col_1, Color _col_2) {
  std::lock_guard<std::mutex> lk(led_mutex);
  new_col = true;
  col_1 = _col_1;
  col_2 = _col_2;
}

void LEDHandler::set_should_update_controllers() {
  std::lock_guard<std::mutex> lk(led_mutex);
  upd_ctrl = true;
}

void LEDHandler::terminate() {
  std::lock_guard<std::mutex> lk(led_mutex);
  should_term = true;
}

void LEDHandler::update_controllers() {
  static const std::filesystem::path base_path("/sys/class/leds/");
  
  ctrl_paths.clear();
  
  for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
    std::string dir_name = entry.path().filename().string();
    if (dir_name.find("054C:09CC") != std::string::npos && dir_name.find(":red") != std::string::npos) {
      std::filesystem::path brightness_path = entry.path() / "brightness";
      if (std::filesystem::exists(brightness_path)) {
        std::string d = (base_path / dir_name.substr(0, dir_name.find(":red") + 1)).string();
        if (std::find(ctrl_paths.begin(), ctrl_paths.end(), d) == ctrl_paths.end()) {
          ctrl_paths.push_back(d);
        }
      }
    }
  }
}

void LEDHandler::led_thread_main() {
  Color c[2] = { Color { 0, 0, 0 }, Color { 0, 0, 0 } };
  bool upd = false;
  while (true) {
    {
      std::lock_guard<std::mutex> lk(led_mutex);
      if (new_col) {
        c[0] = col_1;
        c[1] = col_2;
        new_col = false;
        
      }
      upd = upd_ctrl;
      upd_ctrl = false;
      if (should_term) return;
    }

    if (upd) update_controllers();
    
    for (int i = 0; i < 2; i++) {
      std::string r_str = std::to_string(c[i].r);
      std::string g_str = std::to_string(c[i].g);
      std::string b_str = std::to_string(c[i].b);
      
      for (const auto& d : ctrl_paths) {
        int fd = open(fmt::format("{}red/brightness", d).c_str(), O_WRONLY | O_CREAT | O_TRUNC);
        write(fd, r_str.c_str(), r_str.length());
        close(fd);
        fd = open(fmt::format("{}green/brightness", d).c_str(), O_WRONLY | O_CREAT | O_TRUNC);
        write(fd, g_str.c_str(), g_str.length());
        close(fd);
        fd = open(fmt::format("{}blue/brightness", d).c_str(), O_WRONLY | O_CREAT | O_TRUNC);
        write(fd, b_str.c_str(), b_str.length());
        close(fd);
      }
      
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(0.25s);
    }
  }
}
