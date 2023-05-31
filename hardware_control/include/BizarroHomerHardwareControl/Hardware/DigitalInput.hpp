#pragma once

#include <string>
#include <filesystem>

class DigitalInput {
public:
  DigitalInput(int channel);
  ~DigitalInput();
  
  bool get();
  
private:
  int channel;
  
  std::filesystem::path channel_dir, dir_path, val_path;
};
