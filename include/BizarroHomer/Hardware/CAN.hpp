#pragma once

#include <cstdint>
#include <string>

// Represents a CAN device.
class CAN {
public:
  /**
   * @brief Creates a CAN class instance.
   *
   * @param id The device id.
   * @param manufacturer The device manufacturer.
   * @param type The device type.
   * @param interface The CAN interface to use.
   */
  CAN(int id, int manufacturer, int type, std::string interface = "can0");
  
  /**
   * @brief Writes a packet to the CAN device with a specific ID.
   *
   * @param data The data to write (8 bytes max).
   * @param length The data length to write.
   * @param api_id The API ID to write.
   */
  void write_packet(const uint8_t* data, int length, int api_id);
  
  /**
   * @brief Writes a packet continuously to the CAN device with a specific ID.
   *
   * @param data The data to write (8 bytes max).
   * @param length The data length to write.
   * @param api_id The API ID to write.
   * @param period The period to repeat the write (ms).
   */
  void write_packet_repeating(const uint8_t* data, int length, int api_id, int period);
  
  struct Data {
    uint8_t data[8];
    int32_t length;
    uint64_t timestamp;
  };
  
  /**
   * @brief Reads a packet from the CAN device. Returns only new packets
   *        received. If no packet new is read, function returns false.
   *
   * @param api_id The API ID to read.
   * @param data The data received.
   * @return True if data is valid, false otherwise.
   */
  bool read_packet_new(int api_id, Data* data);
  
  /**
   * @brief Reads a packet from the CAN device. Continuously returns the last
   *        packet received.
   *
   * @param api_id The API ID to read.
   * @param data The data received.
   * @return True if data is valid, false otherwise.
   */
  bool read_packet_latest(int api_id, Data* data);
  
private:

};
