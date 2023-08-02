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
  
  //
  // Adds a new connection to the queue of connections to be handled.
  //
  void handle_new_connection(int client_fd);
  
  //
  // Adds/updates a value in the value map.
  //
  void update_value(std::string_view key, std::string_view value);
  
private:
  //
  // Represents a connection task.
  //
  struct Task {
    ConnectionHandlerFunction func;
    int client_fd;
  };
  
  std::vector<std::thread> m_workers;
  std::queue<Task> m_connection_tasks;
  
  std::map<std::string, std::string> m_values;
  std::string m_values_csv;
  
  bool m_should_term = false;
  
  //
  // Function run by each worker thread.
  //
  void worker_thread();
  
  //
  // Handles a new connection from a client.
  //
  void handle_connection(int client_fd, bool* should_term);
  
  //
  // Resolves a target path from a request.
  //
  std::string get_desired_target(std::string target_request, HTTPResponse::Status& status);
  
  std::mutex m_queue_mutex;
  std::condition_variable m_condition;
};
