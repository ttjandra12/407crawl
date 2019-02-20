
/*
 * This Arduino code is written for the Parallax Continous Rotation servo that drives the scissor jack lift. 
 * 
 * Reference code taken from https://www.parallax.com/product/900-00360
 * 
 */

#include "simpletools.h"
#include "servo.h"

int pinFeedback = 8;
int pinControl = 9; 

volatile int angle, targetAngle;        
volatile int Kp = 1;                                // Proportional Gain

void feedback360();
void control360();                          

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Continuous Servo Test");
  
  pinMode(pinFeedback, INPUT);
  pinMode(pinControl, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  cog_run(feedback360, 128);
  cog_run(control360, 128);

  while(1){
    if (Serial.available()){
      println("Enter angle: ");
      char targetAngle = Serial.read();  
    }
    while(abs(targetAngle - angle) > 4){       // Display until close to finish
      print("targetAngle = %d, angle = %d\r",targetAngle, angle); // Display target & measured             
      pause(50);                              // ...every 50 ms
    }      
  }  
}

void feedback360(){
  int unitsFC = 360;                        // Units in a full circle
  int dutyScale = 1000;                     // Scale duty cycle to 1/1000ths
  int dcMin = 29;                           // Minimum duty cycle, 2.9%
  int dcMax = 971;                          // Maximum duty cycle, 97.1%
  int q2min = unitsFC/4;                    // For checking if in 1st quadrant
  int q3max = q2min*3;                      // For checking if in 4th quadrant
  int turns = 0;
  
  // dc is duty cycle, theta is 0 to 359 angle, thetaP is theta from previous
  // loop repetition, tHigh and tLow are the high and low signal times for 
  // duty cycle calculations.
  int dc, theta, thetaP, tHigh, tLow;

  tLow = pulse_in(pinFeedback, 0);          // Measure low time
  tHigh = pulse_in(pinFeedback, 1);         // Measure high time

  // Calcualte initial duty cycle and angle.
  dc = (dutyScale * tHigh) / (tHigh + tLow);
  theta = (unitsFC - 1) - ((dc - dcMin) * unitsFC) / (dcMax - dcMin + 1);
  thetaP = theta;

  while(1){
    // Measure high and low times, making sure to only take valid cycle
    // times (a high and a low on opposite sides of the 0/359 boundary
    // will not be valid.
    int tCycle = 0;                           // Clear cycle time
    while(1)                                  // Keep checking
    {
      tHigh = pulse_in(pinFeedback, 1);       // Measure time high
      tLow = pulse_in(pinFeedback, 0);        // Measure time low
      tCycle = tHigh + tLow;
      if((tCycle > 1000) && (tCycle < 1200)){  // If cycle time valid 
        break;                                 // break from loop
      }
    }      
    dc = (dutyScale * tHigh) / tCycle;        // Calculate duty cycle
    
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
}

// Most rudimentary control system example, 
// just proportional.  This could be done
// in the same cog as the angle mesurement.                                            
void control360()                             // Cog for control system
{
  servo_speed(pinControl, 0);                 // Start servo control cog
  
  int errorAngle, output, offset;             // Control system variables
  
  while(1)                                    // Main loop for this cog
  {
    errorAngle = targetAngle - angle;         // Calculate error
    output = errorAngle * Kp;                 // Calculate proportional 
    if(output > 200){
      output = 200;            // Clamp output
    }
    if(output < -200){
      output = -200;
    }
    if(errorAngle > 0){                        // Add offset
      offset = 30;
    }
    else if(errorAngle < 0){
      offset = -30;
    }
    else{
      offset = 0;
    }     
    
    servo_speed(pinControl, output + offset); // Set output
    pause(20);                                // Repeat after 20 ms
  }    
}  
