#include "Servo.h"

int pinFeedback = A0;
int pinControl = 9;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Measuring PWM Signals Test");
  
  pinMode(pinFeedback, INPUT);
  pinMode(pinControl, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int tLow = pulseIn(pinFeedback, LOW);
  int tHigh = pulseIn(pinFeedback, HIGH);

  Serial.print("tLow =");
  Serial.print(tLow);
  Serial.print("; tHigh = ");
  Serial.println(tHigh);
}
