#pragma once

#include <BizarroHomer/Basic/Mechanism.hpp>
#include <BizarroHomer/Hardware/LEDStrip.hpp>
#include <BizarroHomer/Hardware/IOMap.hpp>
#include <array>

#define LED_NUM 40

class BlinkyBlinky : public Mechanism {
public:
  BlinkyBlinky();
  ~BlinkyBlinky();
  
  void process() override;
  void send_feedback(DashboardServer* dashboard) override;
  
  enum class LEDMode {
    OFF,
    RAINBOW,
    KITT,
    HOME_DEPOT,
    PRESSURE,
  };
  
  //
  // Sets the mode of the LED strip.
  //
  void set_mode(LEDMode mode);
  
  //
  // Sets the LED mode to PRESSURE and sets the pressure scale percent (0-1).
  //
  void set_pressure(double percent);
  
private:
  thunder::LEDStrip m_strip { GPIO_BLINKY_BLINKY, LED_NUM };
  std::array<Color, LED_NUM> m_buffer;
  
  LEDMode m_mode = LEDMode::OFF;
  double m_pressure_percent = 0.0;
  
  void set_color(Color color);
  
  void rainbow();
  void kitt();
  void pressure();
  
  uint8_t m_first_pixel_hue = 0;
  int m_kitt_iter = 0;
  int m_kitt_dir = 1;
};
