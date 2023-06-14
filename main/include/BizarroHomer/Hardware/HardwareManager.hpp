#pragma once

#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <mutex>
#include <thread>
#include <functional>
#include <vector>
#include <utility>

class HardwareManager {
public:
  static inline HardwareManager* get() {
    return &instance;
  }
  
  HardwareManager(HardwareManager const&) = delete;
  HardwareManager& operator=(HardwareManager const&) = delete;
  
  /**
   * @brief Sets whether hardware components should be enabled.
   *
   * @param enabled Whether the hardware components should be enabled.
   */
  void set_enabled(bool enabled);
  
  void reset_hardware();
  
  enum class HardwareType : uint8_t {
    DIGITAL_IN       = 0,
    DIGITAL_OUT      = 1,
    CAN_TALON_FX     = 2,
    PWM_SPARK_MAX    = 3,
    ABS_THROUGH_BORE = 4,
    CAN_PDP          = 5,
  };
  
  enum class ControlProperty : uint8_t {
    INIT     = 0,
    DIGITAL  = 1,
    PERCENT  = 2,
    POSITION = 3,
  };
  
  void send_ctrl_msg(HardwareType type, uint8_t id, ControlProperty prop, double value);
  
  enum class StatusProperty : uint8_t {
    ENCODER = 0,
    DIGITAL = 1,
    ANGLE   = 2,
    VALUE   = 3,
  };
  
  using StatusCallbackFunc = std::function<void(double value)>;
  
  void register_status_callback(HardwareType type, uint8_t id, StatusProperty prop, StatusCallbackFunc callback);
  
private:
  HardwareManager();
  ~HardwareManager();
  
  IPCSender s;
  
  std::thread status_thread;
  std::mutex control_mutex,
             status_mutex;
  
  struct StatusCallbackID {
    HardwareType type;
    uint8_t id;
    StatusProperty prop;
  };
  
  std::vector<std::pair<StatusCallbackID, StatusCallbackFunc>> status_callbacks;
  
  void status_thread_main();
  
  static HardwareManager instance;
};
