#pragma once

class PWM_SparkMax {
public:
  PWM_SparkMax(int channel);
  ~PWM_SparkMax();
  
  virtual void set_percent(double percent);
  
private:
  int channel;
};
