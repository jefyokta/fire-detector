#pragma once
#define TSUKAMOTO_H

class Tsukamoto {
private:
  int safeRange[2] = { 0, 85 };
  int dangerRange[2] = { 80, 100 };

  float safe;
  float danger;

public:
  Tsukamoto(float safe, float danger) {
    this->safe = safe;
    this->danger = danger;
  }

  float defuzzy() {
    float zSafe = this->getLingusFromLow(this->safe, this->safeRange[0], this->safeRange[1]);
    float zDanger = this->getLingusFromHigh(this->danger, this->dangerRange[0], this->dangerRange[1]);

    float numerator = (zSafe * this->safe) + (zDanger * this->danger);
    float denominator = this->safe + this->danger;

    if (denominator == 0) return 0;
    return numerator / denominator;
  }

  float getLingusFromLow(const float m, const float a, const float b) {
    return b - m * (b - a);
  }

  float getLingusFromHigh(const float m, const float a, const float b) {
    return m * (b - a) + a;
  }
};
