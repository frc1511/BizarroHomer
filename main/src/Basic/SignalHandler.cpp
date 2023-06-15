#include <BizarroHomer/Basic/SignalHandler.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <signal.h>
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
  HardwareManager::get()->set_enabled(false);
  
  // Set controller LED colors to signify program ended.
  DualShock4_LEDManager::get()->set_colors(DualShock4_LEDManager::ColorBits::BLUE | DualShock4_LEDManager::ColorBits::OFF);
}

SignalHandler SignalHandler::instance;

