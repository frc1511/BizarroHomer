#pragma once

#include <BizarroHomerShared/IPC/IPCSender.hpp>
#include <string_view>
#include <mutex>

class FeedbackManager {
public:
  inline static FeedbackManager* get() {
    return &instance;
  }
  
  FeedbackManager(FeedbackManager const&) = delete;
  FeedbackManager& operator=(FeedbackManager const&) = delete;
  
  void send_value(std::string_view key, std::string_view value);
  
private:
  FeedbackManager();
  ~FeedbackManager() = default;
  
  std::mutex send_mut;
  IPCSender s;
  
  static FeedbackManager instance;
};
