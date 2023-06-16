#include <BizarroHomerDiagnosticServer/ConnectionManager.hpp>
#include <BizarroHomerDiagnosticServer/HTTPRequest.hpp>
#include <unistd.h>
#include <sys/fcntl.h>
#include <fmt/core.h>
#include <filesystem>
#include <chrono>

ConnectionManager::ConnectionManager(std::size_t thread_num) {
  for (std::size_t i = 0; i < thread_num; i++) {
    workers.emplace_back([this]() {
      this->worker_thread();
    });
  }
}

ConnectionManager::~ConnectionManager() {
  {
    std::lock_guard<std::mutex> lk(queue_mut);
    conn_tasks = decltype(conn_tasks)();
    should_term = true;
  }
  condition.notify_all();
  
  for (std::thread& worker : workers) {
    worker.join();
  }
}

void ConnectionManager::handle_new_connection(int client_fd) {
  {
    std::lock_guard<std::mutex> lk(queue_mut);
    conn_tasks.emplace([this](int client_fd, bool* _should_term) {
      this->handle_connection(client_fd, _should_term);
    }, client_fd);
  }
  condition.notify_one();
}

void ConnectionManager::worker_thread() {
  while (true) {
    Task task;
    
    {
      std::unique_lock<std::mutex> lk(queue_mut);
      condition.wait(lk, [this] { return should_term || !conn_tasks.empty(); });
      
      if (should_term) break;
      
      task = std::move(conn_tasks.front());
      conn_tasks.pop();
    }
    
    task.func(task.client_fd, &should_term);
  }
}

#define SERVER_PATH std::filesystem::path("/var/frc1511/BizarroHomer/diagnostic_server/")

void ConnectionManager::handle_connection(int client_fd, bool* should_term) {
  using namespace std::literals::chrono_literals;
  auto start = std::chrono::steady_clock::now();
  
  char buf[8192];
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
    
    HTTPRequest request(std::string(buf, len));
    
    HTTPResponse::Status response_status = HTTPResponse::Status::_200_OK;
    
    if (request.get_method() != HTTPMethod::GET) {
      response_status = HTTPResponse::Status::_405_METHOD_NOT_ALLOWED;
    }
    
    std::string target_rel_path = request.get_target();
    
    std::string target_abs_path = get_desired_target(target_rel_path, response_status);
    if (!target_abs_path.empty()) {
      // Get the file!!
      int fd = open(target_abs_path.c_str(), O_RDONLY);
      if (fd <= 0) {
        if (response_status == HTTPResponse::Status::_200_OK) {
          response_status = HTTPResponse::Status::_404_NOT_FOUND;
        }
        break;
      }
      else {
        len = read(fd, buf, sizeof(buf));
        if (len < 0) {
          len = 0;
        }
        close(fd);
      }
    }
    
    HTTPResponse response(response_status, {}, buf, len);
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
  std::filesystem::path target = SERVER_PATH;
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
    target = SERVER_PATH / "index.html";
    if (std::filesystem::is_regular_file(target)) {
      // Found index.html when asked for directory.
      return target.string();
    }
  }
  
  // Not found!!
  status = HTTPResponse::Status::_404_NOT_FOUND;
  target = SERVER_PATH / "404.html";
  return std::filesystem::is_regular_file(target) ? target.string() : "";
}
