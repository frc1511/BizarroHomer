#include <BizarroHomer/Basic/Robot.hpp>

Robot::Robot() {

}

Robot::~Robot() {

}

void Robot::process() {
  for (Mechanism* m : all_mechanisms) {
    m->process();
  }
  
  // Feedback...
  static int run = 0;
  if (!run) {
    for (Mechanism* m : all_mechanisms) {
      m->send_feedback();
    }
  }
  run++;
  run %= 15; // Every 300ms
}
