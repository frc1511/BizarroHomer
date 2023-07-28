#pragma once

class HardwareComponent {
public:
  virtual ~HardwareComponent() = default;
  
  virtual void stop() { }
  virtual void set_enabled(bool enabled) { }
  
protected:
  HardwareComponent() = default;
};
