#ifndef Mole_h
#define Mole_h

#include "Arduino.h"
#include <Servo.h>

class Mole {
  public:
    Servo servo;
    int sensor_p;
    int servo_p;
    int pos_low;
    int pos_high;
    int sensor_antes;

    Mole();
    void init(int sensor_pin, int servo_pin);
    boolean hit();
    void go_up();
    void go_down();
};
/////////////////////////////////////////
Mole::Mole() {
  pos_low = 5;
  pos_high = 40;
}

void Mole::init(int sensor_pin, int servo_pin) {
  sensor_p = sensor_pin;
  servo_p = servo_pin;
  servo.attach(servo_p);
  pinMode(sensor_p, INPUT_PULLUP);
  sensor_antes = digitalRead(sensor_p);
}

boolean Mole::hit() {
  int sensor_agora = digitalRead(sensor_p);
  boolean result = ( sensor_agora == LOW && sensor_antes == HIGH);
  sensor_antes = sensor_agora;
  return result;
}

void Mole::go_up() {
  servo.write(pos_high);
}

void Mole::go_down() {
  servo.write(pos_low);
}

#endif
