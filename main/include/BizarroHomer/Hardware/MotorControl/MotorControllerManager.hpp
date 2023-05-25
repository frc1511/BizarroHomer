#pragma once

#include <BizarroHomer/Hardware/MotorControl/MotorController.hpp>
#include <map>
#include <string>

class MotorControllerManager {
public:
  static inline MotorControllerManager* get() {
    return &instance;
  }
  
  MotorControllerManager(MotorControllerManager const&) = delete;
  MotorControllerManager& operator=(MotorControllerManager const&) = delete;
  
  /**
   * @brief Registers a motor controller with the motor manager.
   *
   * @param key A key to identify the registered motor controller.
   * @param mc The motor controller to register.
   */
  void register_motor(std::string key, MotorController* mc);
  
  /**
   * @brief Unregisters a motor controller with the motor manager.
   *
   * @param key The key of the registered motor controller to unregister.
   */
  void unregister_motor(std::string key);
  
  /**
   * @brief Sets whether the registered motor controllers are enabled.
   *
   * @param enabled Whether the registered motor controllers are enabled.
   */
  void set_enabled(bool enabled);
  
  /**
   * @brief Returns whether the registered motor controllers are enabled.
   *
   * @return Whether the registered motor controllers are enabled.
   */
  constexpr bool get_enabled() const { return enabled; }
  
private:
  MotorControllerManager();
  ~MotorControllerManager();
  
  // Registered motor controllers.
  std::map<std::string, MotorController*> reg_mcs;
  
  bool enabled = false;
  
  static MotorControllerManager instance;
};
