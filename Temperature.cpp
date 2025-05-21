#include "Temperature.h"


Temperature::Temperature(float temp) {
  this->temp = temp;
  this->lowRange[0] = 0;
  this->lowRange[1] = 20;
  this->mediumRange[0] = 18;
  this->mediumRange[1] = 38;
  this->highRange[0] = 30;
  this->highRange[1] = 50;
}

bool Temperature::isCold() {
  return this->temp <= 18.0;
}
bool Temperature::isNormal() {
  return this->temp > 18.0 && this->temp <= 38.0;
}
bool Temperature::isHot() {
  return this->temp > 38 && this->temp < 40.0;
}

bool Temperature::isVeryHot() {
  return this->temp >= 40.0;
}
float Temperature::getTemperature() {
  return this->temp;
}
float Temperature::getValue() {
  return this->getTemperature();
}