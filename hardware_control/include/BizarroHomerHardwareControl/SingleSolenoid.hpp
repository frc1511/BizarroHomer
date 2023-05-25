#pragma once

class SingleSolenoid {
public:
  SingleSolenoid(int channel);
  ~SingleSolenoid();

  void set(bool direction);

private:
  int channel;
};
