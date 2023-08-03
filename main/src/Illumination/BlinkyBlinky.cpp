#include <BizarroHomer/Illumination/BlinkyBlinky.hpp>

BlinkyBlinky::BlinkyBlinky() {

}

BlinkyBlinky::~BlinkyBlinky() {

}

void BlinkyBlinky::process() {
  switch (m_mode) {
    case LEDMode::OFF:
      set_color(ColorPreset::BLACK);
      break;
    case LEDMode::RAINBOW:
      // TODO:
      break;
    case LEDMode::KNIGHT_RIDER:
      // TODO:
      break;
    case LEDMode::HOME_DEPOT:
      // TODO:
      break;
    case LEDMode::PRESSURE:
      // TODO:
      break;
  }
  
  m_strip.set_data(m_buffer);
}

void BlinkyBlinky::send_feedback(DashboardServer* dashboard) {
  
}

void BlinkyBlinky::set_mode(LEDMode mode) {
  m_mode = mode;
}

void BlinkyBlinky::set_pressure(double percent) {
  m_mode = LEDMode::PRESSURE;
  m_pressure_percent = percent;
}

void BlinkyBlinky::set_color(Color color) {
  for (std::size_t i = 0; i < m_buffer.size(); ++i) {
    m_buffer.at(i) = color;
  }
}
