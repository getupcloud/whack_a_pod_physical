#include "Mole.h";

const int servo = 9;
Mole mole;


/////////////////////////////////////////////////
#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location
void softRestart() {
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}
/////////////////////////////////////////////////
int pos;

void setup() {
  mole.init(A0, servo);

  Serial.begin(9600);
  delay(100);
  Serial.println("Adjust Servo");

  pos = mole.pos_low;
  set_pos();
}

void set_pos() {
  mole.servo.write(pos);
  Serial.print("POS: ");
  Serial.println(pos);
}


void loop() {
  if (Serial.available() > 0) {
    // get incoming byte:
    char inByte = Serial.read();
    switch (inByte) {
      case 'l':
      case 'L':
      case '0':
        pos = mole.pos_low;
        break;
      case 'h':
      case 'H':
      case '1':
        pos = mole.pos_high;
        break;
      case '+':
        pos++;
        break;
     case '-':
        pos--;
        break;
    }
    set_pos();
  }

}
