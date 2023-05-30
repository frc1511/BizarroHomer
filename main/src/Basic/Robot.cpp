#include <BizarroHomer/Basic/Robot.hpp>

Robot::Robot() {

}

Robot::~Robot() {

}

void Robot::process() {
  for (Mechanism* m : all_mechanisms) {
    m->process();
  }
}
