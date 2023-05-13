#pragma once

#include <BizarroHomer/Hardware/CAN.hpp>
#include <cstdint>

class CTREPCM {
public:
  enum {
    SOLENOID_CHANNELS = 8,
  };
  
  CTREPCM(int can_id);
  
  /**
   * @brief Returns whether the Compressor is on.
   *
   * @return Whether the Compressor is on.
   */
  bool get_compressor();
  
  void enable_compressor();
  void disable_compressor();
  
  /**
   * @brief Returns the status of the Presure Switch.
   *
   * @return The status of the Pressure Switch.
   */
  bool get_pressure_switch();
  
  double get_compressor_current();
  
  enum Fault {
    COMPRESSOR_CURRENT_TOO_HIGH,
    COMPRESSOR_SHORTED,
    COMPRESSOR_NOT_CONNECTED,
    SOLENOID_VOLTAGE,
  };
  
  /**
   * @brief Returns whether a fault is currently occurring.
   *
   * @param fault The fault to check.
   * @return Whether the fault is current occurring.
   */
  bool get_fault(Fault fault);
  
  /**
   * @brief Returns whether a fault has occurred since sticky faults were last
   *        cleared.
   *
   * @param fault The fault to check.
   * @return Whether the fault has occurred since sticky faults were last
   *         cleared.
   */
  bool get_sticky_fault(Fault fault);
  
  /**
   * @brief Clears all sticky faults on the device.
   */
  void clear_sticky_faults();
  
  int32_t get_solenoids();
  void set_solenoids(int32_t mask, int32_t values);
  
  /* void fire_one_shot(int32_t index); */
  /* void set_one_shot_duration(int32_t index, int32_t dur); */
  
private:
  CAN can;
  
  // Status
  
  union PCMStatus {
    uint8_t data[8];
    struct Bits {
      // Byte 0
      unsigned solenoid_bits : 8;
      // Byte 1
      unsigned compressor_on : 1;
      unsigned sticky_fault_fuse_tripped : 1;
      unsigned sticky_fault_comp_current_too_high : 1;
      unsigned fault_fuse_tripped : 1;
      unsigned fault_comp_current_too_high : 1;
      unsigned fault_hardware_failure : 1;
      unsigned is_closeloop_enabled : 1;
      unsigned pressure_switch_en : 1;
      // Byte 2
      unsigned batt_voltage : 8;
      // Byte 3
      unsigned solenoid_voltage_top8 : 8;
      // Byte 4
      unsigned compressor_current_top6 : 6;
      unsigned solenoid_voltage_btm2 : 2;
      // Byte 5
      unsigned sticky_fault_dI_too_high : 1;
      unsigned fault_dI_too_high : 1;
      unsigned module_enabled : 1;
      unsigned closed_loop_output : 1;
      unsigned compressor_current_btm4 : 4;
      // Byte 6
      unsigned token_seed_top8 : 8;
      // Byte 7
      unsigned token_seed_btm8 : 8;
    } bits;
  };
  
  union PCMStatusFault {
    uint8_t data[8];
    struct Bits {
      // Byte 0
      unsigned solenoid_disabled_list : 8;
      // Byte 1
      unsigned reserved_bit0 : 1;
      unsigned reserved_bit1 : 1;
      unsigned reserved_bit2 : 1;
      unsigned reserved_bit3 : 1;
      unsigned sticky_fault_comp_no_current : 1;
      unsigned fault_comp_no_current : 1;
      unsigned sticky_fault_solenoid_jumper : 1;
      unsigned fault_solenoid_jumper : 1;
    } bits;
  };
  
  bool read_status(PCMStatus* status);
  bool read_sol_faults(PCMStatusFault* fault);
  
  // Control
  
  union PCMControl {
    uint8_t data[8];
    struct Bits {
      // Byte 0
      unsigned token_top8 : 8;
      // Byte 1
      unsigned token_btm8 : 8;
      // Byte 2
      unsigned solenoid_bits : 8;
      // Byte 3
      unsigned reserved : 4;
      unsigned close_loop_output : 1;
      unsigned compressor_on : 1;
      unsigned closed_loop_enable : 1;
      unsigned clear_sticky_faults : 1;
      // Byte 4
      unsigned one_shot_field_h8 : 8;
      // Byte 5
      unsigned one_shot_field_l8 : 8;
    } bits;
  };
  
  struct PCMControlSetOneShotDur {
    uint8_t sol_10ms_per_unit[8];
  };
  
  PCMControl control;

  void send_control();
  
  // Debug
  
  union PCMDebug {
    uint8_t data[8];
    struct Bits {
      unsigned tok_fails_top8 : 8;
      unsigned tok_fails_btm8 : 8;
      unsigned last_failed_tok_top8 : 8;
      unsigned last_failed_tok_btm8 : 8;
      unsigned tok_success_top8 : 8;
      unsigned tok_success_btm8 : 8;
    } bits;
  };
};
