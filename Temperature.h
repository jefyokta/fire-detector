#ifndef TEMPERATURE_H
#define TEMPERATURE_H

class Temperature {
private:
  float temp;
public:
  Temperature(float temp);

  bool isCold();
  bool isHot();
  bool isVeryHot();
  bool isNormal();
  float getTemperature();
};
#endif