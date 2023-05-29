#pragma once

class DigitalOutput {
public:
  DigitalOutput(int channel);
  ~DigitalOutput();
  
  void set(bool output);
  
private:
};
