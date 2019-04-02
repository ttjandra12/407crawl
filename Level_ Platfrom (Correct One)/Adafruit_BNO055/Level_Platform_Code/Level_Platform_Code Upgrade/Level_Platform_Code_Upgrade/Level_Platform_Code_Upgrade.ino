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

double findx;
double findy;

double moveangx;
double moveangy;

double mpulevelx;
double mpulevely;

double nmpuy;
int counter;
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
void setup() {
  // put your setup code here, to run once:
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
  levelx = 88; //initial x level by motor angle
  levely = 93; //initial y level by motor angle

  mpulevelx = -180; //what is considered level by mpu value for x
  mpulevely = -1;  //what is considered level by mpu value for y

  counter = 0;
  
  xservo.attach(8); //attach motor to the correct pin
  yservo.attach(9); //attach motor to the correct pin

  xservo.write(levelx); //moves x servo 
  yservo.write(levely); //moves y servo

  Serial.print("Motors set to initial position");

  delay (5000);
}

void loop() {
  
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */

  mpux = round(euler.z()); //rounds the mpu values to move the motor and reduce noise for if statement
  mpuy = round(euler.y()); //rounds the mpu values to move the motor and reduce noise for if statement
  
  Serial.print("MPU-Y =");
  Serial.println(mpuy);
//yservo
if (mpuy ==-2 || mpuy == -1 || mpuy == 0){
  return;
}

if (mpuy <= -3 && mpuy >= -90) {
  findy = mpulevely + mpuy;
  moveangy = (findy/2) + levely; 
  Serial.print("Moveangy=");
  Serial.print(moveangy);
  yservo.write(moveangy);
  counter = counter + 1;
  //Serial.print(counter);
  nmpuy = round(euler.y());
  Serial.print("NMPU-Y =");
  Serial.println(nmpuy); 
}
if (mpuy >=1 && mpuy <= 90) {
  findy = mpulevely + mpuy;
  moveangy = levely - (findy/2);
  yservo.write(moveangy);
  Serial.print("MPU-Y =");
  Serial.println(mpuy);

}

}
