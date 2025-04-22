#include "Fuzzy.h"
#include "Fire.h"
#include "Gas.h"
#include "Temperature.h"
#include "Arduino.h"
Fuzzy::Fuzzy(float fire, float temperature, int gas, float ro)
  : fire(fire), gas(gas, ro), temp(temperature), value(1) {}

bool Fuzzy::isSafe() {
  return this->value == 1;
}
bool Fuzzy::isDangerous() {
  return this->value == 3;
}
bool Fuzzy::isModerate() {
  return this->value == 2;
}
bool Fuzzy::isVeryDangerous() {
  return this->value == 4;
}
bool Fuzzy::shouldWarn() {
  return this->value >= 3;
}

bool Fuzzy::isSevere() {
  return (fire.isVeryNear() + gas.isVeryConcentrated() + temp.isVeryHot()) >= 2;
}

bool Fuzzy::isModerateRisk() {
  return (fire.isNear() + gas.isConcentrated() + temp.isHot()) >= 2;
}

Fuzzy& Fuzzy::defuzzification() {
  int level = 1;

  if (fire.isVeryNear() && gas.isVeryConcentrated() && temp.isVeryHot()) {
    level = 4;
  } else if (isSevere()) {
    level = 4;
  } else if (fire.isVeryNear() || gas.isVeryConcentrated() || temp.isVeryHot()) {
    level = 3;
  } else if (fire.isNear() && gas.isConcentrated() && temp.isHot()) {
    level = 3;
  } else if (isModerateRisk()) {
    level = 3;
  } else if (fire.isNear() || gas.isConcentrated() || temp.isHot()) {
    level = 2;
  } else if (fire.isFar() && gas.isThin() && temp.isNormal()) {
    level = 2;
  } else if ((fire.isFar() && gas.isThin()) || (fire.isFar() && temp.isNormal()) || (gas.isThin() && temp.isNormal())) {
    level = 2;
  } else {
    level = 1;
  }

  this->value = level;
  return *this;
}

void Fuzzy::log() {

  Serial.print("Fire :" + String(this->fire.getVoltage()) + ", Gas : " + String(this->gas.getAdc()) + ", Temperature : " + String(this->temp.getTemperature()));
}

float Fuzzy::getPpm(){

  return this->gas.getPpm();

}