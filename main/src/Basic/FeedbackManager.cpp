#include <BizarroHomer/Basic/FeedbackManager.hpp>
#include <cstring>

#define MAX_KEY 20
#define MAX_VAL 12

struct IPCMessage {
  long mtype = 1;
  struct Data {
    char key[MAX_KEY];
    char key_len;
    char value[MAX_VAL];
    char value_len;
  } data;
};

FeedbackManager::FeedbackManager()
: s(IPC_PATHNAME, 'Z') { }

void FeedbackManager::send_value(std::string_view key, std::string_view value) {
  IPCMessage msg;
  
  char key_len = key.length() > MAX_KEY ? MAX_KEY : key.length();
  std::memcpy(msg.data.key, key.data(), key_len);
  msg.data.key_len = key_len;
  
  char value_len = value.length() > MAX_VAL ? MAX_VAL : value.length();
  std::memcpy(msg.data.value, value.data(), value_len);
  msg.data.value_len = value_len;
  
  std::lock_guard<std::mutex> lk(send_mut);
  s.send_msg(msg);
}

FeedbackManager FeedbackManager::instance;
