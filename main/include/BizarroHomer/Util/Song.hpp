#pragma once

#include <filesystem>
#include <chrono>

using namespace std::literals::chrono_literals;

struct Song {
  std::filesystem::path primary_chrp;
  std::filesystem::path secondary_chrp;
  std::chrono::milliseconds length;
  
  Song(std::filesystem::path primary_chrp, std::chrono::milliseconds length);
  Song(std::filesystem::path primary_chrp, std::filesystem::path secondary_chrp, std::chrono::milliseconds length);
};
