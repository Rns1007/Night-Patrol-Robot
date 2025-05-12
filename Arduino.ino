#include <Motor_Shield.h> // Include motor shield library

#define Duration 200    // Duration for exiting the loop
int data = A0;           // Sensor Input pin

DCMotor Amotor(1);      // Motor 1 (Left Motor)
DCMotor Bmotor(2);      // Motor 2 (Right Motor)

long previousMillis;   // Variables to scan for time alterations
long currentMillis;

// Bluetooth Serial communication
#include <SoftwareSerial.h>
SoftwareSerial BT(10, 11); // RX, TX pins for HC-05 Bluetooth

void setup() {
  Serial.begin(9600);
  BT.begin(38400);  // HC-05 default baud rate

  pinMode(data, INPUT);  // Configure sensor pin as input
  Serial.println("Bluetooth Robot Ready!");
}

char cmd;
volatile bool flag = 1;

void loop() {
  // Read Bluetooth commands
  if (BT.available()) {
    cmd = BT.read();  // Read incoming data
    Serial.print("Command: ");
    Serial.println(cmd);   // Print command in serial monitor
        
    if (cmd == 'G' || cmd == 'H' || cmd == 'I' || cmd == 'J' || cmd == 'K') {
      flag = 1;
      manual();
    }

    else if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R' || cmd == 'S') {
      flag = 1;
      voice();
    }

    // Execute command based on Bluetooth input
    
  }
}

void manual() {
  if (cmd == 'G') {
    Forward(200);
  } 
  else if (cmd == 'H') {
    Backward(200);
  } 
  else if (cmd == 'I') {
    Left();
  } 
  else if (cmd == 'J') {
    Right();
  } 
  else if (cmd == 'K') {
    Stop();
  }
}

void voice() {
  if (cmd == 'F') {
    Forward(200);
  } 
  else if (cmd == 'B') {
    Backward(200);
  } 
  else if (cmd == 'L') {
    VoiceLeft();
  } 
  else if (cmd == 'R') {
    VoiceRight();
  } 
  else if (cmd == 'S') {
    Serial.print("Stop?");
    Stop();
  }
}

// Forward movement
void Forward(unsigned char Speed) {
  Amotor.run(FORWARD);
  Bmotor.run(FORWARD);
  Amotor.setSpeed(Speed);
  Bmotor.setSpeed(Speed);
}

// Backward movement
void Backward(unsigned char Speed) {
  Amotor.run(BACKWARD);
  Bmotor.run(BACKWARD);
  Amotor.setSpeed(Speed);
  Bmotor.setSpeed(Speed);
}

// Turn Left
void Left(void) {
  Amotor.run(BACKWARD);
  Bmotor.run(FORWARD);
  Amotor.setSpeed(200);
  Bmotor.setSpeed(200);
}


// Turn Right
void Right(void) {
  Amotor.run(FORWARD);
  Bmotor.run(BACKWARD);
  Amotor.setSpeed(200);
  Bmotor.setSpeed(200);
}


// Stop all motors
void Stop(void) {
  Serial.println("Stopping Motors...");
  
  // Try stopping both motors individually
  Amotor.run(RELEASE);  // Release left motor
  Bmotor.run(RELEASE);  // Release right motor
  
  Amotor.setSpeed(0);   // Set speed of left motor to 0
  Bmotor.setSpeed(0);   // Set speed of right motor to 0
  
  delay(500);           // Add a small delay to ensure motors stop completely
}

void VoiceLeft(void) {
  Amotor.run(BACKWARD);
  Bmotor.run(FORWARD);
  Amotor.setSpeed(200);
  Bmotor.setSpeed(200);
  delay(400);     // Turn duration (adjust for desired turn sharpness)
  Stop();         // Stop after turning
}

void VoiceRight(void) {
  Amotor.run(FORWARD);
  Bmotor.run(BACKWARD);
  Amotor.setSpeed(200);
  Bmotor.setSpeed(200);
  delay(400);     // Turn duration (adjust for desired turn sharpness)
  Stop();         // Stop after turning
}