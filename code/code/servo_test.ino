#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(9); // Connect servo signal wire to pin 9
}

void loop() {
  myservo.write(5);      // Move to 0 degrees
  delay(3000);           // Wait for 3 seconds

  myservo.write(180);    // Move to 180 degrees
  delay(3000);           // Wait for 3 seconds
}
