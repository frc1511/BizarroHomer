#include <BizarroHomer/Basic/SignalHandler.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <signal.h>
#include <thread>
#include <chrono>
#include <fmt/core.h>

SignalHandler::SignalHandler() {
  auto handler = [](int sig) {
    SignalHandler::get()->sig_handler(sig);
  };
  
  signal(SIGHUP,  handler); // Hangup
  signal(SIGINT,  handler); // Interrupt
  signal(SIGQUIT, handler); // Quit
  signal(SIGILL,  handler); // Illegal instruction
  signal(SIGTRAP, handler); // Trace trap
  signal(SIGABRT, handler); // Abort
}

SignalHandler::~SignalHandler() {

}

bool SignalHandler::should_exit() {
  return sig_encountered;
}

void SignalHandler::sig_handler(int sig) {
  sig_encountered = true;
  
  fmt::print("Signal {} caught. Exiting now...\n", sig);
  
  // Disable hardware.
  thunder::HardwareManager::get()->stop_all_hardware();
  
  // Set controller LED colors to signify program ended.
  DualShock4::get()->set_colors(DualShock4::ColorBits::BLUE);
  
  using namespace std::literals::chrono_literals;
  std::this_thread::sleep_for(500ms);
  
  exit(0);
}

SignalHandler SignalHandler::instance;

