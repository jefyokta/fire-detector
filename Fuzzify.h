#ifndef FUZZIFY_H
#define FUZZIFY_H

struct Membership {
  float low;
  float medium;
  float high;
};
class Fuzzify {
protected:
  int lowRange[2];
  int mediumRange[2];
  int highRange[2];
  virtual float getValue() = 0;
  float down(const float x, const float bb, const float ba) {
    if ((ba - bb) == 0) return 0.0f;
    return (ba - x) / (ba - bb);
  }
  float up(const float x, const float bb, const float ba) {
    if ((ba - bb) == 0) return 0.0f;
    return (x - bb) / (ba - bb);
  }
  virtual float getLowMembership(const float val) {

    if (val < this->mediumRange[0]) {
      return 1.0;
    }
    if (val > this->lowRange[1]) {
      return 0.0;
    }

    return this->down(val, this->mediumRange[0], this->lowRange[1]);
  }

  virtual float getHighMembership(const float val) {

    if (val < this->highRange[0]) {
      return 0.0;
    }
    if (val > this->highRange[1] || val > this->mediumRange[1]) {
      return 1.0;
    }

    return this->up(val, this->highRange[0], this->mediumRange[1]);
  }

  virtual float getMediumMembership(const float val) {
    if (!this->inMid(val)) return 0.0;
    if (!this->inLow(val) && !this->inHigh(val)) {
      return 1.0;
    }
    if (!this->inHigh(val) && this->inLow(val)) {
      return this->up(val, this->mediumRange[0], this->lowRange[1]);
    }
    return this->down(val, this->highRange[0], this->mediumRange[1]);
  }
  bool inMid(const float val) {
    return (val >= this->mediumRange[0] && val <= this->mediumRange[1]);
  }

  bool inLow(const float val) {
    return val <= this->lowRange[1];
  }

  bool inHigh(const float val) {
    return val >= this->highRange[0];
  }

public:
  Membership fuzzification() {}
};
#endif