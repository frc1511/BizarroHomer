#pragma once

#include <cstddef>

class IPCReceiver {
public:
  IPCReceiver(const char* pathname, int proj_id);
  ~IPCReceiver();
  
  template<typename T>
  void recv_msg(T& msg);
  
private:
  void recv_msg(void* msg, size_t msg_size);
  
  int msqid = -1;
  bool ipc_open = false;
};

#include <BizarroHomerShared/IPC/IPCReceiver.tpp>
