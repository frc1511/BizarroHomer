#include <BizarroHomerDiagnosticServer/Util/Parser.hpp>
#include <fstream>
#include <fmt/core.h>
#include <cmath>

std::string Parser::get_file(std::filesystem::path path) {
  std::ifstream file(path);
  if (!file) {
    fmt::print("Error: Could not open file \"{}\" from directory \"{}\"\n", path.string(), std::filesystem::current_path().string());
    return "";
  }

  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  // Remove DOS line endings.
  std::string new_str;
  for (const char& c : str) {
    if (c != '\r') {
      new_str.push_back(c);
    }
  }

  return new_str;
}

std::ptrdiff_t Parser::count_while(Iter curr_iter, Iter end_iter, ConditionFunc cond_func) {
  std::ptrdiff_t count = 0;
  while (curr_iter != end_iter && cond_func(*curr_iter)) {
    ++curr_iter;
    ++count;
  }
  return count;
}

std::string Parser::parse_while(Iter& curr_iter, Iter end_iter, ConditionFunc cond_func) {
  std::ptrdiff_t count = count_while(curr_iter, end_iter, cond_func);
  std::string str(curr_iter, curr_iter + count);
  curr_iter += count;
  return str;
}

std::ptrdiff_t Parser::count_until(Iter curr_iter, Iter end_iter, std::string chars) {
  return count_while(curr_iter, end_iter, [&chars](char c) {
    return chars.find(c) == std::string::npos;
  });
}

std::string Parser::parse_until(Iter& curr_iter, Iter end_iter, std::string chars) {
  std::ptrdiff_t count = count_until(curr_iter, end_iter, chars);
  std::string str(curr_iter, curr_iter + count);
  curr_iter += count;
  return str;
}

double Parser::parse_number(Iter& curr_iter, Iter end_iter) {
  // Handle + and -
  int sign = 1;
  if (*curr_iter == '-') {
    sign = -1;
    ++curr_iter;
  }
  else if (*curr_iter == '+') {
    ++curr_iter;
  }

  // Parse the integer part.
  std::string int_str = parse_while(curr_iter, end_iter, [](char c) {
    return std::isdigit(c);
  });

  bool do_more = *curr_iter == '.' || *curr_iter == 'e';
  
  if (curr_iter == end_iter || !do_more) {
    return std::stoi(int_str) * sign;
  }

  double num = std::stoi(int_str);

  // Parse the decimal part.
  if (*curr_iter == '.') {
    ++curr_iter;
    std::string dec_str = parse_while(curr_iter, end_iter, [](char c) {
      return std::isdigit(c);
    });

    num = std::stod(fmt::format("{}.{}", int_str, dec_str)) * sign;
  }

  // Parse the exponent part.
  if (*curr_iter == 'e' || *curr_iter == 'E') {
    ++curr_iter;
    int exp_sign = 1;
    if (*curr_iter == '-') {
      exp_sign = -1;
      ++curr_iter;
    }
    else if (*curr_iter == '+') {
      ++curr_iter;
    }

    std::string exp_str = parse_while(curr_iter, end_iter, [](char c) {
      return std::isdigit(c);
    });

    num *= std::pow(10, std::stoi(exp_str) * exp_sign);
  }

  return num;
}
