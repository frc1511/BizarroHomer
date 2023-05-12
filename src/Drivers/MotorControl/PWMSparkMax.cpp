#include <BizarroHomer/Drivers/MotorControl/PWMSparkMax.hpp>

// Microseconds (μs).
#define SPARKMAX_FORWARD 2000
#define SPARKMAX_NEUTRAL 1500
#define SPARKMAX_REVERSE 1000

PWMSparkMax::PWMSparkMax(int channel)
: PWMMotorController(channel, SPARKMAX_FORWARD * 1000, SPARKMAX_NEUTRAL * 1000, SPARKMAX_REVERSE * 1000) { }
