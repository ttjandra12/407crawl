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

double angx;
double angy;

double moveanglex;
double moveangley;

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

  levelx = 93;
  levely = 99;
  
  xservo.attach(8);
  yservo.attach(9);
  
  
  xservo.write(levelx);
  Serial.print("Angle Set Up X");
  yservo.write(levely);

  delay(5000);
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



  mpux = round(euler.z());
  mpuy = round(euler.y());

// yservo 
if (mpuy < -2 && mpuy >=-90) {
  if (mpuy == 0 || mpuy == -1){
    return;
  }
  else{
    levely = levely + 1;
    yservo.write(levely);
  }
  Serial.print("MPU-Y =");
  Serial.println(mpuy);
}


if (mpuy <=90 && mpuy > -2){

    levely = levely - 1;
    yservo.write(levely);

  Serial.print("MPU-Y =");
  Serial.println(mpuy);
}

//xservo
if (mpux > -180 && mpux <= - 90) {
  if (mpux == -180 || mpux == -179){
    return;
  }
  else {
    levelx = levelx - 1;
    xservo.write(levelx);
  }
  Serial.print("MPU-X =");
  Serial.println(mpux);
}

if (mpux >= 90 && mpux < 180){
  if (mpux == 180 || mpux == 179){
    return;
  }
  else {
    levelx = levelx + 1;
    xservo.write(levelx);
  }
  Serial.print("MPU-X =");
  Serial.println(mpux);
}
  Serial.print("MPU-X =");
  Serial.println(mpux);

Serial.println("LEVEL!");

  

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
//  Serial.print("CALIBRATION: Sys=");
//  Serial.print(system, DEC);
//  Serial.print(" Gyro=");
//  Serial.print(gyro, DEC);
//  Serial.print(" Accel=");
//  Serial.print(accel, DEC);
//  Serial.print(" Mag=");
//  Serial.println(mag, DEC);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}
