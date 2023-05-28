#pragma once

#include <BizarroHomerShared/Config/Config.hpp>
#include <cstddef>

class IPCSender {
public:
  IPCSender(const char* pathname, int proj_id);
  ~IPCSender();
  
  template<typename T>
  bool send_msg(T& msg);
  
private:
  bool send_msg(void* msg, size_t msg_size);
  
  int msqid = -1;
  bool ipc_open = false;
};

#include <BizarroHomerShared/IPC/IPCSender.tpp>
