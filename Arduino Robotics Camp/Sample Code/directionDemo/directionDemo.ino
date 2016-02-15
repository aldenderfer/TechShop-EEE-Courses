/* Direction Demo
 * Written by Kristof Aldenderfer and Luca Angeleri
 * Summer 2015
 * 
 * Students are provided with a skeleton of code, which will make their robot drive forward.
 * They must code the rest of the directions so that the robot can also turn left, turn right, and go backward.
 */

byte leftMotorPins[3]   = {21, 18, 19};     // pins connected to the left motor controller
byte rightMotorPins[3]  = {20, 17, 16};     // pins connected to the right motor controller
byte maxSpeed           = 127;              // maximum speed of the robot (possible values: 0 to 255)
byte timeToMove         = 2000;             // amount of time (in milliseconds) which each movement command will be run for

// for this demo, setup only needs to assign pins
void setup() {
  for (int i=0 ; i<3 ; i++) {
    pinMode(leftMotorPins[i], OUTPUT);
    pinMode(rightMotorPins[i], OUTPUT);
  }
}

// loop simply calls four movement functions!
void loop() {
  delay(10000);                             // this is done so that when the robot first boots up it doesnt't take off
  forward();
  turnLeft();
  turnRight();
  backward();
}

// moves the robot forward
void forward() {
  // left motor
  analogWrite(leftMotorPins[0], maxSpeed);  // analogWrite to the first pin will determine speed of the motor
  digitalWrite(leftMotorPins[1], HIGH);     // digitalWrite to the next two pins will determine direction of the motor
  digitalWrite(leftMotorPins[2], LOW);
  // right motor
  analogWrite(rightMotorPins[0], maxSpeed); // analogWrite to the first pin will determine speed of the motor
  digitalWrite(rightMotorPins[1], HIGH);    // digitalWrite to the next two pins will determine direction of the motor
  digitalWrite(rightMotorPins[2], LOW);
  delay(timeToMove);                        // allow the motors to run for timeToMove milliseconds
  analogWrite(leftMotorPins[0], 0);         // remember to stop the motors when you're done moving!
  analogWrite(rightMotorPins[0], 0);
}

// moves the robot backward
void backward() {

}

// turns the robot left
void turnLeft() {

}

// turn the robot right
void turnRight() {

}