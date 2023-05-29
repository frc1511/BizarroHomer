#include <BizarroHomer/Hardware/MotorControl/LazyPWMSparkMax.hpp>

LazyPWMSparkMax::LazyPWMSparkMax(int channel)
: PWMSparkMax(channel) { }

LazyPWMSparkMax::~LazyPWMSparkMax() { }

void LazyPWMSparkMax::set_percent(double percent) {
  if (percent != last_percent) {
    // Remember this control.
    last_percent = percent;
    
    // Actually set the percent.
    PWMSparkMax::set_percent(percent);
  }
  // Already been set to desired percent, so no need to send control message again.
}
