#pragma once

#include <BizarroHomer/Hardware/MotorControl/PWMMotorController.hpp>

class PWMSparkMax final : public PWMMotorController {
public:
  PWMSparkMax(int channel);
  ~PWMSparkMax() = default;
};

