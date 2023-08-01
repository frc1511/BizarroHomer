#include <BizarroHomer/DashboardServer/DataManager.hpp>
#include <BizarroHomerShared/IPC/IPCReceiver.hpp>
#include <fmt/core.h>

#define MAX_KEY 20
#define MAX_VAL 12

struct IPCMessage {
  long mtype;
  struct Data {
    char key[MAX_KEY];
    char key_len;
    char value[MAX_VAL];
    char value_len;
  } data;
};

DataManager::DataManager()
: data_thread([this]() { this->data_thread_main(); }) { }

DataManager::~DataManager() {
  {
    std::lock_guard<std::mutex> lk(data_mutex);
    should_term = true;
  }
  data_thread.join();
}

std::string DataManager::csv() {
  std::lock_guard<std::mutex> lk(data_mutex);
  if (new_data) {
    update_csv();
    new_data = false;
  }
  return csv_str;
}

void DataManager::data_thread_main() {
  IPCReceiver r(IPC_PATHNAME, 'Z');
  while (true) {
    {
      std::lock_guard<std::mutex> lk(data_mutex);
      if (should_term) break;
    }
    
    IPCMessage msg;
    if (!r.recv_msg(&msg)) {
      break;
    }
    
    std::lock_guard<std::mutex> lk(data_mutex);
    data.insert_or_assign(
      std::string(msg.data.key, msg.data.key_len > MAX_KEY ? MAX_KEY : msg.data.key_len),
      std::string(msg.data.value, msg.data.value_len > MAX_VAL ? MAX_VAL : msg.data.value_len)
    );
    new_data = true;
  }
}

void DataManager::update_csv() {
  csv_str.clear();
  for (const auto& [k, v] : data) {
    csv_str += fmt::format("{},{}\n", k, v);
  }
}

DataManager DataManager::instance;
