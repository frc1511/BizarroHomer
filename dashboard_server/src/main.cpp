#include <BizarroHomerDashboardServer/ConnectionManager.hpp>
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

volatile sig_atomic_t sig_status = 0;

void signal_handler(int status) {
  sig_status = status;
  fmt::print(stdout, "caught signal: {}\n", status);
  fmt::print(stderr, "caught signal: {}\n", status);
}

int main(int argc, char** argv) {
  /* signal(SIGINT, signal_handler); */
  
  std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  fmt::print(stdout, "Dashboard Server Service starting at {}", std::ctime(&start_time));
  fmt::print(stderr, "Dashboard Server Service starting at {}", std::ctime(&start_time));
  
  int port = 80;
  
  if (argc == 2) {
    port = atoi(argv[1]);
  }
  
  int server_fd;
  
  {
    ConnectionManager conn_manager(4);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd <= 0) {
      fmt::print(stderr, "socket() failed:\n\t{}\n", strerror(errno));
      return 1;
    }
    
    // Set O_NONBLOCK flag for nonblocking IO.
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    
    // Create ip socket address.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET, // address family
    addr.sin_port = htons(port), // port
    addr.sin_addr.s_addr = INADDR_ANY, // ip address
    memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
    
    // Bind the address to the socket.
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
      fmt::print(stderr, "bind() failed:\n\t{}\n", strerror(errno));
      return 2;
    }
    
    // Listen for connections on socket.
    if (listen(server_fd, 10) < 0) {
      fmt::print(stderr, "listen() failed:\n\t{}\n", strerror(errno));
      return 3;
    }
    
    fmt::print(stdout, "HTTP/1.1 Server running on port {}\n", port);
    
    while (!sig_status) {
      struct sockaddr_in client_addr;
      socklen_t addr_len = sizeof(struct sockaddr_in);
      
      // Accept connection.
      int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
      if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          // No incoming connection.
        }
        else {
          fmt::print(stderr, "accept() failed:\n\t{}\n", strerror(errno));
        }
        continue;
      }
      
      // Set the client socket to non-blocking.
      int flags = fcntl(client_fd, F_GETFL, 0);
      fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
      
      flags = 1;
      if (setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags))) {
        fmt::print(stderr, "setsocketopt(), failed to set SO_KEEPALIVE: \n\t{}\n", strerror(errno));
      };
      
      fmt::print(stdout, "Accepted connection from {}\n", inet_ntoa(client_addr.sin_addr));

      conn_manager.handle_new_connection(client_fd);
    }
  }
  
  close(server_fd);
  
  return 0;
}