#include <BizarroHomer/DashboardServer/DashboardServer.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <fmt/core.h>
#include <chrono>
#include <ctime>

DashboardServer::DashboardServer(int port)
: m_port(port) {

}

DashboardServer::~DashboardServer() {
  close(m_server_fd);
}

void DashboardServer::process() {
  if (!m_init) {
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd <= 0) {
      fmt::print(stderr, "socket() failed:\n\t{}\n", strerror(errno));
      return;
    }
    
    // Set O_NONBLOCK flag for nonblocking IO.
    int flags = fcntl(m_server_fd, F_GETFL, 0);
    fcntl(m_server_fd, F_SETFL, flags | O_NONBLOCK);
    
    // Create ip socket address.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET, // address family
    addr.sin_port = htons(m_port), // port
    addr.sin_addr.s_addr = INADDR_ANY, // ip address
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    
    // Bind the address to the socket.
    if (bind(m_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      fmt::print(stderr, "bind() failed:\n\t{}\n", strerror(errno));
      close(m_server_fd);
      return;
    }
    
    // Listen for connections on socket.
    if (listen(m_server_fd, 10) < 0) {
      fmt::print(stderr, "listen() failed:\n\t{}\n", strerror(errno));
      close(m_server_fd);
      return;
    }
    
    fmt::print(stdout, "HTTP/1.1 Server running on port {}\n", m_port);
    m_init = true;
  }
  
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  
  // Accept connection.
  int client_fd = accept(m_server_fd, (struct sockaddr*)&client_addr, &addr_len);
  if (client_fd < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // No incoming connection.
    }
    else {
      fmt::print(stderr, "accept() failed:\n\t{}\n", strerror(errno));
    }
    return;
  }
  
  // Set the client socket to non-blocking.
  int flags = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
  
  flags = 1;
  if (setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags))) {
    fmt::print(stderr, "setsocketopt(), failed to set SO_KEEPALIVE: \n\t{}\n", strerror(errno));
  };
  
  fmt::print(stdout, "Accepted connection from {}\n", inet_ntoa(client_addr.sin_addr));
  
  m_connection_manager.handle_new_connection(client_fd);
}

void DashboardServer::update_value(std::string_view key, std::string_view value) {
  m_connection_manager.update_value(key, value);
}

void DashboardServer::update_value(std::string_view key, double value) {
  m_connection_manager.update_value(key, fmt::format("{:.5f}", value));
}

void DashboardServer::update_value(std::string_view key, bool value) {
  m_connection_manager.update_value(key, value ? "True" : "False");
}

void DashboardServer::update_value(std::string_view key, int value) {
  m_connection_manager.update_value(key, fmt::format("{}", value));
}

