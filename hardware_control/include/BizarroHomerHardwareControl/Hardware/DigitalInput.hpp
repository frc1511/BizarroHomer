#pragma once

class DigitalInput {
public:
  DigitalInput(int channel);
  ~DigitalInput();
  
  bool get();
  
private:
};
