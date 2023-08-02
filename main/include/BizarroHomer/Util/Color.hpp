#pragma once

#include <cstdint>

//
// Represents a RGB color.
//
class Color {
public:
  //
  // Constructs a color from the given RGB values.
  //
  Color(uint8_t r, uint8_t g, uint8_t b);
  
  //
  // Constructs a color from the given HSV values.
  //
  // The hue parameter is in the range [0, 180].
  // The saturation and value parameters are in the range [0, 255].
  //
  static Color from_hsv(uint8_t h, uint8_t s, uint8_t v);
  
  constexpr uint8_t get_r() const { return r; }
  constexpr uint8_t get_g() const { return g; }
  constexpr uint8_t get_b() const { return b; }
  
private:
  uint8_t r, g, b;
};
