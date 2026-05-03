#pragma once
#include "Servo.h"
#include <Arduino.h>

class ServoPlus
{
private:
  Servo servo;
  int sPin;
  int sDefaultAngle;
  int sCurrentAngle;
  int sMapMin;
  int sMapMax;
  int (*sCheckFunc)(int) = nullptr;
public:
  ServoPlus(int pin, int defaultAngle = 0, int mapMin = 0, int mapMax = 180);
  void setup();
  void write(int angle, bool skipCheck = false);
  int get();
  void enable(bool b);
  void setFunctionChecker(int (*check)(int));
  ~ServoPlus();
};
