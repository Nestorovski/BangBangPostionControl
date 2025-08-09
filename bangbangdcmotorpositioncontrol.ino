//Made by Ivan Nestorovski
//You can change this according to your needs of use and precission needed, i had a big problem with the position control of the N20 DC Motors so i think it might help others too. Enjoy your projects.

#include <AFMotor.h> // For all similar shields, you may need to install it first

AF_DCMotor motor(1); // Change "1" to the pin connected to your motor driver
unsigned long previousMillis = 0;
const long interval = 20; // Adjust the update interval in milliseconds
int target;
int newTarget;
bool newCommandAvailable = false;
int lastError = 0;
int tolerance = 10; // Tolerance for the error

void setup() {
  Serial.begin(57600);
  motor.setSpeed(255); // You can set your speed here (0-255)
  int initialPotValue = analogRead(A0); // Get the initial potentiometer reading
  Serial.print("Initial potentiometer value: ");
  Serial.println(initialPotValue);
}

void loop() {
  unsigned long currentMillis = millis();

  if (Serial.available()) {
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') {
        target = newTarget; // Update the target when a newline character is received
        newCommandAvailable = true;
        break;
      }
      else if (c >= '0' && c <= '9') {
        newTarget = newTarget * 10 + c - '0'; // Update newTarget instead of target
        newTarget = constrain(newTarget, 0, 1023); // Ensure newTarget is within the range of the potentiometer
      }
    }
    if (newCommandAvailable) {
      Serial.print("New target value: ");
      Serial.println(target);
    }
  }

  if (currentMillis - previousMillis >= interval && newCommandAvailable) {
    previousMillis = currentMillis;

    int potValue = analogRead(A0);
    int error = target - potValue;

    Serial.print("Current value: ");
    Serial.print(potValue);
    Serial.print(", Desired value: ");
    Serial.print(target);
    Serial.print(", Error: ");
    Serial.println(error);

    if (abs(error) > tolerance) { // Change this condition to use the tolerance
      if (error < 0 && error < lastError) {
        motor.run(FORWARD);
      } else if (error > 0 && error > lastError) {
        motor.run(BACKWARD);
      }
    } else {
      motor.run(RELEASE); // Set motor to idle when error is small
      newCommandAvailable = false; // Reset the flag after each command
      newTarget = 0; // Reset the newTarget value after each command
    }
    lastError = error; // Store the last error
  }
}
