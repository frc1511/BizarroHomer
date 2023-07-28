#pragma once

#include <BizarroHomerDashboardServer/HTTPResponse.hpp>
#include <cstddef>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using ConnectionHandlerFunction = std::function<void(int client_fd, bool* should_term)>;

class ConnectionManager {
public:
  ConnectionManager(std::size_t thread_num);
  ~ConnectionManager();
  
  void handle_new_connection(int client_fd);
  
private:
  
  struct Task {
    ConnectionHandlerFunction func;
    int client_fd;
  };
  
  std::vector<std::thread> workers;
  std::queue<Task> conn_tasks;
  
  bool should_term = false;
  
  void worker_thread();

  void handle_connection(int client_fd, bool* should_term);

  std::string get_desired_target(std::string target_request, HTTPResponse::Status& status);
  
  std::mutex queue_mut;
  std::condition_variable condition;
};
