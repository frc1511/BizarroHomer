#pragma once

#include <cstddef>

class IPCSender {
public:
  IPCSender(const char* pathname, int proj_id);
  ~IPCSender();
  
  template<typename T>
  void send_msg(T& msg);
  
private:
  void send_msg(void* msg, size_t msg_size);
  
  int msqid = -1;
  bool ipc_open = false;
};

#include <BizarroHomerShared/IPC/IPCSender.tpp>
