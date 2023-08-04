#pragma once

#include <BizarroHomer/DashboardServer/DashboardServer.hpp>
#include <BizarroHomer/Hardware/PDP.hpp>
#include <BizarroHomer/Illumination/BlinkyBlinky.hpp>
#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Drive/Drive.hpp>
#include <BizarroHomer/Control/Controls.hpp>
#include <BizarroHomer/Shooter/Shooter.hpp>
#include <vector>

class Robot {
public:
  Robot(DashboardServer* dashboard_server);
  ~Robot();
  
  // Called every 20ms.
  void process();
  
private:
  DashboardServer* m_dashboard_server;
  
  thunder::PDP m_pdp { 0 };
  
  /* BlinkyBlinky m_blinky_blinky; */
  
  Drive m_drive;
  Shooter m_shooter;
  Controls m_controls { &m_drive, &m_shooter, &m_pdp, nullptr };// &m_blinky_blinky };
  
  std::vector<Mechanism*> m_all_mechanisms {
    /*&m_blinky_blinky,*/ &m_drive, &m_controls, &m_shooter
  };
  
  std::vector<Mechanism*> m_control_mechanisms {
    &m_drive, &m_controls, &m_shooter
  };
  std::vector<Mechanism*> m_other_mechanisms {
    /* &m_blinky_blinky */
  };
  std::size_t m_other_mech_iter = 0;
  
  std::size_t m_feedback_iter = 0;
};
