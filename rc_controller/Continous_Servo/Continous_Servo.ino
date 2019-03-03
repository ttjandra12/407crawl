#include "Servo.h"

Servo servo;

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

int ch6_change = 700;
int ch6_PrevRead = 1200;
int ch6_CurrRead = 0;

int pinFeedback = 8; //yellow cable
int pinControl = 9; //white cable
int pinButton = 18;

float angle;        
volatile int Kp = 1;                                // Proportional Gain
int turns = 0;

int upper_limit = 1000;
int lower_limit = -1;

int buttonState = 0;
volatile byte buttonPressed; 
void feedback360();
void pin_ISR();

void pin_ISR(){                         // Interrupt function 
  buttonPressed = true; 
}

void setup() {
   // Set input pins
  pinMode(chA, INPUT);
  pinMode(chB,INPUT);
  pinMode(chC,INPUT);
  pinMode(chD,INPUT);
  pinMode(chE,INPUT);
  pinMode(chF,INPUT);

  pinMode(pinFeedback, INPUT);
  pinMode(pinControl, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  servo.attach(9);

  Serial.begin(9600);
  Serial.println("Continuous Servo Test");

  attachInterrupt(digitalPinToInterrupt(18), pin_ISR, CHANGE);  // Detects when there is a change in buttonState and runs pin_ISR()
}

void loop() {
  ch1 = pulseIn (chA,HIGH);  //Read and store channel 1
  ch2 = pulseIn (chB,HIGH);  //Read and store channel 2
  ch3 = pulseIn (chC,HIGH);  //Read and store channel 3
  ch4 = pulseIn (chD,HIGH);  //Read and store channel 4
  ch5 = pulseIn (chE,HIGH);  //Read and store channel 5
  ch6 = pulseIn (chF,HIGH);  //Read and store channel 6

  //Read the CH6 input
  ch6_CurrRead = ch6;

  int delta_ch6= abs(ch6_CurrRead-ch6_PrevRead);
    
  if(delta_ch6 > ch6_change ) //if the nob is turned past a certain point, start moving the scissor jack.
  {

  servo.writeMicroseconds(1440);
  feedback360();
  buttonPressed = false;

  servo.writeMicroseconds(1550);
  return_func();
  
  ch6_PrevRead = ch6_CurrRead;
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

  buttonState = digitalRead(pinButton);
  
  while(buttonState == LOW){
    // Measure high and low times, making sure to only take valid cycle
    // times (a high and a low on opposite sides of the 0/359 boundary
    // will not be valid.
    while(buttonState == LOW)                                    // Keep checking
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

    if (angle > upper_limit) {
      servo.writeMicroseconds(1550);          // Make the servo go backwards
    }

    if (angle < lower_limit) {
      servo.writeMicroseconds(1440);          // Make the servo go forward 
    }

    if (buttonPressed) {                      // Display angle in serial monitor if button presed 
      servo.writeMicroseconds(1500);          // Stop the servo
      Serial.print("Angle: ");
      Serial.println(angle); 
      buttonPressed = false;
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

  buttonState = digitalRead(pinButton);
  
  while(buttonState == LOW){
    // Measure high and low times, making sure to only take valid cycle
    // times (a high and a low on opposite sides of the 0/359 boundary
    // will not be valid.
    while(buttonState == LOW)                                    // Keep checking
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


    if (angle <= 0) {
      servo.writeMicroseconds(1500);          // Make the servo go forward 
      break;
    }
  }
}

