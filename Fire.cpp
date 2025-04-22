#include "Fire.h";


Fire::Fire(float voltage) {
  this->voltage = voltage;
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

