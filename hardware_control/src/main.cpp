#include <BizarroHomerHardwareControl/HardwareManager.hpp>
#include <fmt/core.h>
#include <csignal>
#include <csignal>
#include <chrono>
#include <thread>

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
  
  while (!sig) {
    HardwareManager::get()->send_status_msgs();
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
  
  return 0;
}
