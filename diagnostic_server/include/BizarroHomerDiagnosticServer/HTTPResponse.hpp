#pragma once

#include <string>
#include <vector>
#include <utility>
#include <cstddef>

class HTTPResponse {
public:
  enum class Status {
    _200_OK                 = 200,
    _404_NOT_FOUND          = 404,
    _405_METHOD_NOT_ALLOWED = 405,
    _418_IM_A_TEAPOT        = 418,
  };
  
  HTTPResponse(Status status, std::vector<std::pair<std::string, std::string>>, void* content = nullptr, std::size_t content_size = 0);
  
  std::string string();
  
private:
  std::string response;
};
