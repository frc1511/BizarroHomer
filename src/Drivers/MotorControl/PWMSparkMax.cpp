#include <BizarroHomer/Drivers/MotorControl/PWMSparkMax.hpp>

#define SPARKMAX_MAX          (2000 * 1000)
#define SPARKMAX_MAX_DEADBAND (1525 * 1000)
#define SPARKMAX_CENTER       (1500 * 1000)
#define SPARKMAX_MIN_DEADBAND (1475 * 1000)
#define SPARKMAX_MIN          (1000 * 1000)

#define SPARKMAX_FREQ 100

PWMSparkMax::PWMSparkMax(int channel)
: PWMMotorController(channel, SPARKMAX_MAX, SPARKMAX_MAX_DEADBAND, SPARKMAX_CENTER, SPARKMAX_MIN_DEADBAND, SPARKMAX_MIN, SPARKMAX_FREQ) { }
