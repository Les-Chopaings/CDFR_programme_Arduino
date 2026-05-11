#include <Arduino.h>
#include "config.h"
#include <Servo.h>
#include <AccelStepper.h>
#include <Wire.h>
#include "servoPlus.h"
#include "conversionArray.h"

ServoPlus* bascule;
ServoPlus* slider1;
ServoPlus* slider2;
ServoPlus* slider3;
ServoPlus* slider4;
ServoPlus* rotation1;
ServoPlus* rotation2;
ServoPlus* rotation3;
ServoPlus* rotation4;
ServoPlus* temp;

uint8_t onReceiveData[BUFFERONRECEIVESIZE];
uint8_t onRequestData[BUFFERONREQUESTSIZE];
int lenghtOnRequest;
void receiveEvent(int numBytes);
void requestEvent();

int countCommande;
int prevcountCommande;

AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP, STEPPER1_DIR);
int pomp_list[] = {POMPE1, POMPE2, POMPE3, POMPE4};
int toggle = 1;

int limitWithSlider1(int angle) {
  angle = (angle > slider2->get()) ? slider2->get() : angle;
  return angle;
}
int limitWithSlider2(int angle) {
  angle = (angle > slider3->get()) ? slider3->get() : angle;
  angle = (angle < slider1->get()) ? slider1->get() : angle;
  return angle;
}
int limitWithSlider3(int angle) {
  angle = (angle > slider4->get()) ? slider4->get() : angle;
  angle = (angle < slider2->get()) ? slider2->get() : angle;
  return angle;
}
int limitWithSlider4(int angle) {
  angle = (angle < slider3->get()) ? slider3->get() : angle;
  return angle;
}

int limitWithRotation1(int angle) {
  bool fullRotation = abs(angle - rotation1->get()) > 120;
  angle = ((slider1->get() <= 100 && slider2->get() == 180) || fullRotation==false) ? angle : rotation1->get();
  return angle;
}
int limitWithRotation2(int angle) {
  bool fullRotation = abs(angle - rotation2->get()) > 120;
  angle = ((slider1->get() == 0 && slider2->get() >= 40 && slider2->get() <= 100 && slider3->get() == 180) || fullRotation==false) ? angle : rotation2->get();
  return angle;
}
int limitWithRotation3(int angle) {
  bool fullRotation = abs(angle - rotation3->get()) > 120;
  angle = ((slider2->get() == 0 && slider3->get() >= 40 && slider3->get() <= 100 && slider4->get() == 180) || fullRotation==false) ? angle : rotation3->get();
  return angle;
}
int limitWithRotation4(int angle) {
  bool fullRotation = abs(angle - rotation4->get()) > 120;
  angle = ((slider3->get() == 0 && slider4->get() >= 40) || fullRotation==false) ? angle : rotation4->get();
  return angle;
}

void setup() {
  bascule = new ServoPlus(BASCULE,0,180,0);
  slider1 = new ServoPlus(SLIDER1,90);
  slider2 = new ServoPlus(SLIDER2,90);
  slider3 = new ServoPlus(SLIDER3,90);
  slider4 = new ServoPlus(SLIDER4,90);
  rotation1 = new ServoPlus(ROTATION1,0,0,165);
  rotation2 = new ServoPlus(ROTATION2,0,0,165);
  rotation3 = new ServoPlus(ROTATION3,0,10,180);
  rotation4 = new ServoPlus(ROTATION4,0,10,180);
  temp = new ServoPlus(TEMP,0,180,90);

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Start");
  pinMode(STEPPER_RESET, OUTPUT);
  digitalWrite(STEPPER_RESET, 1);
  pinMode(STEPPER_SLEEP, OUTPUT);
  digitalWrite(STEPPER_SLEEP, 1);
  pinMode(STEPPER1_ENABLE, OUTPUT);
  digitalWrite(STEPPER1_ENABLE, 0);
  stepper1.enableOutputs();
  stepper1.setAcceleration(5000);
  stepper1.setMaxSpeed(700);

  pinMode(POMPE1, OUTPUT);
  pinMode(POMPE2, OUTPUT);
  pinMode(POMPE3, OUTPUT);
  pinMode(POMPE4, OUTPUT);

  pinMode(SERVO1, INPUT_PULLUP);
  pinMode(SERVO2, INPUT_PULLUP);

  pinMode(MOSFET1, OUTPUT);
  pinMode(MOSFET2, OUTPUT);
  pinMode(MOSFET3, OUTPUT);
  pinMode(MOSFET4, OUTPUT);

  slider1->setup();
  slider2->setup();
  slider3->setup();
  slider4->setup();
  rotation1->setup();
  rotation2->setup();
  rotation3->setup();
  rotation4->setup();
  bascule->setup();
  temp->setup();

  slider1->setFunctionChecker(limitWithSlider1);
  slider2->setFunctionChecker(limitWithSlider2);
  slider3->setFunctionChecker(limitWithSlider3);
  slider4->setFunctionChecker(limitWithSlider4);
  rotation1->setFunctionChecker(limitWithRotation1);
  rotation2->setFunctionChecker(limitWithRotation2);
  rotation3->setFunctionChecker(limitWithRotation3);
  rotation4->setFunctionChecker(limitWithRotation4);

  slider1->write(90);
  slider2->write(90);
  slider3->write(90);
  slider4->write(90);
  rotation1->write(0);
  rotation2->write(0);
  rotation3->write(0);
  rotation4->write(0);
  bascule->write(0);
  temp->write(0);

  Wire.begin(100);
  Wire.setTimeout(1000);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  countCommande = 0;
  prevcountCommande = 0;

}

void loop() {
  stepper1.run();
  if(countCommande != prevcountCommande){
    Serial.println(countCommande);
    prevcountCommande = countCommande;
  }
}



void receiveEvent(int numBytes) {
  //Receive command and associate an action with the following table:
  //   commande   |   action
  //      1       |   Bascule (position)
  //      2       |   Slider 1 (position)
  //      3       |   Slider 2 (position)
  //      4       |   Slider 3 (position)
  //      5       |   Slider 4 (position)
  //      6       |   Rotation 1 (angle)
  //      7       |   Rotation 2 (angle)
  //      8       |   Rotation 3 (angle)
  //      9       |   Rotation 4 (angle)
  //     10       |   temperature
  //     11       |   Servo 1
  //     12       |   Servo 2
  //     20       |   Stepper 1 (moveTo position)
  //     21       |   Stepper 1 (Enable, bool)
  //     30       |   Pompe 1 (Enable, bool)
  //     31       |   Pompe 2 (Enable, bool)
  //     32       |   Pompe 3 (Enable, bool)
  //     33       |   Pompe 4 (Enable, bool)

  int i = 0;
  while (Wire.available() > 0) {
    if(i<BUFFERONRECEIVESIZE){
      onReceiveData[i] = Wire.read();
      i++;
    }
    else{
      Wire.read();
    }

  }

  int commande;
  arrayToParameter(onReceiveData,BUFFERONRECEIVESIZE,"1%u",&commande);
  switch (commande)
  {
  case 1 :{
    //BASCULE
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    bascule->write(position);
    break;
  }

  case 2 :{
    //SLIDER 1
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    slider1->write(position);
    break;
  }

  case 3 :{
    //SLIDER 2
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    slider2->write(position);
    break;
  }

  case 4 :{
    //SLIDER 3
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    slider3->write(position);
    break;
  }

  case 5 :{
    //SLIDER 4
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    slider4->write(position);
    break;
  }

  case 6 :{
    //ROTATION 1
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    rotation1->write(position);
    break;
  }

  case 7 :{
    //ROTATION 2
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    rotation2->write(position);
    break;
  }

  case 8 :{
    //ROTATION 3
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    rotation3->write(position);
    break;
  }

  case 9 :{
    //ROTATION 4
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    rotation4->write(position);
    break;
  }

  case 10 :{
    //TEMPERATURE
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    temp->write(position);
    break;
  }

  case 11:{
    //SERVO1 
    //temporaire : lire des infos de capteur (?)

    break;
  }

  case 12:{
    //SERVO2
    break;
  }

  case 20:{
    //STEPPER 1 MOVE-TO-POSITION
    int position = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&position);
    stepper1.moveTo(position);
    break;
  }

  case 21:{
    //STEPPER 1 ENABLE
    int enable = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&enable);
    digitalWrite(STEPPER1_ENABLE, !enable);
    break;
  }

  case 30:{
    //POMPE 1
    int enable = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&enable);
    digitalWrite(POMPE1, enable);
    break;
  }

  case 31:{
    //POMPE 2
    int enable = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&enable);
    digitalWrite(POMPE2, enable);
    break;
  }

  case 32:{
    //POMPE 3
    int enable = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&enable);
    digitalWrite(POMPE3, enable);
    break;
  }

  case 33:{
    //POMPE 4
    int enable = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&enable);
    digitalWrite(POMPE4, enable);
    break;
  }

  case 50:{
    int enable = 0;
    arrayToParameter(onReceiveData+1,BUFFERONRECEIVESIZE,"2%d",&enable);
    slider1->enable(enable);
    slider2->enable(enable);
    slider3->enable(enable);
    slider4->enable(enable);
    rotation1->enable(enable);
    rotation2->enable(enable);
    rotation3->enable(enable);
    rotation4->enable(enable);
    bascule->enable(enable);
    temp->enable(enable);
    break;
  }

  case 200:{
    //TEST
    countCommande++;
    break;
  }

  default:
    break;
  }
}

void requestEvent(){
  //Receive command and associate an action with the following table:
  //   commande   |   action
  //    101       |   Color button
  //    102       |   start magnet

  switch (onReceiveData[0])
  {

  case 101 :{
    parameterToArray(onRequestData,BUFFERONREQUESTSIZE,"2%d",!digitalRead(SERVO1));
    lenghtOnRequest = 2;
    break;
  }

  case 102 :{
    parameterToArray(onRequestData,BUFFERONREQUESTSIZE,"2%d",!digitalRead(SERVO2));
    lenghtOnRequest = 2;
    break;
  }

  default:
    break;
  }
  Wire.write(onRequestData, lenghtOnRequest);
}
