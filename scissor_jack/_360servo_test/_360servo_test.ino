
/*
 * This Arduino code is written for the Parallax Continous Rotation servo that drives the scissor jack lift. 
 * 
 * Usage: Pressing the button triggers the interrupt and displays the angle in the serial monitor. 
 * 
 * Reference code taken from https://www.parallax.com/product/900-00360
 * 
 */

#include "Servo.h"

Servo servo;

int pinFeedback = 6;
int pinControl = 9; 
int pinButton = 3;

float angle;        
volatile int Kp = 1;                                // Proportional Gain
int turns = 0;  

int buttonState = 0;
volatile byte buttonPressed; 
void feedback360();
void pin_ISR();

void pin_ISR(){                         // Interrupt function 
  buttonPressed = true; 
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Continuous Servo Test");
  
  pinMode(pinFeedback, INPUT);
  pinMode(pinControl, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  servo.attach(9);

  attachInterrupt(digitalPinToInterrupt(3), pin_ISR, CHANGE);  // Detects when there is a change in buttonState and runs pin_ISR()

}

void loop() {

  servo.writeMicroseconds(1380);
  feedback360();
  
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

  Serial.print("Init tLow: ");
  Serial.print(tLow);
  Serial.print("|| Init tHigh: ");
  Serial.print(tHigh);
  Serial.print("|| Init dc: ");
  Serial.print(dc);
  Serial.print("|| Init theta: ");
  Serial.println(theta);  

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
 
    if (buttonPressed) {                      // Display angle in serial monitor if button presed 
      Serial.print("Angle: ");
      Serial.println(angle);
      servo.writeMicroseconds(1500);          // Stop the servo 
      buttonPressed = false;
      delay(2000);
    }
  }
}



