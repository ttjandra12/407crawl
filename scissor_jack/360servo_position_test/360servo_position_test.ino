#include <Servo.h>            //Servo library

//PID angle control for Parallax 360 Servo
Servo servo_test;        //initialize a servo object for the connected servo  
int pinFeedback = A0;
                
int target_angle = 180;
float tHigh = 0;
float tLow = 0;
int tCycle = 0;
float dc = 0;
float angle = 0; //Measured angle from feedback
float dcMin = 2.9; //From Parallax spec sheet
float dcMax = 97.1; //From Parallax spec sheet
float Kp = .7; //Proportional Gain, higher values for faster response, higher values contribute to overshoot.
float Ki = .2; //Integral Gain, higher values to converge faster to zero error, higher values produce oscillations. Higher values are more unstable near a target_angle = 0.
float iLimit = 5; //Arbitrary Anti-wind-up
float Kd = 1; //Derivative Gain, higher values dampen oscillations around target_angle. Higher values produce more holding state jitter. May need filter for error noise.
float prev_error = 0;
float prev_pError = 0;
float error = 0;
float pError = 0;
float iError = 0;

 
void setup() 
{ 
  servo_test.attach(9);      // attach the signal pin of servo to pin9 of arduino
  pinMode(A0, INPUT);
} 
  
void loop() 
{ 
  //use line below to determine "center" of servo. When near values of 90, manually force servo by hand in both directions to see if it continues to turn. 
  //servo_test.write(85); //clockwise: max 0 - 90 min, counter-clockwise: min 96-180 max, may be different for your servo, 93 stopped.
  
  while(1) //From Parallax spec sheet
  {
    tHigh = pulseIn(pinFeedback, HIGH);
    tLow = pulseIn(pinFeedback, LOW);
    tCycle = tHigh + tLow;
    if ( tCycle > 1000 && tCycle < 1200)
    {
      break; //valid tCycle;
    }
  }
  
  dc = (100 * tHigh) / tCycle; //From Parallax spec sheet, you are trying to determine the percentage of the HIGH in the pulse
  
  angle = ((dc - dcMin) * 360) / (dcMax - dcMin + 1); //From Parallax spec sheet

  //Keep measured angles within bounds
  if (angle < 0)
  {
    angle = 0;
  }
  else if(angle > 359)
  {
    angle = 359;
  }

  if (target_angle < 0)
  {
    target_angle = 360 + target_angle; //handles negative target_angles;
  }
  
  error = target_angle - angle;
  
  if(error > 180)
  {
    error = error - 360; //tells it to rotate in the other direction because it is a smaller angle that way.
  }
  if (error < -180)
  {
    error = 360 - error - 360; //tells it to rotate in the other direction because it is a smaller angle that way.
  }

  // PID controller stuff, Adjust values of Kp, Ki, and Kd above to tune your system
  float pError = Kp * error;
  float iError = Ki * (error + prev_error);

  if  (iError > iLimit)
  {
    iError = iLimit;
  }
  if (iError <  -iLimit)
  {
    iError = -iLimit;
  }
  
  prev_error = error;
  float dError = Kd * (pError - prev_pError);
  prev_pError = pError;

  error = error / 2; //max 180 error will have max 90 offset value

  int val = 93 - (Kp * error) - iError - dError; // 93 is the middle of my servo's "no motion" dead-band
  
  servo_test.write(val); //Move the servo

} 
