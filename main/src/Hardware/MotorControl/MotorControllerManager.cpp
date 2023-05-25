#include <BizarroHomer/Hardware/MotorControl/MotorControllerManager.hpp>

MotorControllerManager::MotorControllerManager() { }

MotorControllerManager::~MotorControllerManager() { }

void MotorControllerManager::register_motor(std::string key, MotorController* mc) {
  reg_mcs[key] = mc;
}

void MotorControllerManager::unregister_motor(std::string key) {
  reg_mcs.erase(key);
}

void MotorControllerManager::set_enabled(bool en) {
  enabled = en;
  for (auto [k, mc] : reg_mcs) {
    mc->set_enabled(enabled);
    // Stop the motor, just to be safe.
    mc->set(0.0);
  }
}

MotorControllerManager MotorControllerManager::instance;
