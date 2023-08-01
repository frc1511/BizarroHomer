#include <BizarroHomer/Basic/SignalHandler.hpp>
#include <BizarroHomer/Basic/Robot.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <BizarroHomer/DashboardServer/DashboardServer.hpp>
#include <ctre/phoenix/unmanaged/Unmanaged.h>
#include <fmt/core.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <ctime>

int main() {
  if (geteuid() != 0) {
    fmt::print("Should be run as root\n");
    return 1;
  }
  
  std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  fmt::print(stdout, "Starting at {}\n", std::ctime(&start_time));
  
  // Start Phoenix background tasks.
  ctre::phoenix::unmanaged::Unmanaged::LoadPhoenix();
  // Start Phoenix diagnostic server immediately.
  ctre::phoenix::unmanaged::Unmanaged::SetPhoenixDiagnosticsStartTime(0);
  
  DashboardServer dashboard_server;
  Robot robot { &dashboard_server };
  
  while (!SignalHandler::get()->should_exit()) {
    using namespace std::literals::chrono_literals;
    
    // Feed CTRE enabled status.
    ctre::phoenix::unmanaged::Unmanaged::FeedEnable(100);
    
    auto start_time_point = std::chrono::high_resolution_clock::now();
    
    // Robot periodic loop.
    robot.process();
    
    // Dashboard server periodic loop.
    dashboard_server.process();
    
    auto end_time_point = std::chrono::high_resolution_clock::now();
    
    // Time left in 20ms loop period.
    auto loop_duration = end_time_point - start_time_point;
    if (loop_duration < 20ms) {
      std::this_thread::sleep_for(20ms - loop_duration);
    }
    else if (loop_duration > 20ms) {
      fmt::print("Loop time of 20ms overrun\n");
    }
  }
  
  return 0;
}
