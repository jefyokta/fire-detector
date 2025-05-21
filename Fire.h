#ifndef FIRE_H
#define FIRE_H
#include "Fuzzify.h"

class Fire : public Fuzzify {
private:
  float voltage;
public:
  Fire(float voltage);
  bool isNear();
  bool isFar();
  bool isUndetected();
  bool isVeryNear();
  float getVoltage();
protected:
  float getValue() override;
};


#endif