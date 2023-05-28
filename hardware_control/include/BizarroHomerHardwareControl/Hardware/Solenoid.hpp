#pragma once

class Solenoid {
public:
  Solenoid(int channel);
  ~Solenoid();

  void set(bool direction);

private:
  int channel;
};
