/******************************************************************************
Mux_Analog_Input
SparkFun Multiplexer Analog Input Example
Jim Lindblom @ SparkFun Electronics
August 15, 2016
https://github.com/sparkfun/74HC4051_8-Channel_Mux_Breakout

This sketch demonstrates how to use the SparkFun Multiplexer
Breakout - 8 Channel (74HC4051) to read eight, separate
analog inputs, using just a single ADC channel.

Hardware Hookup:
    Mux ------------------- Arduino
     S0 ------------------- 36
     S1 ------------------- 37
     S2 ------------------- 38
     Y1 ------------------- 39
     Y2 ------------------- 40
    VCC ------------------- 5V
    GND ------------------- GND


The multiplexers independent I/O (Y0-Y7) can each be wired
up to a potentiometer or any other analog/digital signal-producing
component.


******************************************************************************/
// Pin Definitions
const int selectPins[3] = {36, 37, 38}; // S0~2, S1~3, S2~4
const int Y1Input = 39; // Connect output Y1 to 5
const int Y2Input = 40; // Connect output Y2 to 6 

void setup() 
{
  Serial.begin(9600); // Initialize the serial port
  
  // Set up the select pins as outputs:
  for (int i=0; i<3; i++)
  {
    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
  pinMode(Y1Input, INPUT); // Set up Y1 as an input
  pinMode(Y2Input, INPUT); // Set up Y2 as an input


  // Print the header:
  Serial.println("Y1.0\tY2.0\tY1.1\tY2.1\tY1.2\tY2.2\tY1.3\tY2.3\tY1.4\tY2.4\tY1.5\tY2.5\tY1.6\tY2.6\tY1.7\tY2.7");
  Serial.println("---\t---\t---\t---\t---\t---\t---\t---\t---\t---\t---\t---\t---\t---\t---\t---");
}

void loop() 
{
  // Loop through all eight pins
  for (byte pin=0; pin<=7; pin++)
  {
    selectMuxPin(pin); // Select one at a time
    int inputY1Value = digitalRead(39); // and read Y1
    int inputY2Value = digitalRead(40); // and read Y2
    Serial.print(String(inputY1Value) + "\t" + String(inputY2Value)+ "\t");
  
  }
  Serial.println();
  delay(1000);
}

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7.
void selectMuxPin(byte pin)
{
  for (int i=0; i<3; i++)
  {
    if (pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}
