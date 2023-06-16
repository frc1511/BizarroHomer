#include <BizarroHomerDiagnosticServer/HTTPRequest.hpp>
#include <map>

HTTPRequest::HTTPRequest(std::string _request_str)
: request_str(_request_str) {
  iter = request_str.cbegin();
  
  parse_start_line();
  parse_headers();
  parse_body();
}

HTTPRequest::~HTTPRequest() {

}

HTTPMethod HTTPRequest::get_method() {
  return method;
}

std::string HTTPRequest::get_target() {
  return target;
}

const std::map<std::string, std::string>& HTTPRequest::get_headers() {
  return headers;
}

static const std::map<std::string, HTTPMethod> method_str_map {
  { "GET",     HTTPMethod::GET     },
  { "HEAD",    HTTPMethod::HEAD    },
  { "POST",    HTTPMethod::POST    },
  { "PUT",     HTTPMethod::PUT     },
  { "DELETE",  HTTPMethod::DELETE  },
  { "CONNECT", HTTPMethod::CONNECT },
  { "OPTIONS", HTTPMethod::OPTIONS },
  { "TRACE",   HTTPMethod::TRACE   },
  { "PATCH",   HTTPMethod::PATCH   },
};

void HTTPRequest::parse_start_line() {
  if (*iter < 'C' || *iter > 'T') throw std::exception();
  
  // HTTP method.
  std::string method_str = Parser::parse_until(iter, request_str.cend(), " ");
  iter++;
  
  method = method_str_map.at(method_str);
  
  // Request target.
  target = Parser::parse_until(iter, request_str.cend(), " ");
  iter++;
  
  // Skip HTTP version for now...
  iter += Parser::count_while(iter, request_str.cend(), [](char c) { return c != '\n' && c != '\r'; });
  iter++;
}

void HTTPRequest::parse_headers() {
  std::pair<std::string, std::string> header;
  while (parse_header(header)) {
    headers.insert(header);
  }
}

bool HTTPRequest::parse_header(std::pair<std::string, std::string>& header) {
  if (iter == request_str.cend()) return false;
  if (!*iter) return false;
  
  if (*iter == '\n' || *iter == '\r') {
    iter++;
    return false;
  }
  
  header.first = Parser::parse_until(iter, request_str.cend(), ":");
  iter++;
  header.second = Parser::parse_until(iter, request_str.cend(), "\n\r");
  iter++;
  
  return true;
}

void HTTPRequest::parse_body() { }
