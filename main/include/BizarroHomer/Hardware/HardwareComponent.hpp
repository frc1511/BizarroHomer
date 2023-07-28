#pragma once

class HardwareComponent {
public:
  virtual ~HardwareComponent() = default;
  
  virtual void stop() { }
  
protected:
  HardwareComponent() = default;
};
