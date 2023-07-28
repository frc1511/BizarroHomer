#include <BizarroHomer/Basic/Robot.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <fmt/core.h>

Robot::Robot() { }

Robot::~Robot() { }

void Robot::process() {
  thunder::HardwareManager::get()->process_orchestra();
  
  for (Mechanism* mech : all_mechanisms) {
    mech->process();
  }
  
  // Feedback...
  static int run_num = 0;
  if (run_num == 0) {
    for (Mechanism* mech : all_mechanisms) {
      mech->send_feedback();
    }
  }
  run_num++;
  run_num %= 15; // Every 300ms
}
