#pragma once

#include <string>
#include <filesystem>

class DigitalOutput {
public:
  DigitalOutput(int channel);
  ~DigitalOutput();
  
  void set(bool output);
  
private:
  int channel;
  
  std::filesystem::path channel_dir, dir_path, val_path;
};
