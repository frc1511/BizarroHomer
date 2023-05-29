#pragma once

class PWMSparkMax {
public:
  PWMSparkMax(int channel);
  ~PWMSparkMax();
  
  virtual void set_percent(double percent);
  
private:
  int channel;
};
