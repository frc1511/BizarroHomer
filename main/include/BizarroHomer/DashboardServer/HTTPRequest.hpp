#pragma once

#include <BizarroHomer/Util/Parser.hpp>
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
  
  //
  // Returns the HTTP method.
  //
  HTTPMethod get_method();
  
  //
  // Returns the target of the HTTP request.
  //
  std::string get_target();
  
  //
  // Returns a map of the headers in the HTTP request.
  //
  const std::map<std::string, std::string>& get_headers();
  
private:
  //
  // Functions to parse parts of the HTTP request.
  //
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
