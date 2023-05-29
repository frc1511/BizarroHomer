#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

IPCSender::IPCSender(const char* pathname, int proj_id) {
  key_t key = ftok(pathname, proj_id);
  if (key < 0) {
    fmt::print("ftok({}, {}) failed:\n\t{}\n", pathname, proj_id, strerror(errno));
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

IPCSender::~IPCSender() { }

bool IPCSender::send_msg(void* msg, size_t msg_size) {
  if (!ipc_open) return false;
  
  if (msgsnd(msqid, msg, msg_size, 0) < 0) {
    fmt::print("msgsnd() failed:\n\t{}\n", strerror(errno));
    return false;
  }
  
  return true;
}
