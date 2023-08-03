#include <BizarroHomer/Util/Color.hpp>

Color Color::from_hsv(uint8_t h, uint8_t s, uint8_t v) {
  int chroma = (s * v) >> 8;
  int region = (h / 30) % 6;
  int rem = static_cast<int>((h % 30) * (255 / 30.0));
  
  int m = v - chroma;
  
  int x = (chroma * rem) >> 8;
  
  switch (region) {
    case 0: return Color(v, x + m, m);
    case 1: return Color(v - x, v, m);
    case 2: return Color(m, v, x + m);
    case 3: return Color(m, v - x, v);
    case 4: return Color(x + m, m, v);
    default: return Color(v, m, v - x);
  }
}
