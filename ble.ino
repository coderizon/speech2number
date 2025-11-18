/*
 * Mechanische 7-Segment-Anzeige über BLE (Bluetooth Low Energy)
 * Hardware: Arduino Uno R4 WiFi + 7 Servos
 * Protokoll: Nordic UART Service (kompatibel mit Apps wie "Serial Bluetooth Terminal")
 */

#include <ArduinoBLE.h>
#include <Servo.h>

// --- SERVO KONFIGURATION ---
Servo segmentA;  // Pin 2
Servo segmentB;  // Pin 3
Servo segmentC;  // Pin 4
Servo segmentD;  // Pin 5
Servo segmentE;  // Pin 6
Servo segmentF;  // Pin 7
Servo segmentG;  // Pin 8

// Konstanten für die Servo-Positionen (aus deinem ersten Code)
const int POSITION_AUF = 90;    // Segment aus
const int POSITION_ZU = 0;      // Segment an

// --- BLE KONFIGURATION (Nordic UART Service) ---
BLEService uartService("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
BLEStringCharacteristic rxCharacteristic("6e400002-b5a3-f393-e0a9-e50e24dcca9e", BLEWrite | BLEWriteWithoutResponse, 20);
BLEStringCharacteristic txCharacteristic("6e400003-b5a3-f393-e0a9-e50e24dcca9e", BLENotify, 20);

void setup() {
  Serial.begin(9600);

  // Servos initialisieren
  segmentA.attach(2);
  segmentB.attach(3);
  segmentC.attach(4);
  segmentD.attach(5);
  segmentE.attach(6);
  segmentF.attach(7);
  segmentG.attach(8);

  // Startzustand: Alles aus
  alleSegmenteDeaktivieren();

  // BLE starten
  if (!BLE.begin()) {
    Serial.println("BLE Start fehlgeschlagen!");
    while (1);
  }

  BLE.setLocalName("Servo 7-Seg"); 
  BLE.setAdvertisedService(uartService);
  uartService.addCharacteristic(rxCharacteristic);
  uartService.addCharacteristic(txCharacteristic);
  BLE.addService(uartService);
  BLE.advertise();

  Serial.println("BLE bereit. Verbinde dich und sende eine Zahl (0-9).");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Verbunden mit: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (rxCharacteristic.written()) {
        String nachricht = rxCharacteristic.value();
        nachricht.trim(); // Leerzeichen entfernen

        if (nachricht.length() > 0) {
          char befehl = nachricht.charAt(0); // Nur das erste Zeichen nehmen
          Serial.print("Empfangen: ");
          Serial.println(befehl);

          steuereAnzeige(befehl);
        }
      }
    }
    Serial.println("Getrennt.");
  }
}

// Logik zur Auswahl der Zahl
void steuereAnzeige(char zeichen) {
  switch (zeichen) {
    case '0': zeigeNull(); break;
    case '1': zeigeEins(); break;
    case '2': zeigeZwei(); break;
    case '3': zeigeDrei(); break;
    case '4': zeigeVier(); break;
    case '5': zeigeFuenf(); break;
    case '6': zeigeSechs(); break;
    case '7': zeigeSieben(); break;
    case '8': zeigeAcht(); break;
    case '9': zeigeNeun(); break;
    case 'x': alleSegmenteDeaktivieren(); break; // Reset
    default: Serial.println("Unbekanntes Zeichen"); break;
  }
}

// --- SERVO FUNKTIONEN (Unverändert) ---
void zeigeNull() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_ZU); segmentF.write(POSITION_ZU);
  segmentG.write(POSITION_AUF);
}

void zeigeEins() {
  segmentA.write(POSITION_AUF); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_AUF); segmentE.write(POSITION_AUF); segmentF.write(POSITION_AUF);
  segmentG.write(POSITION_AUF);
}

void zeigeZwei() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_ZU); segmentC.write(POSITION_AUF);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_ZU); segmentF.write(POSITION_AUF);
  segmentG.write(POSITION_ZU);
}

void zeigeDrei() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_AUF); segmentF.write(POSITION_AUF);
  segmentG.write(POSITION_ZU);
}

void zeigeVier() {
  segmentA.write(POSITION_AUF); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_AUF); segmentE.write(POSITION_AUF); segmentF.write(POSITION_ZU);
  segmentG.write(POSITION_ZU);
}

void zeigeFuenf() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_AUF); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_AUF); segmentF.write(POSITION_ZU);
  segmentG.write(POSITION_ZU);
}

void zeigeSechs() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_AUF); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_ZU); segmentF.write(POSITION_ZU);
  segmentG.write(POSITION_ZU);
}

void zeigeSieben() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_AUF); segmentE.write(POSITION_AUF); segmentF.write(POSITION_AUF);
  segmentG.write(POSITION_AUF);
}

void zeigeAcht() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_ZU); segmentF.write(POSITION_ZU);
  segmentG.write(POSITION_ZU);
}

void zeigeNeun() {
  segmentA.write(POSITION_ZU); segmentB.write(POSITION_ZU); segmentC.write(POSITION_ZU);
  segmentD.write(POSITION_ZU); segmentE.write(POSITION_AUF); segmentF.write(POSITION_ZU);
  segmentG.write(POSITION_ZU);
}

void alleSegmenteDeaktivieren() {
  segmentA.write(POSITION_AUF); segmentB.write(POSITION_AUF); segmentC.write(POSITION_AUF);
  segmentD.write(POSITION_AUF); segmentE.write(POSITION_AUF); segmentF.write(POSITION_AUF);
  segmentG.write(POSITION_AUF);
}