# TS-Robotics-Camp-Bluetooth-Tank

Library: https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend/software
App:     https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend/software-resources

DEPRECATED

bluetooth-controlled tank (for robotics camp)
v0.9.2
23 July 2015

HARDWARE:
    - Arduino UNO Rev3
    - DFRobot Bluetooth V3 (connect GND to pin 9, Vcc to pin 10)
    - OR Arduino USB shield plus XBOXONE or PS3 controller
    - 2-channel motor controller
    - 2 motors
    - batteries
SUPPORTED UI:
    - controllers: XBOXONE, PS3
    - Android: SensDuino, ArduDroid
    - iOS: none
CHANGELOG:
    - XBOXONE CONTROLLER INTEGRATION (well done, Luca!)
    - added ArduDroid support (faders 1 and 4 control L and R)
TODO:
    - (SensDuino) math accelerometer values better
    - (SensDuino) smooth accelerometer values
    - add ios app support (this will be a hardware overhaul)
    - add return status messages back to app
