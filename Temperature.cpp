#include "Temperature.h"


Temperature::Temperature(float temp) {
  this->temp = temp;
}

bool Temperature::isCold() {
  return this->temp <= 18.0;
}
bool Temperature::isNormal() {
  return this->temp > 18.0 && this->temp <= 35.0;
}
bool Temperature::isHot() {
  return this->temp > 35.1 && this->temp < 40.0;
}

bool Temperature::isVeryHot() {
  return this->temp >= 40.0;
}
float Temperature::getTemperature() {
  return this->temp;
}