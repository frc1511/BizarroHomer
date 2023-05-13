#include <BizarroHomer/Hardware/CAN.hpp>
#include <fmt/core.h>

CAN::CAN(int id, int manufacturer, int type, std::string interface) {

}

void CAN::write_packet(const uint8_t* data, int length, int api_id) {
  fmt::print("CAN::write_packet() unimplemented\n");
}

void CAN::write_packet_repeating(const uint8_t* data, int length, int api_id, int period) {
  fmt::print("CAN::write_packet_repeating() unimplemented\n");
}

bool CAN::read_packet_new(int api_id, Data* data) {
  fmt::print("CAN::read_packet_new() unimplemented\n");
  return false;
}

bool CAN::read_packet_latest(int api_id, Data* data) {
  fmt::print("CAN::read_packet_latest() unimplemented\n");
  return false;
}

