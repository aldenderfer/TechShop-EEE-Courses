/*
    Red Orm v0.9
    15 July 2015
    Kristof Aldenderfer (aldenderfer.github.io)
    Luca Angeleri

    HARDWARE:
        - Arduino UNO Rev3
        - DFRobot Bluetooth V3 (connect GND to pin 9, Vcc to pin 10)
        - 2-channel motor controller
        - 2 motors
        - batteries
    SUPPORTED UI:
        - controllers: PS3
        - Android: SensDuino
        - iOS: none
    CHANGELOG:
        - PS3 CONTROLLER INTEGRATION (well done, Luca!)
        - renamed from btTank_full to red_orm
    TODO:
        - (SensDuino) math accelerometer values better
        - (SensDuino) smooth accelerometer values
        - add ios app support
        - add return status messages back to app
 */

#include <SoftwareSerial.h>
#include <PS3USB.h>


// is this block of code needed? since we aren't using teensy, probably not
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

#define RxD 13
#define TxD 12
#define PWR 10
// direction 1 (left), speed (left), direction 2 (left), direction 1 (right), speed (right), direction 2 (right)
byte motorPins[6] = { 2, 3, 4, 5, 6, 7 };
String ui = "SensDuino";
boolean verboseMode = false;
SoftwareSerial blueToothSerial(RxD, TxD);
float left, right;
USB Usb;
PS3USB PS3(&Usb);

void setup() {
  // motor control pins
  for (int i = 0 ; i < 6 ; i++) {
    pinMode(motorPins[i], OUTPUT);
  }
  if (ui == "ps3") {
    // this section can also be cleaned up
    // additionally: move this to another function?
    Serial.begin(115200);
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
    if (Usb.Init() == -1) {
      Serial.print(F("\r\nOSC did not start"));
      while (1); //halt
    }
    Serial.print(F("\r\nPS3 USB Library Started"));
  }
  else {
    // communication to the BT board
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(PWR, OUTPUT);
    digitalWrite(PWR, HIGH);
    Serial.begin(9600);
    delay(100);
    blueToothSerial.begin(9600);
    delay(100);
  }
  Serial.println("Ready to roll!");
}

void loop() {
  // are we using a ps3 controller?
  if (ui == "ps3") {
    Usb.Task();
    if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
      motorDirection(PS3.getAnalogHat(LeftHatY) <= 130, (PS3.getAnalogHat(RightHatY) <= 130) );
      motorSpeed((PS3.getAnalogButton(L2)), (PS3.getAnalogButton(R2)));
      delay(100);
    }
  }
  // else are we using an android device?
  else if (ui == "SensDuino") {
    if (blueToothSerial.available()) {
      String btStream = blueToothSerial.readStringUntil('\n');
      if (verboseMode) {
        Serial.print("Received: ");
        Serial.print(btStream);
        Serial.print("\t");
      }
      if (ui == "SensDuino") {
        sensDuinoParse(btStream);
      }
    }
    // if value is greater than or equal to 0, then true, therefore forward
    // else reverse
    motorDirection( (left >= 0), (right >= 0) );
    // PWM vals must be positive; direction already set by motorDirection()
    motorSpeed( abs(left), abs(right) );
  }
  // are we receiving from the tank?
  if (Serial.available()) {
    while (Serial.available()) {
      blueToothSerial.write(Serial.read());
    }
  }
}

/* sets both motor directions
   input: left and right direction (boolean)
   input range: true = forward, false = reverse
   output: none
 */
void motorDirection(boolean l, boolean r) {
  digitalWrite(motorPins[0], l);
  digitalWrite(motorPins[2], !l);
  digitalWrite(motorPins[3], r);
  digitalWrite(motorPins[5], !r);
}

/* sets both motor speeds
   input: left and right PWM values (byte)
   input range: 0 to 255
   output: none
 */
void motorSpeed(byte l, byte r) {
  analogWrite(motorPins[1], l);
  analogWrite(motorPins[4], r);
}

/* parsing for SensDuino (android only)
   chops up the stream into parts, scales to PWM values, and constrains
   input: accelerometer values (String)
   input range: -4.9 to 4.9 for x, 0 to 9.8 for y
   output: none
 */
void sensDuinoParse(String bts) {
  int xloc = bts.indexOf(',', 3);
  float sensorVals[2];
  for (int i = 0 ; i < 2 ; i++ ) {
    int yloc = bts.indexOf(',', xloc + 1);
    sensorVals[i] = ((bts.substring(xloc + 1, yloc)).toFloat()) * 52;
    // why 52? because these values are coming in as +-4.9
    // and we want them to be +-255. 255/4.9 = 52
    if (i == 1) sensorVals[i] -= 255;
    if (sensorVals[i] > 255) sensorVals[i] = 255;
    else if (sensorVals[i] < -255) sensorVals[i] = -255;
    xloc = yloc;
  }
  vectorize(sensorVals[0], sensorVals[1]);
}

/* converts xy to lr
   source: http://www.goodrobot.com/en/2009/09/tank-drive-via-joystick-control/
   input: xy accelerometer values
   input range: -255 to 255
   output: (SET GLOBALS) left and right to +-255
 */
void vectorize(float x, float y) {
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
    right = movve;
    left = turn;
  } else {
    left = movve;
    right = turn;
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
