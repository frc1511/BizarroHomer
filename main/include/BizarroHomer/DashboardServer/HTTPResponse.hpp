#pragma once

#include <string>
#include <cstddef>

class HTTPResponse {
public:
  enum class Status {
    _200_OK                 = 200,
    _400_BAD_REQUEST        = 400,
    _404_NOT_FOUND          = 404,
    _405_METHOD_NOT_ALLOWED = 405,
    _418_IM_A_TEAPOT        = 418,
  };
  
  HTTPResponse(Status status, void* content = nullptr, std::size_t content_size = 0);
  
  //
  // Returns the HTTP response as a string.
  //
  std::string string();
  
private:
  std::string m_response;
};
