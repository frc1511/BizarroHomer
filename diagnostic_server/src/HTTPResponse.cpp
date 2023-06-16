#include <BizarroHomerDiagnosticServer/HTTPResponse.hpp>
#include <fmt/core.h>
#include <map>

std::map<HTTPResponse::Status, std::string> status_str_map {
  { HTTPResponse::Status::_200_OK,                 "200 OK"                 },
  { HTTPResponse::Status::_400_BAD_REQUEST,        "400 Bad Request"        },
  { HTTPResponse::Status::_404_NOT_FOUND,          "404 Not Found"          },
  { HTTPResponse::Status::_405_METHOD_NOT_ALLOWED, "405 Method Not Allowed" },
  { HTTPResponse::Status::_418_IM_A_TEAPOT,        "418 I'm a teapot"       },
};

HTTPResponse::HTTPResponse(Status status, void* content, std::size_t content_size) {
  response = fmt::format("HTTP/1.1 {}\n", status_str_map.at(status));
  response += "Connection: keep-alive\n";
  response += "Server: Bizarro Homer Diagnostic Server\n";
  if (content_size) {
    response += fmt::format("Content-Length: {}\n", content_size);
    response += '\n';
    response += std::string((char*)content, content_size);
  }
  else {
    response += '\n';
  }
}
  
std::string HTTPResponse::string() {
  return response;
}
