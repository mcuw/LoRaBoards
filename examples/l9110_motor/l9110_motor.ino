// TODO add L9110 motor control to LgLoraBoard
#include <Arduino.h>
#include <LgLoraBoard.h>

LgLoraBoard board;

#define MAX_OPEN_POSITION 8
#define AIA 33
#define AIB 32
// #define BIA ..
// #define BIB ..

byte speed = 255;

int incomingByte = 0; // for incoming serial data
unsigned long previousMillis = 0;
const long interval = 1000; // interval until stop (milliseconds)
int state = 0; // stop
int position = MAX_OPEN_POSITION; // set initial position is fully open

void setup() {
  Serial.begin(115200);
  Serial.println(F("L9110 Motor Test"));

  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  // pinMode(BIA, OUTPUT);
  // pinMode(BIB, OUTPUT);
  analogWrite(AIA, 0);
  analogWrite(AIB, 0);
}

// öffne Ventil, bedeutet wärmer
void forward() {
  Serial.println(F("L9110 forward"));
  // if (position >= MAX_OPEN_POSITION) {
  //   state = 0;
  //   Serial.println(F("Already fully open"));
  //   return;
  // }
  // ++position;

  analogWrite(AIA, 0);
  analogWrite(AIB, speed);
  // analogWrite(BIA, 0);
  // analogWrite(BIB, speed);
}

// schliesse Ventil, bedeutet kälter
void backward() {
  Serial.println(F("L9110 backward"));
  // if (position <= 0) {
  //   state = 0;
  //   Serial.println(F("Already fully closed"));
  //   return;
  // }
  // --position;

  analogWrite(AIA, speed);
  analogWrite(AIB, 0);
  // analogWrite(BIA, speed);
  // analogWrite(BIB, 0);
}

// stop
void stop() {
  Serial.println(F("L9110 stop"));

  analogWrite(AIA, 0);
  analogWrite(AIB, 0);
}

void loop() {
  unsigned long currentMillis = millis();
  if (state && currentMillis - previousMillis >= interval) {
    stop();
    state = 0;
  }

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);

    switch (incomingByte)
    {
    case 'f':
      forward();
      state = 1;
      previousMillis = millis(); // reset timer
      break;

    case 'b':
      backward();
      state = 2;
      previousMillis = millis(); // reset timer
      break;

    case 's':
      stop();
      state = 0;
      break;

    default:
      break;
    }
  }
}
