/*
    btTank v0.1
    08 July 2015
    Kristof Aldenderfer (aldenderfer.github.io)

    HARDWARE:
        - Arduino UNO Rev3
        - DFRobot Bluetooth V3 (connect GND to pin 9, Vcc to pin 10)
    SUPPORTED SOFTWARE:
        - Android: SensDuino
        - iOS: none

    CHANGELOG:
        - none; this is the first version!
    TODO:
        - math accelerometer values better
        - smooth accelerometer values
        - add ios app support
        - status messages to app
 */
#include <SoftwareSerial.h>

#define RxD 13
#define TxD 12
#define PWR 10
SoftwareSerial blueToothSerial(RxD, TxD);
String btStream;
float left, right;
float sensorVals[2];
byte motorPins[6] = { 2, 3, 4, 5, 6, 7 };
String app = "SensDuino";
boolean verboseMode = true;

void setup() {
  // communication to the BT board
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(PWR, OUTPUT);
  digitalWrite(PWR, HIGH);
  // motor control pins
  for (int i = 0 ; i < 6 ; i++) {
    pinMode(motorPins[i], OUTPUT);
  }
  Serial.begin(9600);
  delay(100);
  blueToothSerial.begin(9600);
  delay(100);
  Serial.println("Ready to roll!");
}

void loop() {
  if (blueToothSerial.available()) {
    btStream = blueToothSerial.readStringUntil('\n');
    if (verboseMode) {
      Serial.print("Received: ");
      Serial.print(btStream);
      Serial.print("\t");
    }
    if (app == "SensDuino") sensDuinoParse();
    vectorize();
    motorDirection(0, (left >= 0) );
    motorDirection(1, (right >= 0) );
    motorSpeed(0, abs(left) );
    motorSpeed(1, abs(right) );
  }
  if (Serial.available()) {
    while (Serial.available()) {
      blueToothSerial.write(Serial.read());
    }
  }
}

// parsing for SensDuino (android only)
// map() isn't working well
// consider also adding a smoothing algorithm to remove spikes
void sensDuinoParse() {
  int xloc = btStream.indexOf(',', 3);
  String stringVals[2];
  for (int i = 0 ; i < 2 ; i++ ) {
    int yloc = btStream.indexOf(',', xloc + 1);
//    stringVals[i] = btStream.substring(xloc + 1, yloc);
//    sensorVals[i] = stringVals[i].toFloat();
    sensorVals[i] = (btStream.substring(xloc + 1, yloc)).toFloat();;
    if (i == 1) sensorVals[i] -= 4.4;
    xloc = yloc;
  }
}

// expects -4.4 to 4.4
// from http://www.goodrobot.com/en/2009/09/tank-drive-via-joystick-control/
// this is beautiful
void vectorize() {
  float x = sensorVals[0];
  float y = sensorVals[1];
  // first Compute the angle in deg

  // First hypotenuse
  float z = sqrt(x * x + y * y);
  // angle in radians
  float rad = acos(abs(x) / z);
  // and in degrees
  float angle = rad * 180 / PI;

  // Now angle indicates the measure of turn
  // Along a straight line, with an angle o, the turn co-efficient is same
  // this applies for angles between 0-90, with angle 0 the co-eff is -1
  // with angle 45, the co-efficient is 0 and with angle 90, it is 1
  float tcoeff = -1 + (angle / 90) * 2;
  float turn = tcoeff * abs(abs(y) - abs(x));
  turn = round(turn * 100) / 100;
  // And max of y or x is the movement
  float movve = max(abs(y), abs(x));
  // First and third quadrant
  if ( (x >= 0 && y >= 0) || (x < 0 &&  y < 0) ) {
    left = movve;
    right = turn;
  } else {
    right = movve;
    left = turn;
  }
  // Reverse polarity
  if (y > 0) {
    left = 0 - left;
    right = 0 - right;
  }
  if (verboseMode) {
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");
  Serial.print(rad);
  Serial.print("\t");
  Serial.print(angle);
  Serial.print("\t");
  Serial.print(tcoeff);
  Serial.print("\t");
  Serial.print(turn);
  Serial.print("\t");
  Serial.print(movve);
  Serial.print("\t");
  Serial.print(left);
  Serial.print("\t");
  Serial.print(right);
  Serial.println();
  }
}

// 0 = forward, 1 = reverse
void motorDirection(byte m, byte d) {
  digitalWrite(motorPins[m * 3], d);
  digitalWrite(motorPins[2 + m * 3], !d);
}

void motorSpeed(byte m, byte s) {
  analogWrite(motorPins[1 + m * 3], s);
}
