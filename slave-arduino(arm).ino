#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDRESS 0x08  // Must match the Master's address

struct MyData {
  byte X;
  byte Y;
  byte Z;
  byte buttonState;  // 1 = pressed, 0 = unpressed
};

MyData receivedData;
boolean newData = false;

Servo zServo;  // Z-axis servo (pin 9)
Servo yServo;  // Y-axis servo (pin 8)
Servo buttonServo; // Button-controlled servo (pin 7)

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Serial.println("Slave Arduino Ready (Three Servos)");
  
  zServo.attach(9);  // Z-axis servo
  yServo.attach(8);  // Y-axis servo
  buttonServo.attach(7);  // Button-controlled servo
  pinMode(LED_BUILTIN, OUTPUT); // Optional LED
}

void loop() {
  if (newData) {
    // Update LED based on button state (optional)
    digitalWrite(LED_BUILTIN, receivedData.buttonState ? HIGH : LOW);
    
    // Debug output
    Serial.print("Button: ");
    Serial.print(receivedData.buttonState ? "PRESSED" : "UNPRESSED");
    Serial.print(" | X: ");
    Serial.print(receivedData.X);
    Serial.print(" | Y: ");
    Serial.print(receivedData.Y);
    Serial.print(" | Z: ");
    Serial.println(receivedData.Z);

    // **Z-axis Servo (40-245 → 50-140°)**
    int zValue = constrain(receivedData.Z, 40, 245);
    int zAngle = map(zValue, 40, 245, 50, 160);
    zServo.write(zAngle);

    // **Y-axis Servo (30-205 → 0-180°)**
    int yValue = constrain(receivedData.Y, 30, 205);
    int yAngle = map(yValue, 30, 205, 180, 0);
    yServo.write(yAngle);

    // **Button-controlled Servo (pin 7)**
    if (receivedData.buttonState == 1) {
      buttonServo.write(90);  // Hold at 90° when pressed
      Serial.println("Button Servo: 90° (PRESSED)");
    } else {
      buttonServo.write(35);  // Rest at 35° when unpressed
      Serial.println("Button Servo: 35° (UNPRESSED)");
    }

    newData = false;  // Reset flag
  }
  delay(20);  // Small delay for stability
}

// I2C receive event handler
void receiveEvent(int howMany) {
  if (howMany == sizeof(MyData)) {
    byte* bytePointer = (byte*)&receivedData;
    for (int i = 0; i < sizeof(MyData); i++) {
      *(bytePointer + i) = Wire.read();
    }
    newData = true;
  }
  while (Wire.available()) Wire.read();  // Clear buffer
}