#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Basic/FeedbackManager.hpp>
#include <fmt/core.h>
#include <cmath>
#include <algorithm>

Drive::Drive() {

}

Drive::~Drive() {

}

void Drive::process() {
  m_drive_left.set(-m_target_left * 0.3);
  m_drive_right.set(m_target_right * 0.3);
}

void Drive::tank_control(double left, double right) {
  m_target_left = left;
  m_target_right = right;
}

void Drive::arcade_control(double forwards, double turn) {
  // A real simple arcade drive.
  m_target_left = std::clamp(forwards + turn, -1.0, 1.0);
  m_target_right = std::clamp(forwards - turn, -1.0, 1.0);
}

void Drive::send_feedback() {
  
}
