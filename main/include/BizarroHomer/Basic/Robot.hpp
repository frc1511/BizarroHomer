#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Control/Controls.hpp>
#include <vector>

class Robot {
public:
  Robot();
  ~Robot();
  
  // Called every 20ms.
  void process();
  
private:
  Drive drive;
  Controls controls { &drive };
  
  std::vector<Mechanism*> all_mechanisms {
    &drive, &controls,
  };
};
