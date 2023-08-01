#pragma once

#include <BizarroHomer/DashboardServer/DashboardServer.hpp>

class Mechanism {
public:
  virtual ~Mechanism() { }
  
  /**
   * @brief Mechanisms should implement this to perform periodic actions in
   *        response to commands given to the mechanism. This is called every
   *        20ms while the robot program is running.
   */
  virtual void process() { }
  
  /**
   * @brief Send operational and/or diagnostic feedback to the operator of the
   *        robot via the dashboard.
   */
  virtual void send_feedback(DashboardServer* dashboard) { }
};
