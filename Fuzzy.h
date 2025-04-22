#include "Fire.h"
#include "Gas.h"
#include "Temperature.h"
#ifndef FUZZY_H
#define FUZZY_H

class Fuzzy {
private:
  int value;
  Gas gas;
  Fire fire;
  Temperature temp;
  bool isModerateRisk();
  bool isSevere();
public:
  Fuzzy(float fire, float temperature, int gas, float ro);
  float getPpm();
  bool isSafe();
  bool isDangerous();
  bool isVeryDangerous();
  bool isModerate();
  bool shouldWarn();
  void log();
  Fuzzy& defuzzification();
};



#endif
