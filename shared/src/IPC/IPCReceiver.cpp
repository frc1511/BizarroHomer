#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>

IPCReceiver::IPCReceiver(const char* pathname, int proj_id) {
  key_t key = ftok(pathname, proj_id);
  if (key < 0) {
    fmt::print("ftok() failed:\n\t{}\n", strerror(errno));
    return;
  }
  
  // Create or open the message queue.
  msqid = msgget(key, IPC_CREAT | 0666);
  if (msqid < 0) {
    fmt::print("msgget() failed:\n\t{}\n", strerror(errno));
    return;
  }
  
  ipc_open = true;
}

IPCReceiver::~IPCReceiver() { }

void IPCReceiver::recv_msg(void* msg, size_t msg_size) {
  if (!ipc_open) return;
  
  std::memset(msg, 0, msg_size);
  if (msgrcv(msqid, msg, msg_size, 1, 0) < 0) {
    fmt::print("msgrcv() failed:\n\t{}\n", strerror(errno));
    return;
  }
}
