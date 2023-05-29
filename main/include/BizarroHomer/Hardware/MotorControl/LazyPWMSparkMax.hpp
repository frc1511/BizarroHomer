#pragma once

#include <BizarroHomer/Hardware/MotorControl/PWMSparkMax.hpp>

class LazyPWMSparkMax : public PWMSparkMax {
public:
  LazyPWMSparkMax(int channel);
  ~LazyPWMSparkMax();
  
  void set_percent(double percent) override;
  
private:
  double last_percent = 0.0;
  double last_position = 0.0;
};
