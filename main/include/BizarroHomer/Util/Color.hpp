#pragma once

#include <cstdint>

//
// Represents a RGB color.
//
class Color {
public:
  Color() = default;
  
  //
  // Constructs a color from the given RGB values.
  //
  constexpr Color(uint8_t r, uint8_t g, uint8_t b)
  : m_r(r), m_g(g), m_b(b) { }

  constexpr Color(uint32_t rgb_hex) {
    m_r = static_cast<uint8_t>((rgb_hex & 0xFF0000) >> 16);
    m_g = static_cast<uint8_t>((rgb_hex & 0x00FF00) >> 8);
    m_b = static_cast<uint8_t> (rgb_hex & 0x0000FF);
  }
  
  //
  // Constructs a color from the given HSV values.
  //
  // The hue parameter is in the range [0, 180].
  // The saturation and value parameters are in the range [0, 255].
  //
  static Color from_hsv(uint8_t h, uint8_t s, uint8_t v);
  
  constexpr uint8_t R() const { return m_r; }
  constexpr uint8_t G() const { return m_g; }
  constexpr uint8_t B() const { return m_b; }
  
private:
  uint8_t m_r = 0,
          m_g = 0,
          m_b = 0;
};

class ColorPreset {
public:
  enum : uint32_t {
    BLACK = 0x00'00'00,
    WHITE = 0xFF'FF'FF,
  };
};
