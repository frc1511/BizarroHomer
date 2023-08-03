#pragma once

#include <BizarroHomer/Hardware/HardwareComponent.hpp>
#include <BizarroHomer/Util/Color.hpp>
#include <span>
#include <cstdint>
#include <ws2811.h>

namespace thunder {

//
// Represents a WS2812 LED strip connected to a GPIO pin on the robot.
//
class LEDStrip : public HardwareComponent {
public:
  LEDStrip(uint8_t channel, uint32_t led_num);
  ~LEDStrip();
  
  //
  // Sets the colors of the LED strip.
  //
  void set_data(std::span<const Color> colors);
  
private:
  uint8_t m_channel;
  ws2811_t m_led_strip;
  uint32_t m_led_num;
  
  bool m_init = false;
};

} // namespace thunder
