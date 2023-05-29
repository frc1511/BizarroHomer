#pragma once

#include <BizarroHomerHardwareControl/Hardware/MotorControl/PWMSparkMax.hpp>
#define Phoenix_No_WPI
#include <ctre/Phoenix.h>
#include <BizarroHomerHardwareControl/Hardware/Encoder/DutyCycleEncoder.hpp>
#include <BizarroHomerHardwareControl/Hardware/DigitalInput.hpp>
#include <BizarroHomerHardwareControl/Hardware/DigitalOutput.hpp>
#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <utility>

class HardwareManager {
public:
  static inline HardwareManager* get() {
    return &instance;
  }
  
  HardwareManager(HardwareManager const&) = delete;
  HardwareManager& operator=(HardwareManager const&) = delete;
  
  void send_status_msgs();
  void process_hardware();
  
  void terminate();
  
private:
  HardwareManager();
  ~HardwareManager();
  
  void stop();
  
  std::map<uint8_t, std::unique_ptr<PWMSparkMax>> spark_maxes;
  std::map<uint8_t, std::unique_ptr<TalonFX>> talon_fxs;
  std::map<uint8_t, std::unique_ptr<DutyCycleEncoder>> encoders;
  std::map<uint8_t, std::unique_ptr<DigitalInput>> digital_inputs;
  
  std::map<uint8_t, std::pair<std::unique_ptr<DigitalOutput>, int>> digital_outputs;
  
  std::thread recv_thread;
  void recv_thread_main();
  
  bool enabled = false;
  std::mutex hardware_mut;
  
  bool should_term = false;
  std::mutex manager_mut;
  
  IPCSender s;
  
  static HardwareManager instance;
};
