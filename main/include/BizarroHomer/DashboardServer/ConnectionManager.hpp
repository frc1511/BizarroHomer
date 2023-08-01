#pragma once

#include <BizarroHomer/DashboardServer/HTTPResponse.hpp>
#include <cstddef>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <string_view>

using ConnectionHandlerFunction = std::function<void(int client_fd, bool* should_term)>;

class ConnectionManager {
public:
  ConnectionManager(std::size_t thread_num);
  ~ConnectionManager();
  
  void handle_new_connection(int client_fd);
  
  void update_value(std::string_view key, std::string_view value);
  
private:
  struct Task {
    ConnectionHandlerFunction func;
    int client_fd;
  };
  
  std::vector<std::thread> m_workers;
  std::queue<Task> m_connection_tasks;
  
  std::map<std::string, std::string> m_values;
  std::string m_values_csv;
  
  bool m_should_term = false;
  
  void worker_thread();
  
  void handle_connection(int client_fd, bool* should_term);
  
  std::string get_desired_target(std::string target_request, HTTPResponse::Status& status);
  
  std::mutex m_queue_mutex;
  std::condition_variable m_condition;
};
