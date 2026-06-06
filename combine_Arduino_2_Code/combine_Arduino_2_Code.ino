// arduino code :

#include <Servo.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

  DHT dht(DHTPIN, DHTTYPE);

// ===== SERVOS =====
Servo windowServo;  // Air system
Servo rainServo;    // Rain system

// ===== PINS =====
int gasPin = A0;
int airSignalPin = 7;  // air signal

int rainPin = 3;
int rainSignalPin = 13;  // rain signal


// ===== STATES =====
int windowState = 0;
bool rainState = false;


// ===== THRESHOLDS =====
int gasOpenThreshold = 650;
int gasCloseThreshold = 500;

// ===== TIMING =====
unsigned long lastBadAirTime = 0;
unsigned long closeDelay = 4000;

void setup() {
  Serial.begin(9600);

  // Pins
  pinMode(gasPin, INPUT);
  pinMode(airSignalPin, OUTPUT);

  pinMode(rainPin, INPUT);
  pinMode(rainSignalPin, OUTPUT);


  dht.begin();

  // Servos
  windowServo.attach(9);
  rainServo.attach(4);

  windowServo.write(0);
  rainServo.write(0);
}

void loop() {

  // ================= 🌧 RAIN SYSTEM =================
  int rain = digitalRead(rainPin);

  if (rain == LOW && !rainState) {

    Serial.println("🌧 Rain Detected");

    rainServo.write(90);
    digitalWrite(rainSignalPin, HIGH);

    rainState = true;
  }

  else if (rain == HIGH && rainState) {

    Serial.println("☀ Rain Stopped");

    rainServo.write(0);
    digitalWrite(rainSignalPin, LOW);

    rainState = false;
  }


  // ================= 🌫 AIR SYSTEM =================

  int gas = 0;
  for (int i = 0; i < 7; i++) {
    gas += analogRead(gasPin);
    delay(30);  // (same as tera original)
  }
  gas = gas / 7;

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("ERROR");
    delay(1500);
    return;
  }

  Serial.print("GAS:");
  Serial.print(gas);
  Serial.print(",TEMP:");
  Serial.print(temp);
  Serial.print(",HUM:");
  Serial.println(hum);

  bool badAir = false;

  if (gas > gasOpenThreshold || (temp > 32 && hum > 70)) {
    badAir = true;
    lastBadAirTime = millis();
  }

  // 🚨 OPEN
  if (badAir && windowState == 0) {
    windowServo.write(90);
    digitalWrite(airSignalPin, HIGH);
    windowState = 1;

    Serial.println("STATUS:OPEN");
  }

  // ✅ CLOSE
  if (!badAir && windowState == 1) {
    if (gas < gasCloseThreshold && millis() - lastBadAirTime > closeDelay) {
      windowServo.write(0);
      digitalWrite(airSignalPin, LOW);
      windowState = 0;

      Serial.println("STATUS:CLOSE");
    }
  }

  delay(1000);  // same as tera code
}

