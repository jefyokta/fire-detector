#include <vector>
#include <algorithm>
#include "Fuzzy.h"
#include "Fire.h"
#include "Gas.h"
#include "Temperature.h"
#include "Arduino.h"
#include "Tsukamoto.h"
Fuzzy::Fuzzy(float fire, float temperature, int gas, float ro)
    : fire(fire), gas(gas, ro), temp(temperature), value(1) {}

bool Fuzzy::isSafe()
{
  return this->value == 1;
}
bool Fuzzy::isDangerous()
{
  return this->value == 3;
}
bool Fuzzy::isModerate()
{
  return this->value == 2;
}
bool Fuzzy::isVeryDangerous()
{
  return this->value == 4;
}
bool Fuzzy::shouldWarn()
{
  return this->warn;
}

bool Fuzzy::isSevere()
{
  return (fire.isVeryNear() + gas.isVeryConcentrated() + temp.isVeryHot()) >= 2;
}

bool Fuzzy::isModerateRisk()
{
  return (fire.isNear() + gas.isConcentrated() + temp.isHot()) >= 2;
}

void Fuzzy::inference()
{
  Membership fireM = this->fire.fuzzification();
  Membership tempM = this->temp.fuzzification();
  Membership gasM = this->gas.fuzzification();

  this->dangerVal.clear();
  this->safeVal.clear();
  Rule rules[] = {
      {fireM.high, tempM.low, gasM.low, Condition::Safe},
      {fireM.high, tempM.low, gasM.medium, Condition::Safe},
      {fireM.high, tempM.low, gasM.high, Condition::Danger},
      {fireM.high, tempM.medium, gasM.low, Condition::Safe},
      {fireM.high, tempM.medium, gasM.medium, Condition::Danger},
      {fireM.high, tempM.medium, gasM.high, Condition::Danger},
      {fireM.high, tempM.high, gasM.low, Condition::Danger},
      {fireM.high, tempM.high, gasM.medium, Condition::Danger},
      {fireM.high, tempM.high, gasM.high, Condition::Danger},

      {fireM.medium, tempM.low, gasM.low, Condition::Safe},
      {fireM.medium, tempM.low, gasM.medium, Condition::Danger},
      {fireM.medium, tempM.low, gasM.high, Condition::Danger},
      {fireM.medium, tempM.medium, gasM.low, Condition::Danger},
      {fireM.medium, tempM.medium, gasM.medium, Condition::Danger},
      {fireM.medium, tempM.medium, gasM.high, Condition::Danger},
      {fireM.medium, tempM.high, gasM.low, Condition::Danger},
      {fireM.medium, tempM.high, gasM.medium, Condition::Danger},
      {fireM.medium, tempM.high, gasM.high, Condition::Danger},

      {fireM.low, tempM.low, gasM.low, Condition::Danger},
      {fireM.low, tempM.low, gasM.medium, Condition::Danger},
      {fireM.low, tempM.low, gasM.high, Condition::Danger},
      {fireM.low, tempM.medium, gasM.low, Condition::Danger},
      {fireM.low, tempM.medium, gasM.medium, Condition::Danger},
      {fireM.low, tempM.medium, gasM.high, Condition::Danger},
      {fireM.low, tempM.high, gasM.low, Condition::Danger},
      {fireM.low, tempM.high, gasM.medium, Condition::Danger},
      {fireM.low, tempM.high, gasM.high, Condition::Danger}};

  for (Rule r : rules)
  {
    float res = this->getMin3(r.f, r.t, r.g);
    if (res != 0.0f)
    {
      if (r.c == Condition::Danger)
      {
        this->dangerVal.push_back(res);
      }
      else
      {
        this->safeVal.push_back(res);
      }
    }
  }
  // safe.
  // this->safeVal = safe.empty() ? 0.0 : *std::max_element(safe.begin(), safe.end());
  // this->dangerVal = danger.empty() ? 0.0 : *std::max_element(danger.begin(), danger.end());
}

Fuzzy &Fuzzy::defuzzification()
{
  this->inference();
  Tsukamoto ts(this->safeVal, this->dangerVal);
  if (this->safeVal < this->dangerVal)
  {
    this->warn = true;
  }
  else
  {
    this->warn = false;
  }
  this->crispOut = ts.defuzzy();
  return *this;
}

void Fuzzy::log()
{

  Serial.print("Fire :" + String(this->fire.getVoltage()) + ", Gas : " + String(this->gas.getAdc()) + ", Temperature : " + String(this->temp.getTemperature()));
}

float Fuzzy::getMin3(const float a, const float b, const float c)
{

  return std::min(a, std::min(b, c));
}

float Fuzzy::getPpm()
{

  return this->gas.getPpm();
}

float Fuzzy::getCrispOut()
{
  return this->crispOut;
}