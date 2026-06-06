// #include <Servo.h>

// int rainPin = 3;
// int signalPin = 13;
// int servoPin = 4;

// Servo myServo;
// bool rainState = false;

// void setup() {
//   Serial.begin(9600);

//   pinMode(rainPin, INPUT);
//   pinMode(signalPin, OUTPUT);

//   myServo.attach(servoPin);
//   myServo.write(0);
// }

// void loop() {
//   int rain = digitalRead(rainPin);

//   // ⚠️ Usually LOW = rain detected
//   if (rain == LOW && !rainState) {

//     Serial.println("🌧 Rain Detected");

//     myServo.write(90);
//     digitalWrite(signalPin, HIGH); // send to ESP

//     rainState = true;
//   }

//   else if (rain == HIGH && rainState) {

//     Serial.println("☀ Rain Stopped");

//     myServo.write(0);
//     digitalWrite(signalPin, LOW);

//     rainState = false;
//   }

//   delay(300);
// }
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// WiFi
const char* ssid = "OPPO Reno11 5G";
const char* password = "pass";

// Telegram
String botToken = "token";

String chatID1 = "6680036635";
String chatID2 = "7021793764";

// 🔁 Signal from Arduino
int signalPin = 23;

bool lastState = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(signalPin, INPUT);

  // WiFi connect
  WiFi.begin(ssid, password);

  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected ✅");
}

void loop() {
  int state = digitalRead(signalPin);

  // 🌧 Trigger only once
  if (state == HIGH && lastState == LOW) {

    Serial.println("Rain Detected Signal Received");

    sendTelegramToAll("Rain detected");

  }

  lastState = state;

  delay(500);
}

// 📩 Send to both users
void sendTelegramToAll(String message) {
  sendTelegram(chatID1, message);
  delay(500);
  sendTelegram(chatID2, message);
}

// 📡 Telegram function
void sendTelegram(String chatID, String message) {

  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();  // SSL bypass

    HTTPClient http;

    String url = "https://api.telegram.org/bot" + botToken +
                 "/sendMessage?chat_id=" + chatID +
                 "&text=" + message;

    http.begin(client, url);
    int httpResponseCode = http.GET();

    Serial.print("Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}
