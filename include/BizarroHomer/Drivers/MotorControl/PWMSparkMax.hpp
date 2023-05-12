#pragma once

#include <BizarroHomer/Drivers/MotorControl/PWMMotorController.hpp>

class PWMSparkMax final : public PWMMotorController {
public:
  PWMSparkMax(int channel);
  ~PWMSparkMax() = default;
};

