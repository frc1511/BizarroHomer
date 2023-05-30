#include <BizarroHomer/Drive/Drive.hpp>

Drive::Drive() {

}

Drive::~Drive() {

}

void Drive::process() {
  drive_left.set_percent(target_left);
  drive_right.set_percent(-target_right);
}

void Drive::control(double l, double r) {
  target_left = l;
  target_right = r;
}
