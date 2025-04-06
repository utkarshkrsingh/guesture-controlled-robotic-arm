#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"    

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
const int buttonPin = 7;  
const int ledPin = 13; 

#define SLAVE_ADDRESS 0x08  // Define slave address for Arduino-2

struct MyData {
  byte X;
  byte Y;
  byte Z;
  byte buttonState;
};

MyData data;

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); 
  Serial.begin(9600);
  Wire.begin();  // Initialize as master (no address)
  mpu.initialize();
}

void loop()
{
  // Read button state
  data.buttonState = digitalRead(buttonPin) == LOW ? 1 : 0;  
  
  // Update LED based on button state
  digitalWrite(ledPin, data.buttonState ? HIGH : LOW);
  
  // Print button state to serial
  Serial.println(data.buttonState ? "ON" : "OFF");

  // Read MPU6050 data
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.X = map(ax, -17000, 17000, 0, 255); // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255); // Y axis data
  data.Z = map(az, -17000, 17000, 0, 255); // Z axis data
  
  // Send data to Arduino-2 via I2C
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write((byte *)&data, sizeof(data));
  Wire.endTransmission();
  
  // Print data to serial monitor
  Serial.print("Axis X = ");
  Serial.print(data.X);
  Serial.print("  ");
  Serial.print("Axis Y = ");
  Serial.print(data.Y);
  Serial.print("  ");
  Serial.print("Axis Z = ");
  Serial.println(data.Z);
  
  delay(500);
}