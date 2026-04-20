#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

// Bluetooth HC-05
SoftwareSerial BT(7, 8); // RX, TX

// Pins
const int servoPin = 3;
const int ir1 = 2; // Hyrje
const int ir2 = 6; // Dalje

// UID i autorizuar
byte authorizedUID[4] = {0x2E, 0x0F, 0x12, 0x07};

int cars = 0;
const int maxCars = 3;

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();

  myServo.attach(servoPin);
  
  pinMode(ir1, INPUT_PULLUP);
  pinMode(ir2, INPUT_PULLUP);

  closeGate(); 
  Serial.println("Sistemi u ndez. Parkimi bosh.");

  sendStatus();
}

void loop() {

  // -------- DALJA --------
  if (digitalRead(ir2) == LOW) {
    delay(50);
    if (digitalRead(ir2) == LOW) {
      Serial.println("Makina po del...");
      openGate();

      unsigned long timeout = millis();
      while (digitalRead(ir2) == LOW && millis() - timeout < 5000) {
        delay(10);
      }

      if (cars > 0) cars--;

      Serial.print("Makina doli. Mbeten: ");
      Serial.println(cars);

      sendStatus();

      delay(3000);
      closeGate();
      delay(1000);
    }
  }

  // -------- HYRJA (RFID) --------
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  if (isAuthorized(mfrc522.uid.uidByte)) {

    if (cars < maxCars) {
      Serial.println("Autorizuar! Hapet porta.");
      openGate();

      unsigned long waitCar = millis();
      bool detected = false;

      while (millis() - waitCar < 5000) {
        if (digitalRead(ir1) == LOW) {
          detected = true;
          break;
        }
      }

      if (detected) {
        while (digitalRead(ir1) == LOW) {
          delay(200);
        }

        cars++;

        Serial.print("Makina hyri. Total: ");
        Serial.println(cars);

        sendStatus();
      } else {
        Serial.println("Asnjë makinë nuk hyri.");
      }

      closeGate();

    } else {
      Serial.println("Parkimi PLOT!");
      BT.println("FULL");
    }

  } else {
    Serial.println("Kartë e panjohur!");
    BT.println("DENIED");
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// -------- FUNKSIONE --------

bool isAuthorized(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != authorizedUID[i]) return false;
  }
  return true;
}

void openGate() {
  myServo.write(80);
}

void closeGate() {
  myServo.write(20);
}

// Dërgon statusin në Bluetooth
void sendStatus() {
  BT.print("CARS:");
  BT.println(cars);

  Serial.print("Dergohet me BT: ");
  Serial.println(cars);
}