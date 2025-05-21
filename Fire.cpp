#include "Fire.h";


Fire::Fire(float voltage) {
  this->voltage = voltage;
  this->lowRange[0] = 0;
  this->lowRange[1] = 1.4;
  this->mediumRange[0] = 1;
  this->mediumRange[1] = 2;
  this->highRange[0] = 1.7;
  this->highRange[1] = 4;
}
bool Fire::isFar() {
  return this->voltage >= 2.5 && this->voltage <= 3.2;
}

bool Fire::isNear() {
  return this->voltage < 2.5 && this->voltage >= 1.5;
}
bool Fire::isVeryNear() {

  return this->voltage < 1.5;
}
bool Fire::isUndetected() {
  return this->voltage >= 3.2;
}
float Fire::getVoltage() {
  return this->voltage;
};

float Fire::getValue() {
  return this->voltage;
}
