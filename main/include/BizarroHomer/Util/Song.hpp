#pragma once

#include <filesystem>
#include <chrono>

using namespace std::literals::chrono_literals;

//
// Represents a song to be played by Falcon 500s.
//
struct Song {
  // The .chrp files to play on the motors.
  std::filesystem::path primary_chrp;
  std::filesystem::path secondary_chrp;
  
  // The some length (ms).
  std::chrono::milliseconds length;
  
  //
  // Constructs a song from a single music file.
  //
  Song(std::filesystem::path primary_chrp, std::chrono::milliseconds length);
  
  //
  // Constructs a song from a primary and a secondary music file.
  //
  Song(std::filesystem::path primary_chrp, std::filesystem::path secondary_chrp, std::chrono::milliseconds length);
};
