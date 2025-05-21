#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#include "Fuzzify.h"
class Temperature : public Fuzzify {
private:
  float temp;
public:
  Temperature(float temp);

  bool isCold();
  bool isHot();
  bool isVeryHot();
  bool isNormal();
  float getTemperature();
protected:
  float getValue() override;
};
#endif