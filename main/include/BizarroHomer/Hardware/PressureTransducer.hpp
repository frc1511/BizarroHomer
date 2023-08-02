#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <filesystem>
#include <cstdint>

namespace thunder {

// Analig pressure transducer read via an I2C ADS1115 ADC.
class PressureTransducer {
public:
  PressureTransducer(int i2c_adapter_nr = 0, uint8_t address = 0x48);
  ~PressureTransducer();
  
  //
  // Reads the current pressure of the transducer, in PSI.
  //
  double read_pressure();
  
private:
  //
  // Reads a value from a register.
  // 8-bit register address, 16-bit register value.
  //
  uint16_t read_register(uint8_t reg);
  
  //
  // Writes a value to a register.
  // 8-bit register address, 16-bit register value.
  //
  void write_register(uint8_t reg, uint16_t value);
  
  void set_config(uint16_t value, uint16_t mask);
  
  void set_os(uint16_t value);
  void set_multiplexer(uint16_t value);
  void set_pga(uint16_t value);
  void set_mode(uint16_t value);
  void set_data_rate(uint16_t value);
  void set_comparator_mode(uint16_t value);
  void set_comparator_polarity(uint16_t value);
  void set_comparator_latching(uint16_t value);
  void set_comparator_queue(uint16_t value);
  
  std::filesystem::path m_i2c_path;
  int m_i2c_fd;
  bool m_connected = true;
  
  uint16_t m_config = 0;
};

} // namespace thunder
