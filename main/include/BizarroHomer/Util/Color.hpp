#pragma once

#include <cstdint>

class Color {
public:
  Color(uint8_t r, uint8_t g, uint8_t b);
  
  static Color from_hsv(uint8_t h, uint8_t s, uint8_t v);
  
  constexpr uint8_t get_r() const { return r; }
  constexpr uint8_t get_g() const { return g; }
  constexpr uint8_t get_b() const { return b; }
  
private:
  uint8_t r, g, b;
};
