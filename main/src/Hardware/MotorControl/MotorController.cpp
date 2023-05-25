#include <BizarroHomer/Hardware/MotorControl/MotorController.hpp>
#include <BizarroHomer/Hardware/MotorControl/MotorControllerManager.hpp>

MotorController::MotorController(std::string _key) : key(_key) {
  MotorControllerManager::get()->register_motor(key, this);
}

MotorController::~MotorController() {
  MotorControllerManager::get()->unregister_motor(key);
}
