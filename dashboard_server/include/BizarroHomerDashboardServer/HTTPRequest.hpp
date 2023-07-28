#pragma once

#include <BizarroHomerDashboardServer/Util/Parser.hpp>
#include <string>
#include <utility>
#include <map>

enum class HTTPMethod {
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  CONNECT,
  OPTIONS,
  TRACE,
  PATCH,
};

class HTTPRequest {
public:
  HTTPRequest(std::string request_string);
  ~HTTPRequest();
  
  HTTPMethod get_method();
  
  std::string get_target();
  
  const std::map<std::string, std::string>& get_headers();
  
private:
  void parse_start_line();
  void parse_headers();
  bool parse_header(std::pair<std::string, std::string>& header);
  void parse_body();
  
  std::string m_request_str;
  Parser::Iter m_iter;
  
  HTTPMethod m_method;
  std::string m_target;
  
  std::map<std::string, std::string> m_headers;
};
