#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Shooter/Shooter.hpp>
#include <vector>

class Robot {
public:
  Robot();
  ~Robot();
  
  // Called every 20ms.
  void process();
  
private:
  Drive drive;
  Shooter shooter;
  Controls controls { &drive, &shooter };
  
  std::vector<Mechanism*> all_mechanisms {
    &drive, &controls, &shooter
  };
};
