//Include libraries
#include "VL53L0X.h"            //Changed library .h file (previous: "VL53L0X.h")
#include <Wire.h>
#include <VL53L0X.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Set up all the servos
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
VL53L0X Scissor_Sensor;

File myFile;

//Constant variables relating to pin locations
const int chA = 30;
const int chB = 31;
const int chC = 32;
const int chD = 33;
const int chE = 34;
const int chF = 35;

//Varibles to store and display the values of each channel
int ch1;
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

//Define the intial position and the angular increment for the camera servos
int servoPosition1 = 90;
int servoPosition2 = 90;
int servoIncrement = 6;

int ch5_change = 700;
int ch5_PrevRead = 1200;
int ch5_CurrRead = 0;

int ch6_change = 700;
int ch6_PrevRead = 1200;
int ch6_CurrRead = 0;
int vel = 40;
int vel2 = 60;

int pinFeedback = 8; //yellow cable of the continous servo
int pinControl = 7; //white cable of the continous servo

float recorded_height;

//The trigger will be used to run the continuous servo and the firt_height trigger to record the initial height of the scissor arm
int trigger = 0;
int trigger_first_height = 0;

//Define all the individual voids used in the code
void feedback360();
void pin_ISR();
void level();
void return_func();
void selectMuxPin(byte pin);

// Interrupt function
void pin_ISR() {
  trigger = 1;
}

//Variables used in the sensor carousel
const int selectPins[3] = {43, 45, 47}; // S0~2, S1~3, S2~4
const int Y1Input = 44; // Connect output Y1
const int Y2Input = 46; // Connect output Y2

//Set triggers for the level function
int y_trigger = 0;
int x_trigger = 0;

//Variable used to control the scissor jack
int count = 0;

void setup() {

  Serial.begin(9600);

  // Set input pins
  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(chC, INPUT);
  pinMode(chD, INPUT);
  pinMode(chE, INPUT);
  pinMode(chF, INPUT);

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
  servo1.attach(10);
  servo2.attach(11);
  xservo.attach(13);
  yservo.attach(12);


  //Set the pins of the ToF sensors as outputs and give them a low value
  pinMode(9, OUTPUT); // Pin for ToF sensor
  pinMode(6 , OUTPUT); //Pin for scissor sensor
  pinMode(8, OUTPUT); //Pin for NPU

  digitalWrite(9, LOW);
  digitalWrite(6, LOW);
  digitalWrite(8, HIGH);


#define HIGH_ACCURACY
  Wire.begin();

  //Setup ToF Sensor and give it an address
  pinMode(9, INPUT);
  delay(150);
  ToF_sensor.init(true); //WTF
  delay(100);
  ToF_sensor.setAddress((uint8_t)22);
  ToF_sensor.setMeasurementTimingBudget(200000);


  //Setup Scissor Sensor and give it an address
  pinMode(6, INPUT);
  delay(150);
  Scissor_Sensor.init(true);// WTF 2.0
  delay(100);
  Scissor_Sensor.setAddress((uint8_t)25);
  Scissor_Sensor.setMeasurementTimingBudget(200000);

  //Initialize the SD card
  Serial.print("Initializing SD card....");
  if (!SD.begin(53)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Set up the select pins from the sensor carousel as outputs:
  for (int i = 0; i < 3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }

  pinMode(Y1Input, INPUT); // Set up Y1 as an input
  pinMode(Y2Input, INPUT); // Set up Y2 as an input

  //Stuff for the leveling platform
  bno.setExtCrystalUse(true);
  levelx = 120; // initial x level
  levely = 120; //initial y level

  xservo.write(levelx); //moves x servo
  yservo.write(levely); //moves y servo

  // Initialise the sensor
  if (!bno.begin())
  {
    //There was a problem detecting the BNO055 ... check your connections
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    //while (1);
  }

  delay(1000);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();


  bno.setExtCrystalUse(true);
}

void loop() {
  //Get the initial height of the scissor jack. The if statement will only loop once
  if (trigger_first_height == 0)
  {
    float initial_height = Scissor_Sensor.readRangeSingleMillimeters();
    trigger_first_height = 1;
    Serial.print("The initial height of the Scissor Jack is: ");
    Serial.println(initial_height);
    recorded_height = initial_height;

    //Write intial height into the SD card
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
      String Str = String(initial_height);
      myFile.println(Str);
      myFile.close();
      Serial.println("Complete");
      delay(1000);
    }
    else {
      Serial.println("FUCKKKKKKK");
    }

    //Calibrate the MPU
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    uint8_t system, gyro, accel, mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);
    Serial.println("MPU Calibrated");

  }
  ch1 = pulseIn (chA, HIGH); //Read and store channel 1
  ch2 = pulseIn (chB, HIGH); //Read and store channel 2
  ch3 = pulseIn (chC, HIGH); //Read and store channel 3
  ch4 = pulseIn (chD, HIGH); //Read and store channel 4
  ch5 = pulseIn (chE, HIGH); //Read and store channel 5
  ch6 = pulseIn (chF, HIGH); //Read and store channel 6

  //Motor Control
  if (ch4 > 1300 && ch4 < 1700)
  {
    if (ch3 < 1300)
    {
      // turn on motor one
      analogWrite(enA, vel);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);

      // turn on motor two
      analogWrite(enB, vel);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);

      // turn on motor three
      analogWrite(enC, vel);
      digitalWrite(in5, LOW);
      digitalWrite(in6, HIGH);

      // turn on motor four
      analogWrite(enD, vel);
      digitalWrite(in7, HIGH);
      digitalWrite(in8, LOW);
    }

    if (ch3 > 1300 && ch3 < 1700)
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

    if (ch3 > 1700)
    {
      // turn on motor one, reverse
      analogWrite(enA, vel);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);

      // turn on motor two, reverse
      analogWrite(enB, vel);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);

      // turn on motor three, reverse
      analogWrite(enC, vel);
      digitalWrite(in5, HIGH);
      digitalWrite(in6, LOW);

      // turn on motor four, reverse
      analogWrite(enD, vel);
      digitalWrite(in7, LOW);
      digitalWrite(in8, HIGH);
    }
  }

  if (ch4 < 1300)
  {
    // turn on motor one, forward
    analogWrite(enA, vel2);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);

    // turn on motor two, reverse
    analogWrite(enB, vel2);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    // turn on motor three, reverse
    analogWrite(enC, vel2);
    digitalWrite(in5, HIGH);
    digitalWrite(in6, LOW);

    // turn on motor four, forward
    analogWrite(enD, vel2);
    digitalWrite(in7, HIGH);
    digitalWrite(in8, LOW);
  }

  if (ch4 > 1700)
  {
    // turn on motor one, reverse
    analogWrite(enA, vel2);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);

    // turn on motor two, forward
    analogWrite(enB, vel2);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    // turn on motor three, forward
    analogWrite(enC, vel2);
    digitalWrite(in5, LOW);
    digitalWrite(in6, HIGH);

    // turn on motor four, reverse
    analogWrite(enD, vel2);
    digitalWrite(in7, LOW);
    digitalWrite(in8, HIGH);
  }

  //Control of the first servo motor in the FPV camera (up and down)
  if (ch1 > 1700)
  {
    servoPosition1 += servoIncrement;
    if (servoPosition1 > 180)
    {
      servoPosition1 = 180;
    }
    servo1.write(servoPosition1);
  }

  if (ch1 < 1300)
  {
    servoPosition1 -= servoIncrement;

    if (servoPosition1 < 0)
    {
      servoPosition1 = 0;
    }

    servo1.write(servoPosition1);
  }

  //Control of the second servo motor in the FPV camera (left and right)
  if (ch2 > 1700)
  {
    servoPosition2 += servoIncrement;

    if (servoPosition2 > 180)
    {
      servoPosition2 = 180;
    }

    servo2.write(servoPosition2);
  }

  if (ch2 < 1300)
  {
    servoPosition2 -= servoIncrement;

    if (servoPosition2 < 0)
    {
      servoPosition2 = 0;
    }

    servo2.write(servoPosition2);
  }

  //Read the CH6 input
  ch6_CurrRead = ch6;
  int delta_ch6 = abs(ch6_CurrRead - ch6_PrevRead);

  //if the switch changes position the scissor jack will start moving.
  if (delta_ch6 > ch6_change)
  {
    cont_servo.attach(7);
    cont_servo.write(150);
    feedback360();
    trigger = 0;

    //Record the height of the scissor jack after it has hit the laser
    recorded_height = Scissor_Sensor.readRangeSingleMillimeters();
    Serial.print("The Recorded Height of the Scissor Jack is: ");
    Serial.println(recorded_height);
    delay(1000);

    cont_servo.attach(7);
    cont_servo.write(0);
    delay(3000);
    return_func();

    ch6_PrevRead = ch6_CurrRead;
  }

  //Read the CH5 input
  ch5_CurrRead = ch5;
  int delta_ch5 = abs(ch5_CurrRead - ch5_PrevRead);

  //if the nob is turned past a certain point, record sensor readouts.
  if (delta_ch5 > ch5_change )
  {
    String dataString = "";

    //this for loop collects data from the 3 sensors
    for (int i = 0; i < 3; i++) {
      Serial.println(String(i));

      //Data from the ToF Sensor
      if (i == 1) {
        //while (y_trigger == 0 && x_trigger == 0) {
        //level();
        //}
        float sensor = ToF_sensor.readRangeSingleMillimeters();
        dataString += "," + String(sensor);
        Serial.print("The Height Measurement is: ");
        Serial.println(sensor);

        //Re attach the x and y servos
        //xservo.attach(11);
        // yservo.attach(10);

        //Reset triggers
        //y_trigger = 0;
        //x_trigger = 0;
      }
      //Data from the NPU in the leveling platform
      if (i == 0) {

        imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

        float sensor1 = euler.y(); //pitch in degrees
        float sensor2 = euler.z(); //roll in degrees
        dataString += String(sensor1) + "," + String(sensor2);

        uint8_t system, gyro, accel, mag = 0;
        bno.getCalibration(&system, &gyro, &accel, &mag);
      }
      //Height from the scissor sensor
      if (i == 2) {
        dataString += ",";
        float height = recorded_height;
        dataString += String(height);
      }
    }

    //opening file on SD card and writing data string onto it
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
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

void feedback360() {
  while (1)
  {
    count += 1;
    // Loop through all eight pins from the carousel.
    for (byte pin = 0; pin <= 7; pin++)
    {
      selectMuxPin(pin); // Select one at a time
      int inputY1Value = digitalRead(44); // and read Y1
      int inputY2Value = digitalRead(46); // and read Y2

      Serial.print(String(inputY1Value) + "\t" + String(inputY2Value) + "\t");


      if (inputY2Value == 1 || inputY1Value == 1) {
        trigger = 1;
      }
    }
    Serial.println();
    //Dummy system to control the scissor jack:
    //When the number of loops exceeds 1300, the scissor jack will start to come down
    if (count == 800) {
      cont_servo.write(0);          // Make the servo go backwards
      count += -1600;
    }
    if (count == 0) {
      cont_servo.write(150);          // Make the servo go forward
    }

    if (trigger) {                      // Display angle in serial monitor if button presed
      cont_servo.write(90);          // Stop the servo
      cont_servo.detach();
      delay(1000);
      break;
    }
  }
}

void return_func() {
  while (1)
  {
    //Dummy system to control the scissor jack:
    //The counter will starto go to zero and once it reaches zero the servo will stop
    if (count > 0) {
      count -= 1;
    }

    if (count < 0) {
      count += 1;
    }
    Serial.println(count);
    
    if (count == 0) {
      cont_servo.write(90); // Make the servo stop
      cont_servo.detach();
      break;
    }
  }
}

void selectMuxPin(byte pin)
{
  for (int i = 0; i < 3; i++)
  {
    if (pin & (1 << i)) {
      digitalWrite(selectPins[i], HIGH);
    }
    else {
      digitalWrite(selectPins[i], LOW);
    }
  }
}

void level()
{
  //while (y_trigger == 0 && x_trigger == 0) {

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */

  mpux = round(euler.z()); //rounds the mpu values to move the motor and reduce noise for if statement
  mpuy = round(euler.y()); //rounds the mpu values to move the motor and reduce noise for if statement

  // yservo

  if (mpuy < -2 && mpuy >= -90) {
    if (mpuy == -3) {
      return;  //if it is already level, it ends the code
    }
    if (mpuy < -3 && mpuy >= -20) {
      levely = levely + 1;
      yservo.write(levely);
    }
    else {
      levely = levely + 10; // if problem change to 1 rather than 10
      yservo.write(levely);//moves the y motor to one side until it is level
      delay(1000);

    }
  }


  if (mpuy <= 90 && mpuy > -2) {
    if (mpuy == 0 || abs(mpuy) == 1 || mpuy == 2 || mpuy == 3 || mpuy == 4) {
      return; //if level already, moves out
    }
    if (mpuy <= 20 && mpuy > 4) {
      levely = levely - 1;
      yservo.write(levely);
    }
    else {
      levely = levely - 10; //if problem change to 1 rather than 10
      yservo.write(levely); // moves the y motor to one side until it is level
      delay(1000);
    }
  }

  Serial.print("MPU-Y=");
  Serial.println(mpuy);

  //xservo
  if (mpux > -180 && mpux <= - 90) {
    if (mpux == -180 || mpux == -179 || mpux == -178) {
      return; //if x side is level, moves out of the if statement
    }
    if (mpux > -178 && mpux <= -168) {
      levelx = levelx - 1;
      xservo.write(levelx);
    }
    else {
      levelx = levelx - 10; //if problematic change to 1 rathen than 10
      xservo.write(levelx); //moves the x motor to one side until it is level
      delay(1000);
    }
  }

  if (mpux >= 90 && mpux < 180) {
    if (mpux == 180 || mpux == 179) {
      return; // if x side is level, moves out of the if statement
    }
    if (mpux > 168 && mpux < 178 ) {
      levelx = levelx + 1;
      xservo.write(levelx);
    }
    else {
      levelx = levelx + 10; //if problematic change to 1 rather than 10
      xservo.write(levelx); //moves the x motor to one side until it is level
      delay(1000);
    }
  }

  Serial.print("MPU-X=");
  Serial.println(mpux);

  delay(1000);

  if (mpuy == -2 || mpuy == -3 || mpuy == 0 || mpuy == 1 || mpuy == 2 || mpuy == 3 || mpuy == 4) {
    yservo.detach();
    Serial.println("DONE WITH Y");
    y_trigger = 1;

  }

  if (mpux == -180 || mpux == -179 || mpux == -178 || mpux == 180 || mpux == 179) {
    xservo.detach();
    Serial.print("DONE WITH X");
    x_trigger = 1;

  }

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0; //calibrates the MPU, took straight from the example code
  bno.getCalibration(&system, &gyro, &accel, &mag);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}
//}

