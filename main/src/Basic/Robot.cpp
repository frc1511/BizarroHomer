#include <BizarroHomer/Basic/Robot.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <fmt/core.h>

Robot::Robot(DashboardServer* dashboard_server)
: m_dashboard_server(dashboard_server) { }

Robot::~Robot() { }

void Robot::process() {
  thunder::HardwareManager::get()->process_orchestra();
  
  for (Mechanism* mech : m_all_mechanisms) {
    mech->process();
  }

  if (m_dashboard_server) {
    // Feedback...
    static int run_num = 0;
    if (run_num == 0) {
      for (Mechanism* mech : m_all_mechanisms) {
        mech->send_feedback(m_dashboard_server);
      }
    }
    run_num++;
    run_num %= 15; // Every 300ms
  }
}
