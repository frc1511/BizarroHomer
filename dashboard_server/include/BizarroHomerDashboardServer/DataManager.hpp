#pragma once

#include <map>
#include <string>
#include <thread>
#include <mutex>

class DataManager {
public:
  inline static DataManager* get() {
    return &instance;
  }
  
  DataManager(DataManager const&) = delete;
  DataManager& operator=(DataManager const&) = delete;
  
  std::string csv();
  
private:
  DataManager();
  ~DataManager();
  
  std::string csv_str;

  std::map<std::string, std::string> data;
  
  std::thread data_thread;
  std::mutex data_mutex;
  
  void data_thread_main();

  void update_csv();
  bool new_data = true;
  
  bool should_term = false;
  
  static DataManager instance;
};
