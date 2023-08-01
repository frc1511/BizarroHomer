#pragma once

#include <BizarroHomer/DashboardServer/DashboardServer.hpp>
#include <BizarroHomer/Hardware/PDP.hpp>
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
  
  Drive m_drive;
  Shooter m_shooter;
  Controls m_controls { &m_drive, &m_shooter, &m_pdp };
  
  std::vector<Mechanism*> m_all_mechanisms {
    &m_drive, &m_controls, &m_shooter
  };
};
