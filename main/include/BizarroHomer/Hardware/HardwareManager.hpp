#pragma once

#include <mutex>
#include <thread>

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
  
  /**
   * @brief Sets the position of the fill valve.
   *
   * @param closed Whether the valve should be closed.
   */
  void set_fill_valve(bool closed);
  
  /**
   * @brief Sets the position of the shoot valve.
   *
   * @param closed Whether the valve should be closed.
   */
  void set_shoot_valve(bool closed);
  
  /**
   * @brief Sets the percent output of the left drive motor.
   *
   * @param percent The percent output of the motor (-1 to 1).
   */
  void set_drive_left(double percent);
  
  /**
   * @brief Sets the percent output of the right drive motor.
   *
   * @param percent The percent output of the motor (-1 to 1).
   */
  void set_drive_right(double percent);
  
  /**
   * @brief Sets the PID reference position of the left pivot motor.
   *
   * @param position Desired position (rotations) of the left pivot motor.
   */
  void set_pivot_left(double position);
  
  /**
   * @brief Sets the PID reference position of the right pivot motor.
   *
   * @param position Desired position (rotations) of the right pivot motor.
   */
  void set_pivot_right(double position);
  
  /**
   * @brief Sets the PID reference position of the shooter rotation motor.
   *
   * @param position Desired position (rotations) of the shooter rotation motor.
   */
  void set_shooter_rotation(double position);
  
  /**
   * @brief Returns whether the hardware components are enabled.
   *
   * @return Whether the hardware components are enabled.
   */
  bool get_enabled();
  
  /**
   * @brief Returns the status of the 120 PSI pressure switch.
   *
   * @return The status of the pressure switch.
   */
  bool get_pressure_switch();
  
  /**
   * @brief Returns the encoder position of the left pivot motor.
   *
   * @return The encoder position (rotations) of the left pivot motor.
   */
  double get_pivot_left_encoder();
  
  /**
   * @brief Returns the encoder position of the right pivot motor.
   *
   * @return The encoder position (rotations) of the right pivot motor.
   */
  double get_pivot_right_encoder();
  
  /**
   * @brief Returns the encoder position of the shooter rotation motor.
   *
   * @return The encoder position (rotations) of the shooter rotation motor.
   */
  double get_shooter_rotation_encoder();
  
  /**
   * @brief Returns the absolute encoder position of the shooter rotation.
   *
   * @return The absolute rotation of the shooter rotation.
   */
  double get_shooter_rotation_absolute_encoder();
  
  struct ControlData {
    unsigned enabled : 1;
    unsigned fill_valve : 1;
    unsigned shoot_valve : 1;
    unsigned pad : 5;
    // Motor percent outputs (0x00 - 0xFF).
    uint8_t drive_left;
    uint8_t drive_right;
    // Motor PID positions.
    double pivot_left;
    double pivot_right;
    double shooter_rot;
  };
  
  struct StatusData {
    unsigned enabled : 1;
    unsigned pressure_switch : 1;
    unsigned pad : 6;
    double pivot_left_enc;
    double pivot_right_enc;
    double shooter_rot_enc;
    double shooter_rot_abs_enc;
  };
  
private:
  HardwareManager();
  ~HardwareManager();
  
  std::thread control_thread,
              status_thread;
  std::mutex control_mutex,
             status_mutex;
  
  void control_thread_main();
  void status_thread_main();
  
  ControlData control_data;
  
  StatusData status_data;
  
  static HardwareManager instance;
};
