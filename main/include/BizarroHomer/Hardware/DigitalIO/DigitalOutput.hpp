#pragma once

#include <optional>

class DigitalOutput {
public:
  DigitalOutput(int channel, std::optional<bool> default_set = std::nullopt);
  ~DigitalOutput();
  
  virtual void set(bool output);
  
private:
  int channel;
};
