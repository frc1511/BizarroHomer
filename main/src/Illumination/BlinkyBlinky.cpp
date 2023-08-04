#include <BizarroHomer/Illumination/BlinkyBlinky.hpp>

#define LOOP_TIME 60.0 // ms
#define KITT_TIME 800.0 // ms

#define KITT_LOOPS (KITT_TIME / LOOP_TIME)

BlinkyBlinky::BlinkyBlinky() = default;
BlinkyBlinky::~BlinkyBlinky() = default;

void BlinkyBlinky::process() {
  switch (m_mode) {
    case LEDMode::OFF:
      set_color(ColorPalette::BLACK);
      break;
    case LEDMode::RAINBOW:
      rainbow();
      break;
    case LEDMode::KITT:
      kitt();
      break;
    case LEDMode::HOME_DEPOT:
      set_color(ColorPalette::HOMEDEPOT);
      break;
    case LEDMode::PRESSURE:
      pressure();
      break;
  }
  
  m_strip.set_data(m_buffer);
}

void BlinkyBlinky::set_mode(LEDMode mode) {
  m_mode = mode;
}

void BlinkyBlinky::set_pressure(double percent) {
  m_pressure_percent = percent;
}

void BlinkyBlinky::set_color(Color color) {
  for (std::size_t i = 0; i < m_buffer.size(); ++i) {
    m_buffer.at(i) = color;
  }
}

void BlinkyBlinky::rainbow() {
  for (std::size_t i = 0; i < LED_NUM; ++i) {
    const uint8_t pixel_hue = (m_first_pixel_hue + (i * 180 / LED_NUM)) % 180;
    
    m_buffer.at(i) = Color::from_hsv(pixel_hue, 255, 128);
  }
  
  m_first_pixel_hue += 3;
  m_first_pixel_hue %= 180;
}

void BlinkyBlinky::kitt() {
  m_kitt_iter += m_kitt_dir;
  
  if (m_kitt_iter >= KITT_LOOPS || m_kitt_iter <= 0) {
    m_kitt_dir = -m_kitt_dir;
  }
  
  std::size_t pixel = (m_kitt_iter / KITT_LOOPS) * (LED_NUM - 1);
  
  double fade_range = LED_NUM * 0.3;
  
  // Clear the buffer.
  set_color(ColorPalette::BLACK);

  uint8_t hue_offset = 0;
  
  // Fade up.
  for (std::size_t i = pixel; i <= (pixel + fade_range > LED_NUM - 1 ? LED_NUM - 1 : pixel + fade_range); ++i) {
    double percent = static_cast<double>(i - pixel) / fade_range;
    
    double value = (1 / percent) * 128;
    
    m_buffer.at(i) = Color::from_hsv(hue_offset + percent * 5, value * 0.5 + 192, value);
  }
  // Fade down.
  for (std::size_t i = pixel; i >= (pixel - fade_range < 0 ? 0 : pixel - fade_range); --i) {
    double percent = static_cast<double>(pixel - i) / fade_range;
    
    double value = (1 / percent) * 128;
    
    m_buffer.at(i) = Color::from_hsv(hue_offset + percent * 5, value * 0.5 + 192, value);
  }
  // Middle.
  m_buffer.at(pixel) = Color::from_hsv(hue_offset, 255, 255);
}

void BlinkyBlinky::pressure() {
  std::size_t cutoff = LED_NUM * m_pressure_percent;
  // Always at least two pixels.
  if (cutoff < 2) cutoff = 2;
  
  if (cutoff == LED_NUM || m_pressure_percent >= 1.0) {
    rainbow();
    return;
  }
  
  for (std::size_t i = 0; i < LED_NUM; ++i) {
    if (i > cutoff) {
      m_buffer.at(i) = ColorPalette::BLACK;
    }
    else {
      m_buffer.at(i) = ColorPalette::HOMEDEPOT;
    }
  }
}

void BlinkyBlinky::send_feedback(DashboardServer* dashboard) {
  const char* mode_str = "";
  
  switch (m_mode) {
    case LEDMode::OFF:
      mode_str = "Off";
      break;
    case LEDMode::RAINBOW:
      mode_str = "Rainbow";
      break;
    case LEDMode::KITT:
      mode_str = "Kitt";
      break;
    case LEDMode::HOME_DEPOT:
      mode_str = "Home Depot";
      break;
    case LEDMode::PRESSURE:
      mode_str = "Pressure";
      break;
  }
  
  dashboard->update_value("BlinkyBlinky_LEDMode", mode_str);
}
