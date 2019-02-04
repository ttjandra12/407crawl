    /*
      DIY Digital Range Measurer and Spirit Level
      by Dejan Nedelkovski,
      www.HowToMechatronics.com
    */
    #include<Wire.h> // I2C communication library
    #include <LiquidCrystal.h> // includes the LiquidCrystal Library
    #include "Adafruit_VL6180X.h"
    #include <KalmanFilter.h>
    #include "Arduino.h"
    #include <HP20x_dev.h>
    #include "Wire.h"
    
    /* Instance */
    KalmanFilter t_filter;    //temperature filter
    KalmanFilter p_filter;    //pressure filter
    KalmanFilter a_filter;    //altitude filter

    Adafruit_VL6180X vl = Adafruit_VL6180X();
    LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
    const int MPU = 0x68; // I2C address of the MPU6050 accelerometer
    #define trigPin 8
    #define echoPin 9
    #define selectButton 10
    #define intPin 11
    int16_t AcX, AcY, AcZ;
    long duration;
    float distance;
    int program = 0;
    float d = 0;
    float d1 = 0;
    float d2 = 0;
    float area = 0;
    int axis = 0;
    int angle = 0;
    int unitSelect = 0;
    String unit = "cm";
    uint8_t range;
    uint8_t status;
    float t;
    long Altitude;
    unsigned char ret;
    
void setup() {
      // Initialize interface to the MPU6050
      pinMode(intPin, INPUT);
      Wire.begin();
      Wire.beginTransmission(MPU);
      Wire.write(0x6B);
      Wire.write(0);
      Wire.endTransmission(true);
      
      lcd.begin(16, 2); // Initializes the interface to the LCD screen
      
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
      pinMode(selectButton, INPUT_PULLUP);

      delay(150);
      HP20x.begin();
      delay(100);
    }
    
    
    void loop() {
      switch (program) {  // Switch between different programs
        case 0:  // Select unit of measurement
          lcd.setCursor(0, 0); // Sets the location at which subsequent text written to the LCD will be displayed
          lcd.print("Select Unit: ");
          lcd.setCursor(13, 0);
          lcd.print(unit);
          lcd.print("  ");
          delay(10);
          // If button is pressed - change unit
          if (digitalRead(selectButton) == 0) {
            if (unitSelect == 0) {
              unit = "in";
              unitSelect = 1;
            }
            else if (unitSelect == 1) {
              unit = "m";
              unitSelect = 2;
            }
            else if (unitSelect == 2) {
              unit = "ft";
              unitSelect = 3;
            }
            else if (unitSelect == 3) {
              unit = "cm";
              unitSelect = 0;
            }
            // If button is held longer then half a second - change program
            delay(500);
            if (digitalRead(selectButton) == 0) {
              program = 1;
              lcd.clear();
              delay(500);
            }
          }
          break;
          
        case 1:   // Distance measuring program
          distance = getDistance(); // Distance to the nearest object
          lcd.setCursor(0, 0);
          lcd.print("Dist: ");
          lcd.print(distance); // Prints the distance value from the sensor
          lcd.print("    ");
          lcd.setCursor(14, 0);
          lcd.print(unit);
          delay(10);
          lcd.setCursor(0, 1);
          lcd.print("d:");
          lcd.setCursor(8, 1);
          lcd.print("d:");
          delay(200);
          // Save distance 1
          if (digitalRead(selectButton) == 0) {
            if (d == 0) {
              lcd.setCursor(0, 1);
              lcd.print("d:     ");
              lcd.setCursor(2, 1);
              lcd.print(distance);
              d = 1;
              delay(100);
            }
            // Save distance 2
            else if (d == 1) {
              lcd.setCursor(8, 1);
              lcd.print("d:     ");
              lcd.setCursor(10, 1);
              lcd.print(distance);
              d = 0;
              delay(100);
            }
            // If button is held longer then half sencond change program
            delay(500);
            if (digitalRead(selectButton) == 0) {
              program = 2;
              d = 0;
              lcd.clear();
              delay(500);
            }
          }
          break;
        case 2:   // Laser measuring program          
          vl.begin();
          
          range = vl.readRange();
          status = vl.readRangeStatus();

          delay(200);
          
          if (status == VL6180X_ERROR_NONE) {
            lcd.setCursor(0,0);
            lcd.print(range);
            lcd.print("mm   ");
            delay(100);
          } 
          else {
            lcd.clear();
            lcd.print("NO DATA");
            delay(100);
            return;
          }
          // If button is held longer then half sencond change program
          delay(500);
          if (digitalRead(selectButton) == 0) {
              program = 3;
              d = 0;
              lcd.clear();
              delay(500);
          }
          break;
          
        case 4:
          delay(150);
          /* Reset HP20x_dev */
          HP20x.begin();
          delay(100);

          ret = HP20x.isAvailable();
          if(OK_HP20X_DEV == ret)
          {
          lcd.print("Sensor Good");
          Altitude = HP20x.ReadAltitude();
          lcd.setCursor(0,0);
          lcd.print("Alt: ");
          t = Altitude/100.0;
          lcd.print(t);
          lcd.print("m");
          lcd.setCursor(0,1);
          lcd.print("Filter: ");
          lcd.print(a_filter.Filter(t));
          lcd.print("m");
          }
          else
          {
          lcd.print("Sensor Bad");
          }
          
          delay(1000);
            // If button is held longer then half sencond change program
            delay(500);
            if (digitalRead(selectButton) == 0) {
              program = 0;
              d = 0;
              lcd.clear();
              delay(500);
          }        
        break;
        case 3:    // Angle measuring program
          // Read the accelerometer data
          Wire.beginTransmission(MPU);
          Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
          Wire.endTransmission(false);
          Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
          AcX = Wire.read() << 8 | Wire.read(); // X-axis value
          AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
          AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value
         
          if ( axis == 0) {
            // Calculating the Pitch angle (rotation around Y-axis)
            angle = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
            lcd.setCursor(0, 0);
            lcd.print("Pitch");
          }
          else if (axis == 1) {
            // Calculating the Roll angle (rotation around X-axis)
            angle = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;
            lcd.setCursor(0, 0);
            lcd.print("Roll ");
          }
          lcd.setCursor(0, 1);
          lcd.print("Angle: ");
          lcd.print(abs(angle));
          lcd.print("     ");
          lcd.setCursor(10, 1);
          lcd.print("deg");
          delay(200);
          // Change axis
          if (digitalRead(selectButton) == 0) {
            if (axis == 0) {
              axis = 1;
              delay(100);
            }
            // Save distance 2
            else if (axis == 1) {
              axis = 0;
              delay(100);
            }
            // If button is held longer then half sencond change program
            delay(500);
            if (digitalRead(selectButton) == 0) {
              program = 4;
              lcd.clear();
              delay(500);
            }
          }
          break;
      }
    }
    //===== getDistance - Custom Function
    float getDistance() {
      // Clears the trigPin
      digitalWrite(trigPin, LOW);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      distance = duration * 0.034 / 2; // distance in cm
      
      // Converting the units
      if (unitSelect == 1) {
        distance = distance; // cm to cm
        unit = "cm";
      }
      else if (unitSelect == 2) {
        distance = distance *  0.393701; // cm to in
        unit = "in";
      }
      else if (unitSelect == 3) {
        distance = distance * 0.01; // cm to m
        unit = "m";
      }
      else if (unitSelect == 0) {
        distance = distance * 0.0328; // cm to ft
        unit = "ft";
      }
      return distance;
    }
