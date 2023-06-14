#pragma once

#include <string>
#include <filesystem>
#include <thread>
#include <mutex>

class DutyCycleThroughBore {
public:
  DutyCycleThroughBore(int channel);
  ~DutyCycleThroughBore();
  
  /**
   * @brief Returns the angle in degrees of the encoder (0-360).
   *
   * @return The angle of the encoder.
   */
  double get_angle();
  
private:
  bool read_channel();
  
  std::thread read_thread;
  void read_thread_main();
  
  bool should_term = false;
  
  std::mutex read_mut;
  
  double angle = -1;
  
  int channel;
  
  std::filesystem::path channel_dir;
  std::filesystem::path dir_path, value_path;
};
