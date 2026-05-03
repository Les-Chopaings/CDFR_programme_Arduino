#include "servoPlus.h"

ServoPlus::ServoPlus(int pin, int defaultAngle, int mapMin, int mapMax){
  sMapMin = mapMin;
  sMapMax = mapMax;
  sDefaultAngle = defaultAngle;
  sPin = pin;
}


void ServoPlus::setup(){
  servo.attach(sPin);
  write(sDefaultAngle);
}

void ServoPlus::write(int angle, bool skipCheck){
  //perfect angle
  if (sCheckFunc != nullptr) {
    if(!skipCheck){
      angle = sCheckFunc(angle);
    }
  }
  sCurrentAngle = angle;
  //modifie angle
  angle = constrain(angle,0,180);
  angle = map(angle,0,180,sMapMin,sMapMax);
  servo.write(angle);
}

int ServoPlus::get(){
  return sCurrentAngle;
}

void ServoPlus::enable(bool b){
  if(b){
    servo.attach(sPin);
    write(sDefaultAngle);
  }
  else{
    servo.detach();
  }
}

ServoPlus::~ServoPlus(){
}

void ServoPlus::setFunctionChecker(int (*check)(int)){
  sCheckFunc = check;
}