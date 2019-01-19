//This code is a general outline to read/write data to and from an SD card file.

#include <SPI.h>
#include <SD.h>

File myFile;

const int buttonPin = 7;
const int printPin = 8;

void setup() {
  // Open serial communications and wait for port to open:

  pinMode(buttonPin, INPUT);
  pinMode(printPin, INPUT);
  
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
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(buttonPin);
  int printState = digitalRead(printPin);

  String dataString = "";
  
  if (buttonState == HIGH){
    
    for (int i = 0; i < 3; i++){
      int sensor = random(1,10);
      dataString += String(sensor);
      if (i < 2) {
        dataString += ",";
      }
    }

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

//  if (printState == HIGH){
//    SD.remove("test1.csv");
//  }

}
