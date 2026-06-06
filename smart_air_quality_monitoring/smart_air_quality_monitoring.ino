// arduino code :

#include <Servo.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Servo windowServo;

int gasPin = A0;
int signalPin = 7;

int windowState = 0; // 0 = closed, 1 = open

// 🔥 Thresholds (optimized)
int gasOpenThreshold = 650;
int gasCloseThreshold = 500;   // 👈 faster closing

// ⏱ Timing control
unsigned long lastBadAirTime = 0;
unsigned long closeDelay = 4000; // 4 sec stability before closing

void setup() {
  Serial.begin(9600);

  pinMode(signalPin, OUTPUT);
  pinMode(gasPin, INPUT);

  dht.begin();
  windowServo.attach(9);

  windowServo.write(0); // start closed
}

void loop() {

  // 🔥 GAS AVERAGING (better stability)
  int gas = 0;
  for (int i = 0; i < 7; i++) {
    gas += analogRead(gasPin);
    delay(30);
  }
  gas = gas / 7;

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("ERROR");
    delay(1500);
    return;
  }

  // 📡 SERIAL DATA (future ready)
  Serial.print("GAS:");
  Serial.print(gas);
  Serial.print(",TEMP:");
  Serial.print(temp);
  Serial.print(",HUM:");
  Serial.println(hum);

  bool badAir = false;

  // 🧠 SMART DECISION
  if (gas > gasOpenThreshold || (temp > 32 && hum > 70)) {
    badAir = true;
    lastBadAirTime = millis();  // update last bad air time
  }

  // 🚨 OPEN WINDOW (instant)
  if (badAir && windowState == 0) {
    windowServo.write(90);
    digitalWrite(signalPin, HIGH);
    windowState = 1;

    Serial.println("STATUS:OPEN");
  }

  // ✅ CLOSE WINDOW (smart + delay + stable air)
  if (!badAir && windowState == 1) {

    if (gas < gasCloseThreshold && millis() - lastBadAirTime > closeDelay) {
      windowServo.write(0);
      digitalWrite(signalPin, LOW);
      windowState = 0;

      Serial.println("STATUS:CLOSE");
    }
  }

  delay(1000); // faster loop
}

// esp32 code : 

// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <WiFiClientSecure.h>

// // WiFi
// const char* ssid = "OPPO Reno11 5G";
// const char* password = "Tejas1209111";

// // Telegram
// String botToken = "8448083892:AAE0bnE9NOENr3LoUxDc5DwtcQfJHk0zA4Y";

// String chatID1 = "6680036635";
// String chatID2 = "7021793764";

// // 🔁 Signal from Arduino (UPDATED PIN)
// int signalPin = 18;

// bool lastState = LOW;

// void setup() {
//   Serial.begin(115200);

//   pinMode(signalPin, INPUT);

//   // WiFi connect
//   WiFi.begin(ssid, password);

//   Serial.print("Connecting...");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("\nWiFi Connected ✅");
// }

// void loop() {

//   int state = digitalRead(signalPin);

//   // 🌫 GAS DETECTED → WINDOW OPEN
//   if (state == HIGH && lastState == LOW) {

//     Serial.println("Poor Air Detected Signal Received");

//     sendTelegramToAll("🌫 Poor Air Quality Detected!\n🪟 Window Opened Automatically 🚨");
//   }

//   // ✅ AIR NORMAL → WINDOW CLOSED
//   if (state == LOW && lastState == HIGH) {

//     Serial.println("Air Back to Normal");

//     sendTelegramToAll("✅ Air Quality Normal\n🪟 Window Closed");
//   }

//   lastState = state;

//   delay(500);
// }


// // 📩 Send to both users
// void sendTelegramToAll(String message) {
//   sendTelegram(chatID1, message);
//   delay(500);
//   sendTelegram(chatID2, message);
// }


// // 📡 Telegram function
// void sendTelegram(String chatID, String message) {

//   if (WiFi.status() == WL_CONNECTED) {

//     WiFiClientSecure client;
//     client.setInsecure();  // SSL bypass

//     HTTPClient http;

//     String url = "https://api.telegram.org/bot" + botToken +
//                  "/sendMessage?chat_id=" + chatID +
//                  "&text=" + message;

//     http.begin(client, url);
//     int httpResponseCode = http.GET();

//     Serial.print("Response: ");
//     Serial.println(httpResponseCode);

//     http.end();
//   }
// }