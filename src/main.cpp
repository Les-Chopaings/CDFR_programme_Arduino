#include <Arduino.h>
#include "config.h"
#include <Servo.h>
#include <AccelStepper.h>
Servo bascule;
AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP, STEPPER1_DIR);
int pomp_list[] = {POMPE1, POMPE2, POMPE3, POMPE4};
unsigned long T_start;
int toggle = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Start");
  pinMode(STEPPER_RESET, OUTPUT);
  digitalWrite(STEPPER_RESET, 1);
  pinMode(STEPPER_SLEEP, OUTPUT);
  digitalWrite(STEPPER_SLEEP, 1);
  stepper1.enableOutputs();
  stepper1.setAcceleration(1e4);
  stepper1.setMaxSpeed(2e6);
  T_start = millis();

  pinMode(POMPE1, OUTPUT);
  pinMode(POMPE2, OUTPUT);
  pinMode(POMPE3, OUTPUT);
  pinMode(POMPE4, OUTPUT);

  bascule.attach(BASCULE);
  bascule.write(180);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(POMPE1, HIGH);
  
  // for (int i = 0; i < 5; i++){
  //   digitalWrite(pomp_list[i], HIGH);
  //   Serial.println("high");
  //   delay(1000);
  //   digitalWrite(pomp_list[i], LOW);
  //   Serial.println("low");
  //   delay(1000);
  // }

  // stepper1.run();
  // if ((millis() - T_start) > 5000){
  //   stepper1.moveTo(1e4*toggle);
  //   T_start = millis();
  //   toggle = !toggle;
  //   Serial.println(T_start);
  //   Serial.println(toggle*1e4);
  // }

  // delay(1000);
  // bascule.write(0);
  // delay(1000);
  // bascule.write(180);
}
