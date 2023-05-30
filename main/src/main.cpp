#include <BizarroHomer/Basic/SignalHandler.hpp>
#include <BizarroHomer/Basic/Robot.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <fmt/core.h>
#include <thread>
#include <chrono>

int main() {
  // Reset and enable all hardware.
  HardwareManager::get()->reset_hardware();
  HardwareManager::get()->set_enabled(true);
  
  Robot robot;
  
  while (!SignalHandler::get()->should_exit()) {
    using namespace std::literals::chrono_literals;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Robot periodic loop.
    robot.process();
    
    auto end = std::chrono::high_resolution_clock::now();
    
    // Time left in 20ms loop period.
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    if (dur < 20ms) {
      std::this_thread::sleep_for(20ms - dur);
    }
    else if (dur > 20ms) {
      fmt::print("Loop time of 20ms overrun\n");
    }
  }
  
  return 0;
}
