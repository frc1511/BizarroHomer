#pragma once

#include <optional>

class Digital_Output {
public:
  Digital_Output(int channel, std::optional<bool> default_set = std::nullopt);
  ~Digital_Output();
  
  void set(bool output);
  bool get();
  
private:
  int channel;
  bool output = false;
};
