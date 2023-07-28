#include <BizarroHomer/Hardware/PDP.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <ctre/phoenix/cci/PDP_CCI.h>

thunder::PDP::PDP(int can_id)
: m_can_id(can_id) {
  
  HardwareManager::get()->register_hardware(this);
}

thunder::PDP::~PDP() = default;

double thunder::PDP::get_voltage() {
  double voltage = -1;
  double* currents = {};
  int filled = 0;
  
  int err = (int)c_PDP_GetValues(m_can_id, &voltage, currents, 0, &filled);
  if (err == 0) {
    return voltage;
  }
  
  return -1.0;
}
