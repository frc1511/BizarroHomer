#include <BizarroHomer/Basic/SignalHandler.hpp>
#include <BizarroHomer/Control/GameController/GameControllerManager.hpp>
#include <BizarroHomer/Control/GameController/DualShock4.hpp>
#include <BizarroHomer/Hardware/MotorControl/MotorControllerManager.hpp>
#include <BizarroHomer/Hardware/MotorControl/PWMSparkMax.hpp>
#include <fmt/core.h>
#include <thread>
#include <chrono>

int main() {
  PWMSparkMax left_drive(1);
  PWMSparkMax right_drive(0);
  
  // Disable all motors to start.
  MotorControllerManager::get()->set_enabled(false);
  
  // Register the controllers that we are gonna use.
  GameControllerManager::get()->register_controller(0);
  GameControllerManager::InputFrame frame;
  
  DualShock4_LEDManager::get()->set_alternating_colors(Color(0, 0, 255), Color(255, 0, 0));
  
  while (!SignalHandler::get()->should_exit()) {
    
    bool has_controller = GameControllerManager::get()->get_controller_connected(0);
    
    // Enable motors only if controller is connected.
    MotorControllerManager::get()->set_enabled(has_controller);
    
    if (!has_controller) continue;
    
    // Get input from controllers.
    GameControllerManager::get()->get_input(0, &frame);
    
    double left = frame.axes[DualShock4_Axis::LEFT_Y];
    double right = frame.axes[DualShock4_Axis::RIGHT_Y];
    
    if (std::abs(left) < 0.1) {
      left = 0.0;
    }
    if (std::abs(right) < 0.1) {
      right = 0.0;
    }
    
    left_drive.set(left * 0.3);
    right_drive.set(-right * 0.3);
    
    fmt::print("{} --- {}\n", left, -right);
    
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
  
  MotorControllerManager::get()->set_enabled(false);
  
  return 0;
}
