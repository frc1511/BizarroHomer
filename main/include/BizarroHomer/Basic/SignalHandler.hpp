#pragma once

class SignalHandler {
public:
  static inline SignalHandler* get() {
    return &instance;
  }
  
  SignalHandler(SignalHandler const&) = delete;
  SignalHandler& operator=(SignalHandler const&) = delete;
  
  /**
   * @brief Returns whether the program should exit.
   *
   * @return Whether the program should exit.
   */
  bool should_exit();
  
  void sig_handler(int sig);
  
private:
  SignalHandler();
  ~SignalHandler();
  
  volatile bool sig_encountered = false;
  
  static SignalHandler instance;
};
