
// arduino code :

// int sensePin = 2;
// int buzzerPin = 8;
// int ledPin = 5;

// void setup() {
//   pinMode(sensePin, INPUT_PULLUP);
//   pinMode(buzzerPin, OUTPUT);
//   pinMode(ledPin, OUTPUT);
// }

// void loop() {
//   int state = digitalRead(sensePin);

//   if (state == HIGH) {
//     tone(buzzerPin, 1000);
//     digitalWrite(ledPin, HIGH);   // LED ON
//   }
//   else {
//     noTone(buzzerPin);
//     pinMode(buzzerPin, INPUT);    // heat fix
//     digitalWrite(ledPin, LOW);    // LED OFF
//   }
// }

// esp32 code :

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "OPPO Reno11 5G";
const char* password = "pass";

String botToken = "your token";

String chatID1 = "6680036635";
String chatID2 = "7021793764";

int sensePin = 4;
bool lastState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(sensePin, INPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected ✅");
}

void loop() {
  int state = digitalRead(sensePin);

  if (state == HIGH && lastState == LOW) {
    Serial.println("ALERT Triggered");

    sendTelegramToAll(
      "EMERGENCY ALERT : Fan load detected !! Check immediately");
  }

  lastState = state;
  delay(500);
}

// Send to BOTH people
void sendTelegramToAll(String message) {
  sendTelegram(chatID1, message);
  delay(500);  // safe delay
  sendTelegram(chatID2, message);
}

// Send function
void sendTelegram(String chatID, String message) {
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();  // SSL bypass

    HTTPClient http;

    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;

    http.begin(client, url);
    int httpResponseCode = http.GET();

    Serial.print("Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}


