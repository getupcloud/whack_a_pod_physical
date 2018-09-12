// biblioteca para rede
#include <SPI.h>
#include <Ethernet.h>

//#include <Bounce2.h>
#include "Mole.h";

// instalar teensyduino
// Use Teensy LC Serial + Keyboard + Mouse no menu ferramentas
IPAddress ip(192, 168, 13, 37);
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

const int WIZ_RST = 26;

//                       0   1   2   3   4   5   6   7   8   9
const int pos_low[] =  { 3, 9,  2,  11,  6,  1,  1,  5,  4,  8};
const int pos_high[] = {39, 36, 29, 40, 41, 23, 20, 38, 32, 44};

const int pod_qt = 10;
const int servo[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const int sens_pin[] = {23, 22, 21, 20, 19, 18, 17, 16, 15, 14};
const uint16_t tecla[] = {
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
};

Mole mole[pod_qt];

String incoming = String(255);
int pod = 0;
int val = 0;
int indice = -1;
int var_qt = 0;

/////////////////////////////////////////////////
#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location
void softRestart() {
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}
/////////////////////////////////////////////////

void setup() {
  for (int i = 0; i < pod_qt; i++) {
    mole[i].init(sens_pin[i], servo[i]);
    mole[i].pos_low = pos_low[i];
    mole[i].pos_high = pos_high[i];    
    mole[i].go_down();
  }

  pinMode(WIZ_RST, OUTPUT);
  digitalWrite(WIZ_RST, LOW);
  delay(1);
  digitalWrite(WIZ_RST, HIGH);

  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields

  //  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }
  delay(100);
  Serial.println("Whack-a-mole");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    delay(500);
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}
void loop() {
  //  while (Ethernet.linkStatus() == LinkOFF) {
  //    Serial.println("Ethernet cable is not connected.");
  //    delay(500);
  //  }
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    //Serial.println("Client connected");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // Here is where the POST data is.
          Serial.println("> server hit");
          int ctr = 0;
          var_qt = 0;
          while (client.available())
          {
            char c = client.read();
            switch (c) {
              case '=':
                incoming [ctr] = '\0';
                ctr = 0;
                Serial.println(incoming);
                if (incoming.equals("pod")) {
                  indice = 0;
                  //                  Serial.println("POD");
                }
                else if (incoming.equals("val")) {
                  indice = 1;
                  //                  Serial.println("VAL");
                }
                else {
                  indice = -1;
                }
                break;
              ////////////////////////////////
              case '&':
                incoming [ctr] = '\0';
                ctr = 0;
                checkVal();
                break;
              ////////////////////////////////
              default:
                incoming[ctr] = c;
                ctr++;
            }
          }
          incoming[ctr] = '\0';;
          if (ctr > 0) checkVal();
          if (var_qt >= 2) {
            set_pod();
          }

          //Serial.println("Sending response");
          // send a standard http response header
          client.println("HTTP/1.0 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: keep-alive");
          client.println();
          //form added to send data from browser and view received data in serial monitor
          client.println();
          client.println("<!DOCTYPE html>");
          client.println("<html lang=\"en\">");
          client.println("<body>");
          client.println(" <FORM action=\"\" method=\"POST\">");
          //          client.println(" <P>");
          //          client.print("pod: ");
          //          client.print(pod);
          //          client.print(" val: ");
          //          client.print(val);
          //          client.println(" </P>");
          client.println(" <P>");
          client.println(" <LABEL for=\"pod\">pod:</LABEL>");
          client.println(" <INPUT type=\"text\" name=\"pod\"><BR><BR>");
          client.println(" <LABEL for=\"val\">val:</LABEL>");
          client.println(" <INPUT type=\"text\" name=\"val\"><BR><BR>");
          client.println(" <INPUT type=\"submit\" value=\"Submit\">");
          client.println(" </P>");
          client.println(" </FORM>");
          client.println("</body>");
          client.println("</html>");
          client.println();
          client.stop();
        }
        else if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    Serial.println("Disconnected");
    Serial.println();
  }

  // botoes
  for (int i = 0; i < pod_qt; i++) {
    if (mole[i].hit()) {
      mole[i].go_down();
      Keyboard.press(tecla[i]);
      Serial.print("mole ");
      Serial.print(i);
      Serial.println(" hit");
      delay(10);
      Keyboard.release(tecla[i]);
    }
  }
}

void checkVal() {
  int result = incoming.toInt();
  Serial.print("check: \"");
  Serial.print(incoming);
  Serial.print("\" = ");
  Serial.print(result);
  Serial.println();
  switch (indice) {
    case 0:
      pod = result;
      var_qt++;
      break;
    ////////////////////////////////
    case 1:
      val = result;
      var_qt++;
      break;
  }
}

void set_pod() {
  if (val == 1) mole[pod].go_up();
  else mole[pod].go_down();

  Serial.print ("> MOLE(");
  Serial.print(pod);
  Serial.print(") =  ");
  Serial.print(val);
  Serial.println();
}
