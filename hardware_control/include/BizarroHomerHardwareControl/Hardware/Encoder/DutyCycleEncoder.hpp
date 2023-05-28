#pragma once

class DutyCycleEncoder {
public:
  DutyCycleEncoder(int channel);
  ~DutyCycleEncoder();

  /**
   * @brief Returns the position of the encoder.
   *
   * @return The position of the encoder.
   */
  double get();

private:
  int channel;
};
