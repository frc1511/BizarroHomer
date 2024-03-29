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
  
  //
  // Constructs a color from a 24-bit RGB hex value.
  //
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

class ColorPalette {
public:
  // Colors!!! From WPILib 'Colors.h'.
  enum : uint32_t {
    ALICEBLUE             = 0xF0F8FF,
    ANTIQUEWHITE          = 0xFAEBD7,
    AQUA                  = 0x00FFFF,
    AQUAMARINE            = 0x7FFFD4,
    AZURE                 = 0xF0FFFF,
    BEIGE                 = 0xF5F5DC,
    BISQUE                = 0xFFE4C4,
    BLACK                 = 0x000000,
    BLANCHEDALMOND        = 0xFFEBCD,
    BLUE                  = 0x0000FF,
    BLUEVIOLET            = 0x8A2BE2,
    BROWN                 = 0xA52A2A,
    BURLYWOOD             = 0xDEB887,
    CADETBLUE             = 0x5F9EA0,
    CHARTREUSE            = 0x7FFF00,
    CHOCOLATE             = 0xD2691E,
    CORAL                 = 0xFF7F50,
    CORNFLOWERBLUE        = 0x6495ED,
    CORNSILK              = 0xFFF8DC,
    CRIMSON               = 0xDC143C,
    CYAN                  = 0x00FFFF,
    DARKBLUE              = 0x00008B,
    DARKCYAN              = 0x008B8B,
    DARKGOLDENROD         = 0xB8860B,
    DARKGRAY              = 0xA9A9A9,
    DARKGREEN             = 0x006400,
    DARKKHAKI             = 0xBDB76B,
    DARKMAGENTA           = 0x8B008B,
    DARKOLIVEGREEN        = 0x556B2F,
    DARKORANGE            = 0xFF8C00,
    DARKORCHID            = 0x9932CC,
    DARKRED               = 0x8B0000,
    DARKSALMON            = 0xE9967A,
    DARKSEAGREEN          = 0x8FBC8F,
    DARKSLATEBLUE         = 0x483D8B,
    DARKSLATEGRAY         = 0x2F4F4F,
    DARKTURQUOISE         = 0x00CED1,
    DARKVIOLET            = 0x9400D3,
    DEEPPINK              = 0xFF1493,
    DEEPSKYBLUE           = 0x00BFFF,
    DIMGRAY               = 0x696969,
    DODGERBLUE            = 0x1E90FF,
    FIREBRICK             = 0xB22222,
    FLORALWHITE           = 0xFFFAF0,
    FORESTGREEN           = 0x228B22,
    FUCHSIA               = 0xFF00FF,
    GAINSBORO             = 0xDCDCDC,
    GHOSTWHITE            = 0xF8F8FF,
    GOLD                  = 0xFFD700,
    GOLDENROD             = 0xDAA520,
    GRAY                  = 0x808080,
    GREEN                 = 0x008000,
    GREENYELLOW           = 0xADFF2F,
    HOMEDEPOT             = 0xFF1B00,
    HONEYDEW              = 0xF0FFF0,
    HOTPINK               = 0xFF69B4,
    INDIANRED             = 0xCD5C5C,
    INDIGO                = 0x4B0082,
    IVORY                 = 0xFFFFF0,
    KHAKI                 = 0xF0E68C,
    LAVENDER              = 0xE6E6FA,
    LAVENDERBLUSH         = 0xFFF0F5,
    LAWNGREEN             = 0x7CFC00,
    LEMONCHIFFON          = 0xFFFACD,
    LIGHTBLUE             = 0xADD8E6,
    LIGHTCORAL            = 0xF08080,
    LIGHTCYAN             = 0xE0FFFF,
    LIGHTGOLDENRODYELLOW  = 0xFAFAD2,
    LIGHTGRAY             = 0xD3D3D3,
    LIGHTGREEN            = 0x90EE90,
    LIGHTPINK             = 0xFFB6C1,
    LIGHTSALMON           = 0xFFA07A,
    LIGHTSEAGREEN         = 0x20B2AA,
    LIGHTSKYBLUE          = 0x87CEFA,
    LIGHTSLATEGRAY        = 0x778899,
    LIGHTSTEELBLUE        = 0xB0C4DE,
    LIGHTYELLOW           = 0xFFFFE0,
    LIME                  = 0x00FF00,
    LIMEGREEN             = 0x32CD32,
    LINEN                 = 0xFAF0E6,
    MAGENTA               = 0xFF00FF,
    MAROON                = 0x800000,
    MEDIUMAQUAMARINE      = 0x66CDAA,
    MEDIUMBLUE            = 0x0000CD,
    MEDIUMORCHID          = 0xBA55D3,
    MEDIUMPURPLE          = 0x9370DB,
    MEDIUMSEAGREEN        = 0x3CB371,
    MEDIUMSLATEBLUE       = 0x7B68EE,
    MEDIUMSPRINGGREEN     = 0x00FA9A,
    MEDIUMTURQUOISE       = 0x48D1CC,
    MEDIUMVIOLETRED       = 0xC71585,
    MIDNIGHTBLUE          = 0x191970,
    MINTCREAM             = 0xF5FFFA,
    MISTYROSE             = 0xFFE4E1,
    MOCCASIN              = 0xFFE4B5,
    NAVAJOWHITE           = 0xFFDEAD,
    NAVY                  = 0x000080,
    OLDLACE               = 0xFDF5E6,
    OLIVE                 = 0x808000,
    OLIVEDRAB             = 0x6B8E23,
    ORANGE                = 0xFFA500,
    ORANGERED             = 0xFF4500,
    ORCHID                = 0xDA70D6,
    PALEGOLDENROD         = 0xEEE8AA,
    PALEGREEN             = 0x98FB98,
    PALETURQUOISE         = 0xAFEEEE,
    PALEVIOLETRED         = 0xDB7093,
    PAPAYAWHIP            = 0xFFEFD5,
    PEACHPUFF             = 0xFFDAB9,
    PERU                  = 0xCD853F,
    PINK                  = 0xFFC0CB,
    PLUM                  = 0xDDA0DD,
    POWDERBLUE            = 0xB0E0E6,
    PURPLE                = 0x800080,
    RED                   = 0xFF0000,
    ROSYBROWN             = 0xBC8F8F,
    ROYALBLUE             = 0x4169E1,
    SADDLEBROWN           = 0x8B4513,
    SALMON                = 0xFA8072,
    SANDYBROWN            = 0xF4A460,
    SEAGREEN              = 0x2E8B57,
    SEASHELL              = 0xFFF5EE,
    SIENNA                = 0xA0522D,
    SILVER                = 0xC0C0C0,
    SKYBLUE               = 0x87CEEB,
    SLATEBLUE             = 0x6A5ACD,
    SLATEGRAY             = 0x708090,
    SNOW                  = 0xFFFAFA,
    SPRINGGREEN           = 0x00FF7F,
    STEELBLUE             = 0x4682B4,
    TAN                   = 0xD2B48C,
    TEAL                  = 0x008080,
    THISTLE               = 0xD8BFD8,
    TOMATO                = 0xFF6347,
    TURQUOISE             = 0x40E0D0,
    VIOLET                = 0xEE82EE,
    WHEAT                 = 0xF5DEB3,
    WHITE                 = 0xFFFFFF,
    WHITESMOKE            = 0xF5F5F5,
    YELLOW                = 0xFFFF00,
    YELLOWGREEN           = 0x9ACD32,
  };
};
