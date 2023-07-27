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
  
  struct IPCControlMessage {
    long mtype = 1;
    struct Data {
      uint8_t control_type;
      uint8_t hardware_type;
      uint8_t hardware_id;
      uint8_t hardware_prop;
      double value;
    } data;
  };
  
  enum ControlMessageType {
    MSG_CONTROL = 1,
    MSG_ENABLE  = 2,
    MSG_DISABLE = 3,
    MSG_RESET   = 4,
  };
  
  enum HardwareType {
    HTYPE_DIG_IN           = 0,
    HTYPE_DIG_OUT          = 1,
    HTYPE_CAN_TALON_FX     = 2,
    HTYPE_PWM_SPARK_MAX    = 3,
    HTYPE_ABS_THROUGH_BORE = 4,
    HTYPE_CAN_PDP          = 5,
  };
  
  enum ControlProperty {
    CPROP_INIT  = 0,
    CPROP_DIG   = 1,
    CPROP_PCT   = 2,
    CPROP_POS   = 3,
    CPROP_MUSIC = 4,
  };
  
  void handle_control_msg(IPCControlMessage& msg);
  void init_hardware(HardwareType type, uint8_t id, double value);
  void start_music(double value);
  void control_hardware(ControlProperty prop, HardwareType type, uint8_t id, double value);
  
  void stop();
  
  std::map<uint8_t, std::unique_ptr<PWMSparkMax>> spark_maxes;
  std::map<uint8_t, std::unique_ptr<TalonFX>> talon_fxs;
  std::map<uint8_t, std::unique_ptr<DutyCycleThroughBore>> through_bores;
  std::map<uint8_t, std::unique_ptr<DigitalInput>> digital_inputs;
  std::map<uint8_t, std::pair<std::unique_ptr<DigitalOutput>, int>> digital_outputs;
  int pdp_can_id = -1;
  
  Orchestra orchestra_primary;
  Orchestra orchestra_secondary;
  
  IPCSender s;
  
  std::thread recv_thread;
  void recv_thread_main();
  
  bool enabled = false;
  std::mutex hardware_mut;
  
  bool should_term = false;
  std::mutex manager_mut;

  bool playing_music = false;
  uint8_t which_song = 0;
  
  static HardwareManager instance;
};
