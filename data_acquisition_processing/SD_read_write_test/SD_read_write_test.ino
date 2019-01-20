/* This code is a general outline to read/write data to and from an SD card file.
 *  
 * Pinouts (Arduino Mega):
 *  - button: 7 (press to collect data)
 *  - print: 8 (press to display data on serial monitor)
 *  - CLK: 52
 *  - DO: 50
 *  - DI: 51
 *  - CS: 53
 *  
 */


#include <SPI.h>
#include <SD.h>

File myFile;

const int buttonPin = 7;
const int printPin = 8;

void setup() {

  pinMode(buttonPin, INPUT);
  pinMode(printPin, INPUT);

  //Open Serial monitor and check that SD card is initialized
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

}

void loop() {

  int buttonState = digitalRead(buttonPin);
  int printState = digitalRead(printPin);

  //the random numbers generated will be stored in a string formatted as a CSV
  String dataString = "";

//  //press button connected to pin 7 to generate three random numbers and have it saved on the SD card
//  if (buttonState == HIGH){
//
//    //for loop to generate random numbers
//    for (int i = 0; i < 3; i++){
//      int sensor = random(1,10);
//      dataString += String(sensor);
//      if (i < 2) {
//        dataString += ",";
//      }
//    }
//
//    //opening file on SD card and writing data string onto it
//    myFile = SD.open("test1.csv", FILE_WRITE);
//    if (myFile){
//      myFile.println(dataString);
//      myFile.close();
//      Serial.println("complete");
//      delay(1000);
//    } else {
//      Serial.println("error opening test1.csv");
//    }
//  }

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

//use this code to delete the csv file
  if (buttonState == HIGH){
    SD.remove("test1.csv");
  }

}
