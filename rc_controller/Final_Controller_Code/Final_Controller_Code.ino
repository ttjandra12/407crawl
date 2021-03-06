#include "VL53L0X.h"            //Changed library .h file (previous: "VL53L0X.h")
#include <Wire.h>               // Libraries already included as default in Arduino   
#include <VL53L0X.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


Servo servo1;
Servo servo2;
Servo cont_servo;
Servo xservo;
Servo yservo;
 
double levelx;
double levely;

double mpux;
double mpuy;

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

VL53L0X ToF_sensor;

File myFile;

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

int ch5_change = 700;
int ch5_PrevRead = 1200;
int ch5_CurrRead = 0;

int ch6_change = 700;
int ch6_PrevRead = 1200;
int ch6_CurrRead = 0;


int pinFeedback = 8; //yellow cable
int pinControl = 7; //white cable

float angle;
float recorded_angle;        
volatile int Kp = 1;                                // Proportional Gain
int turns = 0;

int upper_limit = -2000;
int lower_limit = 0;


int trigger = 0;
void feedback360();
void pin_ISR();
void level();
void return_func();
void selectMuxPin(byte pin);

void pin_ISR(){                         // Interrupt function 
  trigger = 1; 
}

const int selectPins[3] = {43, 45, 47}; // S0~2, S1~3, S2~4
const int Y1Input = 44; // Connect output Y1 to
const int Y2Input = 46; // Connect output Y2 

void setup() {

  Serial.begin(9600);

//  if(!bno.begin())
//  {
//    /* There was a problem detecting the BNO055 ... check your connections */
//    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
//    while(1);
//  }
  
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

  pinMode(pinFeedback, INPUT);
  pinMode(pinControl, OUTPUT);
  
  //Set the servos
  servo1.attach(13);
  servo2.attach(12);
  cont_servo.attach(7);

  #define HIGH_ACCURACY
  
  Wire.begin();

  ToF_sensor.init();
  ToF_sensor.setMeasurementTimingBudget(20000);

  Serial.print("Initializing SD card....");
  if (!SD.begin(53)){
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // Set up the select pins from the sensor carousel as outputs:
  for (int i=0; i<3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  
  pinMode(Y1Input, INPUT); // Set up Y1 as an input
  pinMode(Y2Input, INPUT); // Set up Y2 as an input

  if (!SD.begin(53)) {
    Serial.println("SD card is not connected");
    return;
  }

  bno.setExtCrystalUse(true);

  levelx = 90; // initial x level
  levely = 99; //initial y level 
  
  xservo.attach(11); //attach to the correct pin
  yservo.attach(6); //attach to the correct pin

  xservo.write(levelx); //moves x servo
  Serial.print("Angle Set Up X");
  yservo.write(levely); //moves y servo
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
      analogWrite(enA, 120);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      
      // turn on motor two
      analogWrite(enB, 120);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      
      // turn on motor three
      analogWrite(enC, 120);
      digitalWrite(in5, LOW);
      digitalWrite(in6, HIGH);
      
      // turn on motor four
      analogWrite(enD, 120);
      digitalWrite(in7, HIGH);
      digitalWrite(in8, LOW);
    }
  
    if (ch3>1300 && ch3<1700)
    {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      digitalWrite(in5, LOW);
      digitalWrite(in6, LOW);
      digitalWrite(in7, LOW);
      digitalWrite(in8, LOW);
    }

    if (ch3 >1700)
    {  
      // turn on motor one, reverse
      analogWrite(enA, 120);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      
      // turn on motor two, reverse
      analogWrite(enB, 120);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      
      // turn on motor three, reverse
      analogWrite(enC, 120);
      digitalWrite(in5, HIGH);
      digitalWrite(in6, LOW);
      
      // turn on motor four, reverse
      analogWrite(enD, 120);
      digitalWrite(in7, LOW);
      digitalWrite(in8, HIGH);
    }
  }

  if (ch4<1300)
  {    
    // turn on motor one, forward
    analogWrite(enA, 120);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
      
    // turn on motor two, reverse
    analogWrite(enB, 120);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
      
    // turn on motor three, reverse
    analogWrite(enC, 120);
    digitalWrite(in5, HIGH);
    digitalWrite(in6, LOW);
      
    // turn on motor four, forward
    analogWrite(enD, 120);
    digitalWrite(in7, HIGH);
    digitalWrite(in8, LOW);
  }

  if (ch4>1700)
  {
    // turn on motor one, reverse
    analogWrite(enA, 120);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
      
     // turn on motor two, forward
     analogWrite(enB, 120);
     digitalWrite(in3, LOW);
     digitalWrite(in4, HIGH);

     // turn on motor three, forward
     analogWrite(enC, 120);
     digitalWrite(in5, LOW);
     digitalWrite(in6, HIGH);
      
     // turn on motor four, reverse
     analogWrite(enD, 120);
     digitalWrite(in7, LOW);
     digitalWrite(in8, HIGH);
  }

  //Control of the first servo motor in the FPV camera (up and down)
  if (ch1 > 1700)
  {
    servoPosition1 += servoIncrement;
    if (servoPosition1>180)
    {
      servoPosition1=180;
    }
    servo1.write(servoPosition1);
  }
 
  if (ch1 < 1300)
  {
    servoPosition1 -= servoIncrement;
  
    if (servoPosition1<0)
    {
      servoPosition1=0;
    }
  
    servo1.write(servoPosition1);
  }

 //Control of the second servo motor in the FPV camera (left and right)
  if (ch2 > 1700)
  {
    servoPosition2 += servoIncrement;
  
    if (servoPosition2>180)
    {
      servoPosition2=180;
    }
  
    servo2.write(servoPosition2);
  }
 
  if (ch2 < 1300)
  {
    servoPosition2 -= servoIncrement;
  
    if (servoPosition2<0)
    {
      servoPosition2=0;
    }
  
  servo2.write(servoPosition2);
  }

  //Read the CH6 input
  ch6_CurrRead = ch6;
  int delta_ch6= abs(ch6_CurrRead-ch6_PrevRead);
    
  if(delta_ch6 > ch6_change) //if the switch changes position the scissor jack will move.
  {

    cont_servo.writeMicroseconds(1560);
    feedback360();
    trigger = 0;

    recorded_angle = -1*angle;
    Serial.print("The Recorded Angle is: ");
    Serial.println(recorded_angle);
    delay(5000);

    cont_servo.writeMicroseconds(1440);
    return_func();

    ch6_PrevRead = ch6_CurrRead;
  }

  //Read the CH5 input
  ch5_CurrRead = ch5;
  int delta_ch5= abs(ch5_CurrRead-ch5_PrevRead);
    
  if(delta_ch5 > ch5_change ) //if the nob is turned past a certain point, record sensor readouts.
  {
    String dataString = "";

    //this for loop collects data from the 3 sensors 
    for (int i = 0; i < 3; i++){
      //collects data from ToF sensor
      Serial.println(String(i));
      if (i == 1){
        level();
        
        float sensor = ToF_sensor.readRangeSingleMillimeters();
        dataString += "," + String(sensor);
      } 
      if (i == 0){
        imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
        
        float sensor1 = euler.y(); //pitch in degrees
        float sensor2 = euler.z(); //roll in degrees
        dataString += String(sensor1) + "," + String(sensor2);
      }
      if (i == 2){ //dummy code in place for 1 more sensor
        dataString += ",";
        float height_adjs = angle; //get the height adjustment by mutiplying angular displacement with corresponding quantity
        dataString += String(height_adjs);
      }
    }

    //opening file on SD card and writing data string onto it
    myFile = SD.open("OUTPUT_FINAL.csv", FILE_WRITE);
    if (myFile){
      myFile.println(dataString);
      myFile.close();
      Serial.println("complete");
      delay(1000);
    } 
    else {
      Serial.println(dataString);
      Serial.println("error opening test1.csv");
    }
 
    ch5_PrevRead = ch5_CurrRead;
  }
 
}

void feedback360(){
  
  int unitsFC = 360;                        // Units in a full circle
  int dutyScale = 100;                      // Scale duty cycle to 1/1000ths
  float dcMin = 2.9;                        // Minimum duty cycle, 2.9%
  float dcMax = 97.1;                       // Maximum duty cycle, 97.1%
  int q2min = unitsFC/4;                    // For checking if in 1st quadrant
  int q3max = q2min*3;                      // For checking if in 4th quadrant
  
  // dc is duty cycle, theta is 0 to 359 angle, thetaP is theta from previous
  // loop repetition, tHigh and tLow are the high and low signal times for 
  // duty cycle calculations.
  float dc, tHigh, tLow, tCycle, theta, thetaP;    

  tLow = pulseIn(pinFeedback, LOW);          // Measure low time
  tHigh = pulseIn(pinFeedback, HIGH);        // Measure high time
  tCycle = tLow + tHigh;
  
  // Calcualte initial duty cycle and angle.
  dc = (dutyScale * tHigh) / (tHigh + tLow);
  theta = (unitsFC - 1) - ((dc - dcMin) * unitsFC) / (dcMax - dcMin + 1);
  thetaP = theta; 
  
  while(1){
    // Measure high and low times, making sure to only take valid cycle
    // times (a high and a low on opposite sides of the 0/359 boundary
    // will not be valid.
    while(1)                                    // Keep checking
    {
      
      // Loop through all eight pins from the carousel.
      for (byte pin=0; pin<=7; pin++)
      {
        selectMuxPin(pin); // Select one at a time
        int inputY1Value = digitalRead(44); // and read Y1
        int inputY2Value = digitalRead(46); // and read Y2

        if (inputY1Value == HIGH || inputY2Value == HIGH){
          trigger= 1;
        }
  
      }
      
      tHigh = pulseIn(pinFeedback, LOW);        // Measure time high
      tLow = pulseIn(pinFeedback, HIGH);        // Measure time low
      tCycle = tHigh + tLow;
      if((tCycle > 1000) && (tCycle < 1200)){   // If cycle time valid 
        break;                                  // break from loop
      }
    }

    dc = (dutyScale * tHigh) / tCycle;          // Calculate duty cycle
    
    // This gives a theta increasing in the counterclockwise direction.
    theta = (unitsFC - 1) - ((dc - dcMin) * unitsFC)/(dcMax - dcMin + 1);          // Calculate angle
   
    
    if(theta < 0){                             // Keep theta valid
      theta = 0; 
    }
    else if(theta > (unitsFC - 1)){ 
      theta = unitsFC - 1;
    }
    
    //These four lines of code accounts for rotation beyond 359 degrees.
    // If transition from quadrant 4 to quadrant 1, increase turns count. 
    if((theta < q2min) && (thetaP > q3max)){
      turns++;
    }
    // If transition from quadrant 1 to quadrant 4, decrease turns count. 
    else if((thetaP < q2min) && (theta > q3max)){
      turns --;
    }
    // Construct the angle measurement from the turns count and current theta value.
    if(turns >= 0){
      angle = (turns * unitsFC) + theta;
    }
    else if(turns <  0){
      angle = ((turns + 1) * unitsFC) - (unitsFC - theta);
    }
    
    thetaP = theta;                           // Theta previous for next rep
    
    if (angle < upper_limit) {
      cont_servo.writeMicroseconds(1440);          // Make the servo go backwards
    }
      
    if (angle > lower_limit) {
      cont_servo.writeMicroseconds(1560);          // Make the servo go forward 
    }
 
    if (trigger) {                      // Display angle in serial monitor if button presed 
      cont_servo.writeMicroseconds(1500);          // Stop the servo
      trigger = 0;
      delay(1000);
      break;
    }
  } 
}

void return_func(){
  int unitsFC = 360;                        // Units in a full circle
  int dutyScale = 100;                      // Scale duty cycle to 1/1000ths
  float dcMin = 2.9;                        // Minimum duty cycle, 2.9%
  float dcMax = 97.1;                       // Maximum duty cycle, 97.1%
  int q2min = unitsFC/4;                    // For checking if in 1st quadrant
  int q3max = q2min*3;                      // For checking if in 4th quadrant
  
  // dc is duty cycle, theta is 0 to 359 angle, thetaP is theta from previous
  // loop repetition, tHigh and tLow are the high and low signal times for 
  // duty cycle calculations.
  float dc, tHigh, tLow, tCycle, theta, thetaP;    

  tLow = pulseIn(pinFeedback, LOW);          // Measure low time
  tHigh = pulseIn(pinFeedback, HIGH);        // Measure high time
  tCycle = tLow + tHigh;
  
  // Calcualte initial duty cycle and angle.
  dc = (dutyScale * tHigh) / (tHigh + tLow);
  theta = (unitsFC - 1) - ((dc - dcMin) * unitsFC) / (dcMax - dcMin + 1);
  thetaP = theta; 
  
  while(1){
    // Measure high and low times, making sure to only take valid cycle
    // times (a high and a low on opposite sides of the 0/359 boundary
    // will not be valid.
    while(1)                    
    {
      tHigh = pulseIn(pinFeedback, LOW);        // Measure time high
      tLow = pulseIn(pinFeedback, HIGH);        // Measure time low
      tCycle = tHigh + tLow;
      if((tCycle > 1000) && (tCycle < 1200)){   // If cycle time valid 
        break;                                  // break from loop
      }
    }

    dc = (dutyScale * tHigh) / tCycle;          // Calculate duty cycle
    
    // This gives a theta increasing in the counterclockwise direction.
    theta = (unitsFC - 1) - ((dc - dcMin) * unitsFC)/(dcMax - dcMin + 1);          // Calculate angle
   
    
    if(theta < 0){                             // Keep theta valid
      theta = 0; 
    }
    else if(theta > (unitsFC - 1)){ 
      theta = unitsFC - 1;
    }
    
    //These four lines of code accounts for rotation beyond 359 degrees.
    // If transition from quadrant 4 to quadrant 1, increase turns count. 
    if((theta < q2min) && (thetaP > q3max)){
      turns++;
    }
    // If transition from quadrant 1 to quadrant 4, decrease turns count. 
    else if((thetaP < q2min) && (theta > q3max)){
      turns --;
    }
    // Construct the angle measurement from the turns count and current theta value.
    if(turns >= 0){
      angle = (turns * unitsFC) + theta;
    }
    else if(turns <  0){
      angle = ((turns + 1) * unitsFC) - (unitsFC - theta);
    }
    
    thetaP = theta;                           // Theta previous for next rep
    
    if (angle >= 0) {
      cont_servo.writeMicroseconds(1500); // Make the servo go forward 
      Serial.println("The return Angle is:");
      Serial.println(angle);
      break;
    }
  }
}

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.

void selectMuxPin(byte pin)
{
  for (int i=0; i<3; i++)
  {
    if (pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}

void level()
{
  while(1) {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */

  mpux = round(euler.z()); //rounds the mpu values to move the motor and reduce noise for if statement
  mpuy = round(euler.y()); //rounds the mpu values to move the motor and reduce noise for if statement

// yservo 
if (mpuy < -2 && mpuy >=-90) {
  if (mpuy == -3){
    return;  //if it is already level, it ends the code
  }
  else{
    levely = levely + 1;
    yservo.write(levely); //moves the y motor to one side until it is level
  }
//  Serial.print("MPU-Y =");
//  Serial.println(mpuy);
}


if (mpuy <=90 && mpuy > -2){
  if (mpuy ==0 || mpuy ==-1 || mpuy ==1){
    return; //if level already, moves out
  }
  else{

    levely = levely - 1;
    yservo.write(levely); // moves the y motor to one side until it is level
  }
//  Serial.print("MPU-Y =");
//  Serial.println(mpuy);
}

//xservo
if (mpux >= -180 && mpux <= - 90) {
  if (mpux == -180 || mpux == -179 || mpux == -178){
    return; //if x side is level, moves out of the if statement
  }
  else {
    levelx = levelx - 1;
    xservo.write(levelx); //moves the x motor to one side until it is level
  }
//  Serial.print("MPU-X =");
//  Serial.println(mpux);
}

if (mpux >= 90 && mpux < 180){
  if (mpux == 180 || mpux == 179){
    return; // if x side is level, moves out of the if statement 
  }
  else {
    levelx = levelx + 1;
    xservo.write(levelx); //moves the x motor to one side until it is level
  }

}
 

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0; //calibrates the MPU, took straight from the example code 
  bno.getCalibration(&system, &gyro, &accel, &mag);

  delay(BNO055_SAMPLERATE_DELAY_MS);

  if (mpuy ==0 || abs(mpuy) ==1){
    if (abs(mpux) == 180 || abs(mpux) == 179){
      break;
      }
    }
  }
}

