#pragma once

#include <BizarroHomer/Hardware/PWMMotorController.hpp>

namespace thunder {

class SparkMax : public PWMMotorController {
public:
  SparkMax(int channel);
 };

} // namespace thunder
