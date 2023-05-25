#include <BizarroHomer/Hardware/Pneumatics/CTREPCM.hpp>
#include <cstdint>
#include <cstring>

#define CAN_MAN_CTRE 4
#define CAN_DEV_PNEUMATICS 9

#define STATUS_1          0x50
#define STATUS_SOL_FAULTS 0x51
#define STATUS_DEBUG      0x52

#define CONTROL_1 0x70
#define CONTROL_2 0x71
#define CONTROL_3 0x72

// milliseconds
#define SEND_PERIOD 20

CTREPCM::CTREPCM(int can_id)
: can(can_id, CAN_MAN_CTRE, CAN_DEV_PNEUMATICS) {
  
  std::memset(&control, 0, 8);
  
  enable_compressor();
}

bool CTREPCM::get_compressor() {
  PCMStatus status;
  if (!read_status(&status)) return false;
  
  return status.bits.compressor_on;
}

void CTREPCM::enable_compressor() {
  control.bits.closed_loop_enable = true;
  send_control();
}

void CTREPCM::disable_compressor() {
  control.bits.closed_loop_enable = false;
  send_control();
}

bool CTREPCM::get_pressure_switch() {
  PCMStatus status;
  if (!read_status(&status)) return false;
  
  return status.bits.pressure_switch_en;
}

double CTREPCM::get_compressor_current() {
  PCMStatus status;
  if (!read_status(&status)) return false;
  
  uint32_t result = status.bits.compressor_current_top6;
  result <<= 4;
  result |= status.bits.compressor_current_btm4;
  return result * 0.03125; // 5.5 fixed pt value in Amps
}

bool CTREPCM::get_fault(Fault fault) {
  PCMStatus status;
  if (!read_status(&status)) return false;
  
  PCMStatusFault status_fault;
  if (!read_sol_faults(&status_fault)) return false;
  
  switch (fault) {
    case Fault::COMPRESSOR_CURRENT_TOO_HIGH:
      return status.bits.fault_comp_current_too_high;
    case Fault::COMPRESSOR_SHORTED:
      return status.bits.fault_dI_too_high;
    case Fault::COMPRESSOR_NOT_CONNECTED:
      return status_fault.bits.fault_comp_no_current;
    case Fault::SOLENOID_VOLTAGE:
      return status.bits.fault_fuse_tripped;
  }
  
  return false;
}

bool CTREPCM::get_sticky_fault(Fault fault) {
  PCMStatus status;
  if (!read_status(&status)) return false;
  
  PCMStatusFault status_fault;
  if (!read_sol_faults(&status_fault)) return false;
  
  switch (fault) {
    case Fault::COMPRESSOR_CURRENT_TOO_HIGH:
      return status.bits.sticky_fault_comp_current_too_high;
    case Fault::COMPRESSOR_SHORTED:
      return status.bits.sticky_fault_dI_too_high;
    case Fault::COMPRESSOR_NOT_CONNECTED:
      return status_fault.bits.sticky_fault_comp_no_current;
    case Fault::SOLENOID_VOLTAGE:
      return status.bits.sticky_fault_fuse_tripped;
  }
  
  return false;
}

void CTREPCM::clear_sticky_faults() {
  uint8_t control_data[] = {0, 0, 0, 0x80};
  can.write_packet(control_data, sizeof(control_data), CONTROL_2);
}

int32_t CTREPCM::get_solenoids() {
  PCMStatus status;
  if (!read_status(&status)) return false;
  
  return status.bits.solenoid_bits & 0xFF;
}

void CTREPCM::set_solenoids(int32_t mask, int32_t values) {
  PCMStatus status;
  if (!read_status(&status)) return;
  
  uint8_t small_mask = mask & 0xFF;
  uint8_t small_values = (values & 0xFF) & small_mask;  // Enforce only masked values are set
  uint8_t invert_mask = ~small_mask;
  
  uint8_t existing_value = invert_mask & control.bits.solenoid_bits;
  control.bits.solenoid_bits = existing_value | small_values;
  
  send_control();
}

bool CTREPCM::read_status(PCMStatus* status) {
  if (!status) return false;
  
  CAN::Data data;
  if (!can.read_packet_latest(STATUS_1, &data)) return false;
  
  std::memcpy(data.data, status->data, 8);
  
  return true;
}

bool CTREPCM::read_sol_faults(PCMStatusFault* fault) {
  if (!fault) return false;
  
  CAN::Data data;
  if (!can.read_packet_latest(STATUS_SOL_FAULTS, &data)) return false;
  
  std::memcpy(data.data, fault->data, 8);
  
  return true;
}

void CTREPCM::send_control() {
  can.write_packet_repeating(control.data, 8, CONTROL_1, SEND_PERIOD);
}

