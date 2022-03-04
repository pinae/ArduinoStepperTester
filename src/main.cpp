#include <Arduino.h>
#include <AccelStepper.h>

//#define DEBUG_OUT

#define dirPin 2
#define stepPin 3
#define enPin 4
#define sleepPin 6
#define resetPin 7

#define dirButtonPin 5
#define potiPin A0

#define maxSpeed 2000
#define microsteps 32
#define maxAcc 500.0

AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, stepPin, dirPin);
float currentSpeed = 0.0;
unsigned long lastUpdate;
unsigned long lastDirChange;
int dir = 1;

void setup() {  
  #ifdef DEBUG_OUT
  Serial.begin(115200);
  #endif
  pinMode(dirButtonPin, INPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  stepper.setEnablePin(enPin);
  stepper.setPinsInverted(false, false, true);
  digitalWrite(sleepPin, HIGH);
  digitalWrite(resetPin, HIGH);
  stepper.enableOutputs();
  stepper.setMaxSpeed(maxSpeed);
  stepper.setSpeed(currentSpeed);
  lastUpdate = millis();        
}

void loop() {
  unsigned long now = millis();
  stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed();
  float targetSpeed = dir * maxSpeed * microsteps * max((float) analogRead(potiPin)-3, 0) / 1020;
  stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed();
  if ((now - lastDirChange >= 1000) && 
      (!digitalRead(dirButtonPin))) {
    dir *= -1;
    lastDirChange = now;
  }
  stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed();
  if(now - lastUpdate > 20) {
    #ifdef DEBUG_OUT
    Serial.print("Direction: ");
    Serial.print(dir);
    Serial.print(" Target Speed: ");
    Serial.print(targetSpeed);
    #endif
    float acstp = maxAcc * microsteps * 0.02;
    float acceleration = max(min(targetSpeed - currentSpeed, acstp), -acstp);
    float newSpeed = currentSpeed + acceleration;
    stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed();
    if(abs(newSpeed - currentSpeed) > 0.00001) {
      stepper.setSpeed(currentSpeed);
      currentSpeed = newSpeed;
    }
    #ifdef DEBUG_OUT
    Serial.print(" Speed: ");
    Serial.println(currentSpeed);
    #endif
    lastUpdate = now;
  }
  stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed(); stepper.runSpeed();
}