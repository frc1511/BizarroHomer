#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>

namespace thunder {

class PDP : public HardwareComponent {
public:
  PDP(int can_id = 0);
  ~PDP();
  
  double get_voltage();
  
private:
  int m_can_id;
};

} // namespace thunder
