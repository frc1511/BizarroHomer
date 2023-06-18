#include <BizarroHomerHardwareControl/HardwareManager.hpp>
#include <fmt/core.h>
#include <csignal>
#include <csignal>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <ctime>

std::sig_atomic_t sig = 0;
void sig_handler(int s) {
  sig = s;
}

int main() {
  if (geteuid() != 0) {
    fmt::print("Must be run as root\n");
    return 1;
  }
  
  std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  fmt::print(stdout, "Hardware Control Service starting at {}\n", std::ctime(&start_time));
  fmt::print(stderr, "Hardware Control Service starting at {}\n", std::ctime(&start_time));
  
  signal(SIGHUP,  sig_handler);
  signal(SIGINT,  sig_handler);
  signal(SIGQUIT, sig_handler);
  signal(SIGILL,  sig_handler);
  signal(SIGTRAP, sig_handler);
  signal(SIGABRT, sig_handler);
  
  while (!sig) {
    HardwareManager::get()->send_status_msgs();
    HardwareManager::get()->process_hardware();
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
  
  return 0;
}
