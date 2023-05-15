#include <BizarroHomer/Control/GameController/PS4Controller.hpp>
#include <cstring>
#include <chrono>

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/hidraw.h>
#include <fmt/core.h>

PS4Controller::PS4Controller(int id)
: ctrl_thread([&]() { this->ctrl_thread_start(id); }) { }

bool PS4Controller::get_button(Button button) {
  std::lock_guard<std::mutex> lk(input_mutex);
  
  return buttons & static_cast<uint16_t>(button);
}

double PS4Controller::get_axis(Axis axis) {
  std::lock_guard<std::mutex> lk(input_mutex);
  
  uint8_t byte_val = axes[static_cast<int>(axis)];
  
  if (axis == Axis::LEFT_TRIGGER || axis == Axis::RIGHT_TRIGGER) {
    return static_cast<double>(byte_val) / 0xFF;
  }
  
  return static_cast<double>(byte_val - 0x80) / 0x80;
}

void PS4Controller::ctrl_thread_start(int id) {
  using namespace std::literals::chrono_literals;
  
  int fd;
  struct input_event ev;
  
  std::string filename = fmt::format("/dev/input/event{}", id);
  
OPEN_FILE:
  fd = open(filename.c_str(), O_RDWR);
  if (fd < 0) {
    fmt::print("Failed to open {} for PS4Controller input\n", filename);
    
    {
      std::lock_guard<std::mutex> lk(input_mutex);
      buttons = 0;
      std::memset(axes, 0, 6);
    }
    
    std::this_thread::sleep_for(2s);
    goto OPEN_FILE;
  }
  
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  
  while (true) {
    // Read events from the controller.
    ssize_t bytes_read = read(fd, &ev, sizeof(struct input_event)) == sizeof(struct input_event);
    if (bytes_read > 0) {
      
      // Button events.
      if (ev.type == EV_KEY) {
        if (ev.code >= 0x130 && ev.code <= 0x13D) {
          std::lock_guard<std::mutex> lk(input_mutex);
          
          int position = ev.code - 0x130;
          
          // Un-set the bit.
          buttons &= ~(1 << position);
          // Set the bit if pressed.
          if (ev.value) buttons |= (1 << position);
        }
      }
      
      // Joystick events.
      if (ev.type == EV_ABS) {
        if (ev.code >= 0 && ev.code <= 5) {
          std::lock_guard<std::mutex> lk(input_mutex);
          
          axes[ev.code] = ev.value;
        }
      }
    }
    else if (bytes_read < 0) {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        close(fd);
        goto OPEN_FILE;
      }
    }
    else {
      struct stat buffer;
      if (stat(filename.c_str(), &buffer)) {
        close(fd);
        goto OPEN_FILE;
      }
    }
  }
}
