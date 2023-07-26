#pragma once

#include <BizarroHomerHardwareControl/Hardware/MotorControl/PWMSparkMax.hpp>
#define Phoenix_No_WPI
#include <ctre/Phoenix.h>
#include <BizarroHomerHardwareControl/Hardware/Encoder/DutyCycleThroughBore.hpp>
#include <BizarroHomerHardwareControl/Hardware/DigitalIO/DigitalInput.hpp>
#include <BizarroHomerHardwareControl/Hardware/DigitalIO/DigitalOutput.hpp>
#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <ctre/phoenix/music/Orchestra.h>
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
  std::map<uint8_t, std::unique_ptr<DutyCycleThroughBore>> through_bores;
  std::map<uint8_t, std::unique_ptr<DigitalInput>> digital_inputs;
  
  std::map<uint8_t, std::pair<std::unique_ptr<DigitalOutput>, int>> digital_outputs;
  
  Orchestra orchestra1;
  Orchestra orchestra2;
  
  IPCSender s;
  
  std::thread recv_thread;
  void recv_thread_main();
  
  bool enabled = false;
  std::mutex hardware_mut;
  
  bool should_term = false;
  std::mutex manager_mut;

  bool playing_music;
  
  static HardwareManager instance;
};
