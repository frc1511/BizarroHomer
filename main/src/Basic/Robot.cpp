#include <BizarroHomer/Basic/Robot.hpp>
#include <BizarroHomer/Hardware/HardwareManager.hpp>
#include <fmt/core.h>

Robot::Robot(DashboardServer* dashboard_server)
: m_dashboard_server(dashboard_server) { }

Robot::~Robot() { }

void Robot::process() {
  thunder::HardwareManager::get()->process_orchestra();
  
  //
  // Control Mechanisms
  //
  for (Mechanism* mech : m_control_mechanisms) {
    mech->process();
  }
  
  //
  // Other Mechanisms (every 60ms).
  //
  if (m_other_mech_iter == 0) {
    for (Mechanism* mech : m_other_mechanisms) {
      mech->process();
    }
  }
  m_other_mech_iter++;
  m_other_mech_iter %= 3; // Every 60ms
  
  //
  // Dashboard Feedback (every 300ms).
  //
  if (m_dashboard_server) {
    // Feedback...
    if (m_feedback_iter == 0) {
      for (Mechanism* mech : m_all_mechanisms) {
        mech->send_feedback(m_dashboard_server);
      }
    }
    m_feedback_iter++;
    m_feedback_iter %= 15; // Every 300ms
  }
}
