//This will sketch will read all 6 channels of a RC reciever and input the values via serial monitor.
//Programed for the Arduino UNO

// Define Variables:
const int chA=7;  //Constant variables relating to pin locations
const int chB=2;
const int chC=3;
const int chD=4;
const int chE=5;
const int chF=6;

int ch1;  //Varibles to store and display the values of each channel
int ch2;
int ch3;
int ch4;
int ch5;
int ch6;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  // Set input pins
  pinMode(chA, INPUT);
  pinMode(chB,INPUT);
  pinMode(chC,INPUT);
  pinMode(chD,INPUT);
  pinMode(chE,INPUT);
  pinMode(chF,INPUT);
}

//Main Program
void loop() {
  // read the input channels
 
  ch1 = pulseIn (chA,HIGH);  //Read and store channel 1
  Serial.print ("Ch1:");  //Display text string on Serial Monitor to distinguish variables
  Serial.print (ch1);     //Print in the value of channel 1
  Serial.print ("|");
  
  ch2 = pulseIn (chB,HIGH);
  Serial.print ("Ch2:");
  Serial.print (ch2);
  Serial.print ("|");
  
  ch3 = pulseIn (chC,HIGH);
  Serial.print ("Ch3:");
  Serial.print (ch3);
  Serial.print ("|");

  ch4 = pulseIn (chD,HIGH);
  Serial.print ("Ch4:");
  Serial.print (ch4);
  Serial.print ("|");
 
  ch5 = pulseIn (chE,HIGH);
  Serial.print ("Ch5:");
  Serial.print (ch5);
  Serial.print ("|");
 
  ch6 = pulseIn (chF,HIGH);
  Serial.print ("Ch6:");
  Serial.print (ch6);
  Serial.print ("|");
  
  delay(1000);
  
}

