#include <Servo.h>
Servo servo1;
Servo servo2;

// Define Variables:
const int chA=30;  //Constant variables relating to pin locations
const int chB=31;
const int chC=32;
const int chD=33;
const int chE=34;
const int chF=35;

int ch1;  //Varibles to store and display the values of each channel
int ch2;
int ch3;
int ch4;
int ch5;
int ch6;

int enA = 2;  // Motor one
int in1 = 22;
int in2 = 23;

int enB = 3;  // Motor two
int in3 = 24;
int in4 = 25;

int enC = 4;  // Motor three
int in5 = 28;
int in6 = 29;

int enD = 5;  // Motor four
int in7 = 26;
int in8 = 27;

int servoPosition1 = 90;
int servoPosition2 = 90;
int servoIncrement = 6;

void setup() {
  
  // Set input pins
  pinMode(chA, INPUT);
  pinMode(chB,INPUT);
  pinMode(chC,INPUT);
  pinMode(chD,INPUT);
  pinMode(chE,INPUT);
  pinMode(chF,INPUT);
  
  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enC, OUTPUT);
  pinMode(enD, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);

  //Set the servos
  servo1.attach(11);
  servo2.attach(12);
}

void loop() {
 ch1 = pulseIn (chA,HIGH);  //Read and store channel 1
 ch2 = pulseIn (chB,HIGH);  //Read and store channel 2
 ch3 = pulseIn (chC,HIGH);  //Read and store channel 3
 ch4 = pulseIn (chD,HIGH);  //Read and store channel 4
 ch5 = pulseIn (chE,HIGH);  //Read and store channel 5
 ch6 = pulseIn (chF,HIGH);  //Read and store channel 6

  //Motor Control
  if (ch4>1300 && ch4<1700)
  {
    if (ch3 <1300)
    {
      // turn on motor one
      analogWrite(enA, 200);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      
      // turn on motor two
      analogWrite(enB, 200);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      
      // turn on motor three
      analogWrite(enC, 200);
      digitalWrite(in5, HIGH);
      digitalWrite(in6, LOW);
      
      // turn on motor four
      analogWrite(enD, 200);
      digitalWrite(in7, LOW);
      digitalWrite(in8, HIGH);
    }
  
    if (ch3>1300 && ch3<1700)
    {
      analogWrite(enA, 0);
      analogWrite(enB, 0);
      analogWrite(enC, 0);
      analogWrite(enD, 0);
    }

    if (ch3 >1700)
    {
      // turn on motor one, reverse
      analogWrite(enA, 200);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      
      // turn on motor two, reverse
      analogWrite(enB, 200);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      
      // turn on motor three, reverse
      analogWrite(enC, 200);
      digitalWrite(in5, LOW);
      digitalWrite(in6, HIGH);
      
      // turn on motor four, reverse
      analogWrite(enD, 200);
      digitalWrite(in7, HIGH);
      digitalWrite(in8, LOW);
    }
  }

  if (ch4<1300)
  {
    // turn on motor one, forward
      analogWrite(enA, 200);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      
      // turn on motor two, reverse
      analogWrite(enB, 200);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      
      // turn on motor three, reverse
      analogWrite(enC, 200);
      digitalWrite(in5, HIGH);
      digitalWrite(in6, LOW);
      
      // turn on motor four, forward
      analogWrite(enD, 200);
      digitalWrite(in7, HIGH);
      digitalWrite(in8, LOW);
  }

  if (ch4>1700)
  {
    // turn on motor one, reverse
      analogWrite(enA, 200);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      
      // turn on motor two, forward
      analogWrite(enB, 200);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);

      // turn on motor three, forward
      analogWrite(enC, 200);
      digitalWrite(in5, LOW);
      digitalWrite(in6, HIGH);
      
      // turn on motor four, reverse
      analogWrite(enD, 200);
      digitalWrite(in7, LOW);
      digitalWrite(in8, HIGH);
  }

  //Control of the first servo motor in the FPV camera (up and down)
  if (ch2 > 1700)
 {
  servoPosition1 += servoIncrement;
  if (servoPosition1>180)
  {
    servoPosition1=180;
  }
  servo1.write(servoPosition1);
 }
 
 if (ch2 < 1300)
 {
  servoPosition1 -= servoIncrement;
  
  if (servoPosition1<0)
  {
    servoPosition1=0;
  }
  
  servo1.write(servoPosition1);
 }

 //Control of the second servo motor in the FPV camera (left and right)
  if (ch1 > 1700)
 {
  servoPosition2 += servoIncrement;
  
  if (servoPosition2>180)
  {
    servoPosition2=180;
  }
  
  servo2.write(servoPosition2);
 }
 
 if (ch1 < 1300)
 {
  servoPosition2 -= servoIncrement;
  
  if (servoPosition2<0)
  {
    servoPosition2=0;
  }
  
  servo2.write(servoPosition2);
 }
}
