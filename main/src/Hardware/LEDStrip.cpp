#include <BizarroHomer/Hardware/LEDStrip.hpp>
#include <fmt/core.h>
#include <cassert>

thunder::LEDStrip::LEDStrip(uint8_t channel, uint32_t led_num)
: m_channel(channel), m_led_num(led_num) {
  
  m_led_strip.freq = WS2811_TARGET_FREQ;
  m_led_strip.dmanum = 10;
  
  m_led_strip.channel[0].gpionum = m_channel;
  m_led_strip.channel[0].count = m_led_num;
  m_led_strip.channel[0].invert = 0;
  m_led_strip.channel[0].brightness = 0xFF;
  m_led_strip.channel[0].strip_type = WS2812_STRIP; // WS2811_STRIP_GRB
  
  m_led_strip.channel[1].gpionum = 0;
  m_led_strip.channel[1].count = 0;
  m_led_strip.channel[1].invert = 0;
  m_led_strip.channel[1].brightness = 0x00;
  
  ws2811_return_t ret = ws2811_init(&m_led_strip);
  if (ret != WS2811_SUCCESS) {
    fmt::print(stderr, "Failed to init WS2812 LED strip at GPIO pin {}.\n", m_channel);
    return;
  }
  
  m_init = true;
}

thunder::LEDStrip::~LEDStrip() {
  if (!m_init) {
    return;
  }
  
  ws2811_fini(&m_led_strip);
}

void thunder::LEDStrip::set_data(std::span<const Color> colors) {
  if (!m_init) {
    return;
  }
  
  std::size_t colors_num = colors.size();
  assert(m_led_num == colors_num && "Data length does not match the configured number of LEDs");
  
  for (std::size_t i = 0; i < colors_num; ++i) {
    const Color& color = colors[i];
    uint32_t output_color = 0x00;
    
    output_color &= color.G() << 16;
    output_color &= color.R() << 8;
    output_color &= color.B();
    
    m_led_strip.channel[0].leds[i] = output_color;
  }
  
  ws2811_render(&m_led_strip);
}
