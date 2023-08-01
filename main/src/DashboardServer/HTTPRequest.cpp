#include <BizarroHomer/DashboardServer/HTTPRequest.hpp>
#include <map>

HTTPRequest::HTTPRequest(std::string request_str)
: m_request_str(request_str) {
  m_iter = m_request_str.cbegin();
  
  parse_start_line();
  parse_headers();
  parse_body();
}

HTTPRequest::~HTTPRequest() {

}

HTTPMethod HTTPRequest::get_method() {
  return m_method;
}

std::string HTTPRequest::get_target() {
  return m_target;
}

const std::map<std::string, std::string>& HTTPRequest::get_headers() {
  return m_headers;
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
  if (*m_iter < 'C' || *m_iter > 'T') throw std::exception();
  
  // HTTP method.
  std::string method_str = Parser::parse_until(m_iter, m_request_str.cend(), " ");
  m_iter++;
  
  m_method = method_str_map.at(method_str);
  
  // Request target.
  m_target = Parser::parse_until(m_iter, m_request_str.cend(), " ");
  m_iter++;
  
  // Skip HTTP version for now...
  m_iter += Parser::count_while(m_iter, m_request_str.cend(), [](char c) { return c != '\n' && c != '\r'; });
  m_iter++;
}

void HTTPRequest::parse_headers() {
  std::pair<std::string, std::string> header;
  while (parse_header(header)) {
    m_headers.insert(header);
  }
}

bool HTTPRequest::parse_header(std::pair<std::string, std::string>& header) {
  if (m_iter == m_request_str.cend()) return false;
  if (!*m_iter) return false;
  
  if (*m_iter == '\n' || *m_iter == '\r') {
    m_iter++;
    return false;
  }
  
  header.first = Parser::parse_until(m_iter, m_request_str.cend(), ":");
  m_iter++;
  header.second = Parser::parse_until(m_iter, m_request_str.cend(), "\n\r");
  m_iter++;
  
  return true;
}

void HTTPRequest::parse_body() { }
