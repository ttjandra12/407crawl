#include <Servo.h>
Servo servo;

// Define Variables:
const int chA=7;  //Constant variables relating to pin locations
const int chB=2;
const int chC=3;
const int chD=4;
const int chE=5;
const int chF=6;

int ch1;  //Varibles to store and display the values of each channel
int ch2;
int ch3;
int ch4;
int ch5;
int ch6;

int servoPosition = 90;
int servoIncrement = 6;


void setup() {
 // initialize serial communication at 9600 bits per second:
 Serial.begin(115200);

 // Set input pins
 pinMode(chA, INPUT);
 pinMode(chB,INPUT);
 pinMode(chC,INPUT);
 pinMode(chD,INPUT);
 pinMode(chE,INPUT);
 pinMode(chF,INPUT);

 servo.attach(12);
 servo.write(90);
}

void loop() {
 ch1 = pulseIn (chA,HIGH);  //Read and store channel 1
 ch2 = pulseIn (chB,HIGH);  //Read and store channel 1
 ch3 = pulseIn (chC,HIGH);  //Read and store channel 1
 ch4 = pulseIn (chD,HIGH);  //Read and store channel 1
 ch5 = pulseIn (chE,HIGH);  //Read and store channel 1
 ch6 = pulseIn (chF,HIGH);  //Read and store channel 1

 if (ch2 > 1700)
 {
  servoPosition -= servoIncrement;
  servo.write(servoPosition);
 }
 
 if (ch2 < 1300)
 {
  servoPosition += servoIncrement;
  servo.write(servoPosition);
 }

 Serial.print ("Ch2:");
 Serial.print (ch2);     
 Serial.print ("|");
 Serial.print(servoPosition);


}
