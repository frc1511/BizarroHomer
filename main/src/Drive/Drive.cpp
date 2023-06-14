#include <BizarroHomer/Drive/Drive.hpp>
#include <fmt/core.h>
#include <cmath>
#include <algorithm>

Drive::Drive() {

}

Drive::~Drive() {

}

void Drive::process() {
  drive_left.set_percent(-target_left * 0.3);
  drive_right.set_percent(target_right * 0.3);
  
  /* talon.set_percent(0.2); */
  /* fmt::print("talon: {}\n", talon.get_position()); */
  /* fmt::print("encoder: {}\n", encoder.get_angle()); */
}

void Drive::tank_control(double left, double right) {
  target_left = left;
  target_right = right;
}

void Drive::arcade_control(double forwards, double turn) {
  // A real simple arcade drive.
  target_left = std::clamp(forwards + turn, -1.0, 1.0);
  target_right = std::clamp(forwards - turn, -1.0, 1.0);
}
