#include <BizarroHomer/Hardware/PressureTransducer.hpp>
/* #include <linux/i2c.h> */
/* #include <linux/i2c-dev.h> */
#include <fmt/core.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#define I2C_SLAVE 0x0703

#define OS_SINGLE               0x8000 // Single-conversion.

#define MUX_DIFF_0_1            0x0000 // Differential P = AIN0, N = AIN1 (default)
#define MUX_DIFF_0_3            0x1000 // Differential P = AIN0, N = AIN3
#define MUX_DIFF_1_3            0x2000 // Differential P = AIN1, N = AIN3
#define MUX_DIFF_2_3            0x3000 // Differential P = AIN2, N = AIN3
#define MUX_SINGLE_0            0x4000 // Single-ended AIN0
#define MUX_SINGLE_1            0x5000 // Single-ended AIN1
#define MUX_SINGLE_2            0x6000 // Single-ended AIN2
#define MUX_SINGLE_3            0x7000 // Single-ended AIN3

#define PGA_6144                0x0000 // +/-6.144V range = Gain 2/3
#define PGA_4096                0x0200 // +/-4.096V range = Gain 1
#define PGA_2048                0x0400 // +/-2.048V range = Gain 2 (default)
#define PGA_1024                0x0600 // +/-1.024V range = Gain 4
#define PGA_512                 0x0800 // +/-0.512V range = Gain 8
#define PGA_256                 0x0A00 // +/-0.256V range = Gain 16

#define MODE_CONTINUOUS         0x0000 // Continuous conversion mode
#define MODE_SINGLE_SHOT        0x0100 // Power-down single-shot mode (default)

#define DATA_RATE_8             0x0000 // 8 samples per second
#define DATA_RATE_16            0x0020 // 16 samples per second
#define DATA_RATE_32            0x0040 // 32 samples per second
#define DATA_RATE_64            0x0060 // 64 samples per second
#define DATA_RATE_128           0x0080 // 128 samples per second (default)
#define DATA_RATE_250           0x00a0 // 250 samples per second
#define DATA_RATE_475           0x00c0 // 475 samples per second
#define DATA_RATE_860           0x00e0 // 860 samples per second

#define COMP_MODE_TRADITIONAL   0x0000 // Traditional comparator with hysteresis (default)
#define COMP_MODE_WINDOW        0x0010 // Window comparator

#define COMP_POLARITY_ACTIVE_LO 0x0000 // ALERT/RDY pin is low when active (default)
#define COMP_POLARITY_ACTIVE_HI 0x0008 // ALERT/RDY pin is high when active

#define COMP_NON_LATCHING       0x0000 // Non-latching comparator (default)
#define COMP_LATCHING           0x0004 // Latching comparator

#define COMP_QUEUE_1            0x0000 // Assert ALERT/RDY after one conversions
#define COMP_QUEUE_2            0x0001 // Assert ALERT/RDY after two conversions
#define COMP_QUEUE_4            0x0002 // Assert ALERT/RDY after four conversions
#define COMP_QUEUE_NONE         0x0003 // Disable the comparator and put ALERT/RDY in high state (default)

#define CONFIG_OS_MASK                  0x8000
#define CONFIG_MULTIPLEXER_MASK         0x7000
#define CONFIG_PGA_MASK                 0x0E00
#define CONFIG_MODE_MASK                0x0100
#define CONFIG_DATA_RATE_MASK           0x00E0
#define CONFIG_COMPARATOR_MODE_MASK     0x0010
#define CONFIG_COMPARATOR_POLARITY_MASK 0x0008
#define CONFIG_COMPARATOR_LATCHING_MASK 0x0004
#define CONFIG_COMPARATOR_QUEUE_MASK    0x0003


#define REG_CONVERSION          0x00
#define REG_CONFIG              0x01

thunder::PressureTransducer::PressureTransducer(int adapter_nr, uint8_t address)
: m_i2c_path(fmt::format("/dev/i2c-{}", adapter_nr)) {
  
  m_i2c_fd = open(m_i2c_path.c_str(), O_RDWR);
  if (m_i2c_fd < 0) {
    fmt::print(stderr, "Failed to open I2C device at {}.\n", m_i2c_path.string());
    m_connected = false;
    return;
  }
  
  if (ioctl(m_i2c_fd, I2C_SLAVE, address) < 0) {
    fmt::print(stderr, "Failed to set I2C device to address 0x{:02x}.\n", address);
    m_connected = false;
    close(m_i2c_fd);
    return;
  }
  
  // Config things...
  set_multiplexer(MUX_DIFF_0_1);
  set_pga(PGA_4096);
  set_mode(MODE_CONTINUOUS);
  set_data_rate(DATA_RATE_128);
  set_comparator_mode(COMP_MODE_TRADITIONAL);
  set_comparator_polarity(COMP_POLARITY_ACTIVE_LO);
  set_comparator_latching(COMP_NON_LATCHING);
  set_comparator_queue(COMP_QUEUE_NONE);
  
  // Set the config.
  write_register(REG_CONFIG, m_config);
}

thunder::PressureTransducer::~PressureTransducer() {
  if (m_connected) {
    close(m_i2c_fd);
  }
}

double thunder::PressureTransducer::read_pressure() {
  // 0-4.096V range, 0-0xFFFF
  uint16_t result = read_register(REG_CONVERSION);
  double voltage = (static_cast<double>(result) / 0xFFFF) * 4.096;
  
  double pressure_pct = (voltage - 0.5) / 4.0;
  return pressure_pct * 200.0;
}

uint16_t thunder::PressureTransducer::read_register(uint8_t reg) {
  if (!m_connected) {
    return 0x0;
  }
  
  uint8_t buffer[2];
  // 8-bit register address.
  buffer[0] = reg;
  
  // Which register to read.
  write(m_i2c_fd, buffer, 1);
  
  // Read the register value.
  read(m_i2c_fd, buffer, 2);
  
  // 16-bit register value.
  uint16_t value = static_cast<uint16_t>(buffer[0]) << 8;
  value |= static_cast<uint16_t>(buffer[1]);
  
  return value;
}

void thunder::PressureTransducer::write_register(uint8_t reg, uint16_t value) {
  uint8_t buffer[3];
  // 8-bit register address.
  buffer[0] = reg;
  // 16-bit register value.
  buffer[1] = static_cast<uint8_t>(value >> 8);
  buffer[2] = static_cast<uint8_t>(value & 0xFF);

  write(m_i2c_fd, buffer, 3);
}

void thunder::PressureTransducer::set_config(uint16_t value, uint16_t mask) {
  m_config &= ~mask;
  m_config |= (value & mask);
}

void thunder::PressureTransducer::set_os(uint16_t value) {
  set_config(value, CONFIG_OS_MASK);
}

void thunder::PressureTransducer::set_multiplexer(uint16_t value) {
  set_config(value, CONFIG_MULTIPLEXER_MASK);
}

void thunder::PressureTransducer::set_pga(uint16_t value) {
  set_config(value, CONFIG_PGA_MASK);
}

void thunder::PressureTransducer::set_mode(uint16_t value) {
  set_config(value, CONFIG_MODE_MASK);
}

void thunder::PressureTransducer::set_data_rate(uint16_t value) {
  set_config(value, CONFIG_DATA_RATE_MASK);
}

void thunder::PressureTransducer::set_comparator_mode(uint16_t value) {
  set_config(value, CONFIG_COMPARATOR_MODE_MASK);
}

void thunder::PressureTransducer::set_comparator_polarity(uint16_t value) {
  set_config(value, CONFIG_COMPARATOR_POLARITY_MASK);
}

void thunder::PressureTransducer::set_comparator_latching(uint16_t value) {
  set_config(value, CONFIG_COMPARATOR_LATCHING_MASK);
}

void thunder::PressureTransducer::set_comparator_queue(uint16_t value) {
  set_config(value, CONFIG_COMPARATOR_QUEUE_MASK);
}
