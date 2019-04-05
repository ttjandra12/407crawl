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

int servo1Position = 90;
int servo2Position = 90;
int servoIncrement = 6;


void setup() {
 // initialize serial communication at 9600 bits per second:
 Serial.begin(9600);

 // Set input pins
 pinMode(chA, INPUT_PULLUP);
 pinMode(chB,INPUT_PULLUP);
 //pinMode(chC,INPUT_PULLUP);
 //pinMode(chD,INPUT_PULLUP);
 pinMode(chE,INPUT_PULLUP);
 pinMode(chF,INPUT_PULLUP);

 servo1.attach(10);
 servo1.write(90);
 servo2.attach(11);
 servo2.write(90);
}

void loop() {
 ch1 = pulseIn (chA,HIGH);  //Read and store channel 1
 ch2 = pulseIn (chB,HIGH);  //Read and store channel 2
 ch3 = pulseIn (chC,HIGH);  //Read and store channel 3
 ch4 = pulseIn (chD,HIGH);  //Read and store channel 4
 ch5 = pulseIn (chE,HIGH);  //Read and store channel 5
 ch6 = pulseIn (chF,HIGH);  //Read and store channel 6

 if (ch2 > 1700)
 {
  servo1Position -= servoIncrement;
  servo1.write(servo1Position);
 }
 
 if (ch2 < 1300)
 {
  servo1Position += servoIncrement;
  servo1.write(servo1Position);
 }

 //Servo 2: Up and Down
 if (ch1 > 1700)
 {
  servo2Position -= servoIncrement;
  servo2.write(servo2Position);
 }
 
 if (ch1 < 1300)
 {
  servo2Position += servoIncrement;
  servo2.write(servo2Position);
 }

 Serial.println(ch5);
}
