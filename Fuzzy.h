#include "Fire.h"
#include "Gas.h"
#include "Temperature.h"
#include "Fuzzify.h"
#include "vector"
#ifndef FUZZY_H
#define FUZZY_H


enum class Condition {
  Danger,
  Safe,
};

struct Rule {
  float f;
  float t;
  float g;
  Condition c;
};

class Fuzzy {
private:
  int value;
  float safeVal;
  float dangerVal;
  float crispOut;
  Gas gas;
  Fire fire;
  Temperature temp;
  bool warn = false;
  bool isModerateRisk();
  bool isSevere();
  void inference();
  float getMin3(const float a, const float b, const float c);
public:
  Fuzzy(float fire, float temperature, int gas, float ro);
  float getPpm();
  float getCrispOut();
  bool isSafe();
  bool isDangerous();
  bool isVeryDangerous();
  bool isModerate();
  bool shouldWarn();
  void log();
  Fuzzy& defuzzification();
};



#endif
