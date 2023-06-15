#pragma once

#include <optional>

class DigitalOutput {
public:
  DigitalOutput(int channel, std::optional<bool> default_set = std::nullopt);
  ~DigitalOutput();
  
  void set(bool output);
  bool get();
  
private:
  int channel;
  bool output = false;
};
