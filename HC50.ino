#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
Servo gate;

// pins
const int irEnter = 2;
const int irExit  = 6;
const int servoPin = 3;

// authorized card
byte allowedUID[4] = {0x2E, 0x0F, 0x12, 0x07};

int cars = 0;
const int maxCars = 3;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  gate.attach(servoPin);

  pinMode(irEnter, INPUT_PULLUP);
  pinMode(irExit, INPUT_PULLUP);

  closeGate();

  sendData();
}

void loop() {

  // EXIT
  if (digitalRead(irExit) == LOW) {
    delay(50);
    if (digitalRead(irExit) == LOW) {
      openGate();

      delay(2000);

      if (cars > 0) cars--;

      sendData();

      delay(2000);
      closeGate();
    }
  }

  // RFID ENTRY
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (isAllowed(rfid.uid.uidByte)) {

    if (cars < maxCars) {
      openGate();

      delay(2000);

      if (digitalRead(irEnter) == LOW) {
        cars++;
      }

      sendData();
      closeGate();

    } else {
      Serial.println("FULL");
    }
  }

  rfid.PICC_HaltA();
}

bool isAllowed(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != allowedUID[i]) return false;
  }
  return true;
}

void openGate() {
  gate.write(90);
}

void closeGate() {
  gate.write(0);
}

// 🔥 Dërgon të dhëna në server
void sendData() {
  Serial.print("CARS:");
  Serial.println(cars);
}
