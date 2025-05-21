#include "Gas.h"
#include <Arduino.h>

Gas::Gas(int adc, float ro) {
  this->adc = adc;
  this->ro = ro;
  this->lowRange[0] = 0;
  this->lowRange[1] = 100;
  this->mediumRange[0] = 50;
  this->mediumRange[1] = 300;
  this->highRange[0] = 250;
  this->highRange[1] = 500;
}


bool Gas::isThin() {
  return this->adc > 800 && this->adc < 1000;

  // return this->getPpm() >= 20 && this->getPpm() < 100;
}
bool Gas::isAlmostUndetected() {
  return this->adc <= 800;

  // return this->getPpm() < 20;
}
bool Gas::isConcentrated() {
  return this->adc >= 1000 && this->adc < 3100;

  // return this->getPpm() >= 100 && this->getPpm() < 300;
}
bool Gas::isVeryConcentrated() {
  return this->adc >= 3100;

  // return this->getPpm() >=300;
}
int Gas::getAdc() {
  return this->adc;
}

void Gas::logRo() {
  Serial.print("Ro: ");
  Serial.println(this->ro);
}

float Gas::getPpm() {
  float vout = (this->adc * (Gas::volt / Gas::maxAdc));
  float rs = Gas::rl * ((Gas::volt / vout) - 1);
  float ratio = rs / this->ro;
  float ppm = pow(10, (log10(ratio) - 1.85) / -0.63);
  return ppm;
}

float Gas::getValue() {
  return this->getPpm();
}
