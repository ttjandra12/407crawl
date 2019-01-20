/* This code is a general outline to read/write data to and from an SD card file.
 *  
 * Pinouts (Arduino Mega):
 * 
 *  BUTTONS
 *  - button: 7 (press to collect data)
 *  - print: 8 (press to display data on serial monitor)
 *  SD CARD READER
 *  - CLK: 52
 *  - DO: 50
 *  - DI: 51
 *  - CS: 53
 *  ToF SENSOR
 *  - SDA: 20
 *  - SCL: 21
 *  
 */


#include <SPI.h>
#include <SD.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

File myFile;

const int buttonPin = 7;
const int printPin = 8;

void setup() {

  pinMode(buttonPin, INPUT);
  pinMode(printPin, INPUT);

  //Open Serial monitor 
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Initializing SD card reader
  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //Initializing ToF sensor
  Serial.print("Initializing ToF sensor...");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("initialization done.")); 

}

void loop() {

  int buttonState = digitalRead(buttonPin);
  int printState = digitalRead(printPin);

  VL53L0X_RangingMeasurementData_t measure;

  //data collected will be stored as a string formatted as a CSV
  String dataString = "";

  //press button connected to pin 7 to collect and record data
  if (buttonState == HIGH){

    //this for loop collects data from the 3 sensors 
    for (int i = 0; i < 3; i++){
      //collects data from ToF sensor
      if (i == 0){
        lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
        float sensor = measure.RangeMilliMeter;
        dataString += sensor;
      } else{ //dummy code in place for 2 more sensors
        dataString += ",";
        float sensor = random(1,10);
        dataString += sensor;
      }
    }
    //opening file on SD card and writing data string onto it
    myFile = SD.open("test1.csv", FILE_WRITE);
    if (myFile){
      myFile.println(dataString);
      myFile.close();
      Serial.println("complete");
      delay(1000);
    } else {
      Serial.println("error opening test1.csv");
    }
  }

  //press button connected to pin 8 to display the values saved in the SD card
  if (printState == HIGH){
    myFile = SD.open("test1.csv");
    if (myFile){
      Serial.println("test1.csv:");
      while (myFile.available()){
        Serial.write(myFile.read());
      }
      myFile.close();
      delay(1000);
    } else {
      Serial.println("error opening test1.csv");
    }
  }

////use this code to delete the csv file
//  if (buttonState == HIGH){
//    SD.remove("test1.csv");
//  }

}
