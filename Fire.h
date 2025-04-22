#ifndef FIRE_H
#define FIRE_H


class Fire {
private:
  float voltage;
public:
  Fire(float voltage);
  bool isNear();
  bool isFar();
  bool isUndetected();
  bool isVeryNear();
  float getVoltage();
};


#endif