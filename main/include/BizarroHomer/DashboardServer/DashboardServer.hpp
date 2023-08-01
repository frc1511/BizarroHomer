#pragma once

#include <BizarroHomer/DashboardServer/ConnectionManager.hpp>
#include <string_view>

class DashboardServer {
public:
  DashboardServer(int port = 80);
  ~DashboardServer();
  
  void process();
  
  //
  // Initializes or updates a key/value pair in the dashboard.
  //
  // Important: Keep the string length of key and value less than 20 characters.
  //
  void update_value(std::string_view key, std::string_view value);
  void update_value(std::string_view key, double value);
  void update_value(std::string_view key, bool value);
  void update_value(std::string_view key, int value);
  
private:
  int m_port;
  int m_server_fd;
  bool m_init = false;
  
  // 4 simultaneous connections should be plenty.
  ConnectionManager m_connection_manager { 4 };
};
