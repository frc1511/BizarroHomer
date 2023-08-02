#pragma once

#include <filesystem>
#include <string>
#include <functional>

class Parser {
public:
    using Iter = std::string::const_iterator;
    using ConditionFunc = std::function<bool(char)>;
    
    //
    // Returns the contents of a file as a string.
    //
    static std::string get_file(std::filesystem::path path);
    
    //
    // Counts the number of characters while a condition is met.
    //
    static std::ptrdiff_t count_while(Iter curr_iter, Iter end_iter, ConditionFunc cond_func);
    
    //
    // Parses a string while a condition is met.
    //
    static std::string parse_while(Iter& curr_iter, Iter end_iter, ConditionFunc cond_func);
    
    //
    // Counts the number of characters until a character is met.
    //
    static std::ptrdiff_t count_until(Iter curr_iter, Iter end_iter, std::string chars);
    
    //
    // Parses a string until a character is met.
    //
    static std::string parse_until(Iter& curr_iter, Iter end_iter, std::string chars);
    
    //
    // Parses a number from a string.
    //
    static double parse_number(Iter& curr_iter, Iter end_iter);
};
