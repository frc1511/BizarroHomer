#include <BizarroHomerDashboardServer/ConnectionManager.hpp>
#include <BizarroHomerDashboardServer/HTTPRequest.hpp>
#include <BizarroHomerDashboardServer/DataManager.hpp>
#include <unistd.h>
#include <sys/fcntl.h>
#include <fmt/core.h>
#include <filesystem>
#include <chrono>

#define SERVER_PATH "/var/frc1511/BizarroHomer/dashboard/"

ConnectionManager::ConnectionManager(std::size_t thread_num) {
  for (std::size_t i = 0; i < thread_num; i++) {
    m_workers.emplace_back([this]() {
      this->worker_thread();
    });
  }
}

ConnectionManager::~ConnectionManager() {
  {
    std::lock_guard<std::mutex> lk(m_queue_mutex);
    m_connection_tasks = std::queue<Task>();
    m_should_term = true;
  }
  m_condition.notify_all();
  
  for (std::thread& worker : m_workers) {
    worker.join();
  }
}

void ConnectionManager::handle_new_connection(int client_fd) {
  {
    std::lock_guard<std::mutex> lk(m_queue_mutex);
    ConnectionHandlerFunction func = [this](int client_fd, bool* _should_term) {
      this->handle_connection(client_fd, _should_term);
    };
    // emplace() is being stupid so this'll have to do.
    m_connection_tasks.push(Task{func, client_fd});
  }
  m_condition.notify_one();
}

void ConnectionManager::worker_thread() {
  while (true) {
    Task task;
    
    {
      std::unique_lock<std::mutex> lk(m_queue_mutex);
      m_condition.wait(lk, [this] { return m_should_term || !m_connection_tasks.empty(); });
      
      if (m_should_term) break;
      
      task = std::move(m_connection_tasks.front());
      m_connection_tasks.pop();
    }
    
    task.func(task.client_fd, &m_should_term);
  }
}

void ConnectionManager::handle_connection(int client_fd, bool* should_term) {
  using namespace std::literals::chrono_literals;
  auto start = std::chrono::steady_clock::now();
  
  char buf[81920];
  while (!*should_term) {
    auto now = std::chrono::steady_clock::now();
    if (now - start >= 10s) {
      break;
    }
    
    ssize_t len = read(client_fd, buf, sizeof(buf));
    if (len < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      break;
    }
    else if (len <= 0) {
      continue;
    }
    
    HTTPResponse::Status response_status = HTTPResponse::Status::_200_OK;
    
    try {
      HTTPRequest request(std::string(buf, len));
      
      len = 0;
      
      if (request.get_method() != HTTPMethod::GET) {
        response_status = HTTPResponse::Status::_405_METHOD_NOT_ALLOWED;
      }
      else {
        std::string target_rel_path = request.get_target();
        
        if (target_rel_path == "/get/values") {
          std::string str = DataManager::get()->csv();
          strncpy(buf, str.c_str(), str.length());
          len = str.length();
        }
        else {
          std::string target_abs_path = get_desired_target(target_rel_path, response_status);
          if (!target_abs_path.empty()) {
            // Get the file!!
            int fd = open(target_abs_path.c_str(), O_RDONLY);
            if (fd <= 0) {
              if (response_status == HTTPResponse::Status::_200_OK) {
                response_status = HTTPResponse::Status::_404_NOT_FOUND;
              }
            }
            else {
              len = read(fd, buf, sizeof(buf));
              if (len < 0) {
                len = 0;
              }
              close(fd);
            }
          }
        }
      }
    }
    catch (...) {
      response_status = HTTPResponse::Status::_400_BAD_REQUEST;
      len = 0;
    }
    
    HTTPResponse response(response_status, buf, len);
    std::string response_str = response.string();
    
    // Turn off non-blocking IO.
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags & ~O_NONBLOCK);
    
    ssize_t size = write(client_fd, response_str.c_str(), response_str.length());
    if (size < 0) {
      break;
    }
    
    // Set the socket back to non-blocking i/o.
    flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    
    // Refresh timer after last message.
    start = std::chrono::steady_clock::now();
  }

  close(client_fd);

  fmt::print("Closed connection to client\n");
}

std::string ConnectionManager::get_desired_target(std::string target_request, HTTPResponse::Status& status) {
  std::filesystem::path target(SERVER_PATH);
  if (target_request == "/coffee") {
    // I'm a teapot!!
    status = HTTPResponse::Status::_418_IM_A_TEAPOT;
    target /= "418.html";
    return std::filesystem::is_regular_file(target) ? target.string() : "";
  }
  
  if (!target_request.empty()) {
    if (target_request.at(0) == '/') {
      target_request.erase(0, 1);
    }
  }
  target /= target_request;
  if (std::filesystem::is_regular_file(target)) {
    // Path is good!!
    return target.string();
  }
  if (std::filesystem::is_directory(target)) {
    target /= "index.html";
    if (std::filesystem::is_regular_file(target)) {
      // Found index.html when asked for directory.
      return target.string();
    }
  }
  
  // Not found!!
  status = HTTPResponse::Status::_404_NOT_FOUND;
  target = std::filesystem::path(SERVER_PATH) / "404.html";
  
  return std::filesystem::is_regular_file(target) ? target.string() : "";
}
