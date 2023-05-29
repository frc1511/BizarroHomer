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
  
  fmt::print("Signal {} caught\n", sig);
  
  HardwareManager::get()->set_enabled(false);
  
  DualShock4_LEDManager::get()->set_alternating_colors(Color(0, 0, 255), Color(0, 0, 0));
  
}

SignalHandler SignalHandler::instance;

