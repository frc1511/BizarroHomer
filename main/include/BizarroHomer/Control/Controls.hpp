#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>

class Drive;

class Controls : public Mechanism {
public:
  Controls(Drive* drive);
  ~Controls();
  
  void process() override;
  
private:
  Drive* drive;
};
