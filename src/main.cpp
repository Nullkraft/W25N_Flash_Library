#include <Arduino.h>

// PA5 is connected to the onboard Green LED (LD2)
#define LED_PIN PA5

void setup() {
  pinMode(LED_PIN, OUTPUT); // Set PA5 as an output pin
}

void loop() {
  digitalWrite(LED_PIN, HIGH); // Turn the LED on
  delay(500);                  // Wait for 500 milliseconds
  digitalWrite(LED_PIN, LOW);  // Turn the LED off
  delay(500);                  // Wait for 500 milliseconds
}
