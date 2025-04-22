#ifndef GAS_H
#define GAS_H
#include <Arduino.h>

class Gas {
private:
  int adc;
  float ro;
  static constexpr float rl = 10.0;
  static constexpr float volt = 3.3;
  static constexpr float maxAdc = 4095.0;
public:
  Gas(int adc, float ro);
  bool isConcentrated();
  bool isAlmostUndetected();
  bool isVeryConcentrated();
  bool isThin();
  int getAdc();
  void logRo();
  float getPpm();
};


#endif