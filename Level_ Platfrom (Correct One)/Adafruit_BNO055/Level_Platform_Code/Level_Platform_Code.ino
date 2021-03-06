#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Servo.h>

Servo xservo;
Servo yservo;

double levelx;
double levely;

double mpux;
double mpuy;


/* This driver reads raw data from the BNO055

   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(9600);

  yservo.attach(10); //attach to the correct pin
    yservo.write(90); //moves y servo
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();


  bno.setExtCrystalUse(true);

  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");

  levelx = 90; // initial x level 88
  levely = 120; //initial y level 93
  

  xservo.attach(11); //attach to the correct pin
  yservo.attach(10); //attach to the correct pin

  
  
  xservo.write(levelx); //moves x servo
  yservo.write(90); //moves y servo
  
  Serial.print("Angle Set Up");
  //delay(5000);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */

  mpux = round(euler.z()); //rounds the mpu values to move the motor and reduce noise for if statement
  mpuy = round(euler.y()); //rounds the mpu values to move the motor and reduce noise for if statement

// yservo 

if (mpuy < -2 && mpuy >=-90) {
  if (mpuy == -3){
    return;  //if it is already level, it ends the code
  }
  if (mpuy <-3 && mpuy >= -20){
    levely = levely + 1;
    yservo.write(levely);
    Serial.println("One");
  }
  else{
    levely = levely + 10; // if problem change to 1 rather than 10
    yservo.write(levely);//moves the y motor to one side until it is level
    delay(1000);
    Serial.println("Two");
 
  }
  Serial.print("MPU-Y =");
  Serial.println(mpuy);
}


if (mpuy <=90 && mpuy > -2){
  if (mpuy ==0 || abs(mpuy) ==1 || mpuy ==2 || mpuy == 3 || mpuy == 4){
    return; //if level already, moves out
  }
 if (mpuy <= 20 && mpuy > 4){
   levely = levely - 1;
   yservo.write(levely);
   Serial.println("Three");
 }
  else{
    levely = levely - 10; //if problem change to 1 rather than 10
    yservo.write(levely); // moves the y motor to one side until it is level
    delay(1000);
    Serial.println("Four");
  }
  Serial.print("MPU-Y =");
  Serial.println(mpuy);
}

//xservo
if (mpux > -180 && mpux <= - 90) {
  if (mpux == -180 || mpux == -179 || mpux == -178){
    return; //if x side is level, moves out of the if statement
  }
  if (mpux > -178 && mpux <= -168){
    levelx = levelx - 1;  
    xservo.write(levelx);
    Serial.println("Five");
  }
  else {
    levelx = levelx - 10; //if problematic change to 1 rathen than 10
    xservo.write(levelx); //moves the x motor to one side until it is level
    delay(1000);
    Serial.println("Six");
  }
  Serial.print("MPU-X =");
  Serial.println(mpux);
}

if (mpux >= 90 && mpux < 180){
  if (mpux == 180 || mpux == 179){
    return; // if x side is level, moves out of the if statement 
  }
  if (mpux > 168 && mpux <178 ){
     levelx = levelx + 1;
     xservo.write(levelx);
     Serial.println("Seven");
  }
  else {
    levelx = levelx + 10; //if problematic change to 1 rather than 10
    xservo.write(levelx); //moves the x motor to one side until it is level
    delay(1000);
    Serial.println("Eight");
  }
  Serial.print("MPU-X =");
  Serial.println(mpux);
}

delay(1000);

if (mpuy == -2 || mpuy == -3 || mpuy ==0 || mpuy== 1 || mpuy ==2 || mpuy == 3 || mpuy == 4){
  yservo.detach();
  Serial.print("FINAL MPUY =");
  Serial.println(mpuy); 
  Serial.println("DONE WITH Y");
}
  else {
    yservo.attach(9);
    Serial.println("NOT DONE Y");
    Serial.println(mpuy);

}
  if(mpux == -180 || mpux == -179 || mpux == -178 ||mpux == 180 || mpux == 179){
  xservo.detach();
  Serial.print("FINAL MPUX =");
  Serial.println(mpux);
  
  
  Serial.println("LEVEL!");
  }
  else {
  xservo.attach(8);
  Serial.println("NOT DONE X");

}

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0; //calibrates the MPU, took straight from the example code 
  bno.getCalibration(&system, &gyro, &accel, &mag);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}
