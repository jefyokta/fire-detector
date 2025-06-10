#include <vector>
#pragma once
#define TSUKAMOTO_H

class Tsukamoto
{
private:
  int safeRange[2] = {0, 80};
  int dangerRange[2] = {20, 100};

  std::vector<float> safe;
  std::vector<float> danger;
  float getLingusFromLow(const float m, const float a, const float b)
  {
    return b - m * (b - a);
  }

  float getLingusFromHigh(const float m, const float a, const float b)
  {
    return m * (b - a) + a;
  }

public:
  Tsukamoto(std::vector<float> safe, std::vector<float> danger)
  {
    this->safe = safe;
    this->danger = danger;
  }

  float defuzzy()
  {
    float numerator = 0;
    float denominator = 0;

    for (size_t i = 0; i < danger.size(); i++)
    {
      float z = danger[i] * (this->safeRange[1] - this->dangerRange[0]) + this->dangerRange[0];
      numerator += z * danger[i];
      denominator += danger[i];
    }

    for (size_t i = 0; i < safe.size(); i++)
    {
      float z = this->safeRange[1] - ((this->safeRange[1] - this->dangerRange[0]) * safe[i]);
      numerator += z * safe[i];
      denominator += safe[i];
    }

    if (denominator == 0) return 0;
    return numerator / denominator;
  }
};
