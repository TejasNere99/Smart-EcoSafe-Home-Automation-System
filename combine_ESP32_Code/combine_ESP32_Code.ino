// esp32 code :

// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <WiFiClientSecure.h>

// // ===== WiFi =====
// const char* ssid = "OPPO Reno11 5G";
// const char* password = "Tejas1209111";

// // ===== Telegram =====
// String botToken = "8448083892:AAE0bnE9NOENr3LoUxDc5DwtcQfJHk0zA4Y";

// String chatID1 = "6680036635";
// String chatID2 = "7021793764";

// // ===== PINS =====
// int airPin = 18;    // Air quality signal
// int rainPin = 23;   // Rain signal
// int loadPin = 4;    // Load detection


// // ===== STATES =====
// bool lastAirState = LOW;
// bool lastRainState = LOW;
// bool lastLoadState = LOW;


// void setup() {
//   Serial.begin(115200);

//   pinMode(airPin, INPUT);
//   pinMode(rainPin, INPUT);
//   pinMode(loadPin, INPUT);


//   WiFi.begin(ssid, password);

//   Serial.print("Connecting...");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("\nWiFi Connected ✅");
// }

// // ================= MAIN LOOP =================
// void loop() {

//   // ===== 🌫 AIR QUALITY =====
//   int airState = digitalRead(airPin);

//   if (airState == HIGH && lastAirState == LOW) {
//     Serial.println("🌫 Poor Air Detected");

//     sendTelegramToAll(
//       "Poor Air Quality Detected! Window Opened Automatically");
//   }

//   if (airState == LOW && lastAirState == HIGH) {
//     Serial.println("✅ Air Normal");

//     sendTelegramToAll(
//       "Air Quality Normal. Window Closed");
//   }

//   lastAirState = airState;


//   // ===== 🌧 RAIN SYSTEM =====
//   int rainState = digitalRead(rainPin);

//   if (rainState == HIGH && lastRainState == LOW) {
//     Serial.println("🌧 Rain Detected");

//     sendTelegramToAll(
//       "Rain detected! Window Closed for Protection");
//   }

//   if (rainState == LOW && lastRainState == HIGH) {
//     Serial.println("☀ Rain Stopped");

//     sendTelegramToAll(
//       "Rain stopped. System back to normal");
//   }

//   lastRainState = rainState;


//   // ===== ⚡ LOAD ALERT =====
//   int loadState = digitalRead(loadPin);

//   if (loadState == HIGH && lastLoadState == LOW) {
//     Serial.println("⚡ Load Alert");

//     sendTelegramToAll(
//       "EMERGENCY ALERT! Abnormal fan load detected!");
//   }

//   lastLoadState = loadState;

//   delay(300);
// }


// // ================= TELEGRAM FUNCTIONS =================

// void sendTelegramToAll(String message) {
//   sendTelegram(chatID1, message);
//   delay(500);
//   sendTelegram(chatID2, message);
// }

// void sendTelegram(String chatID, String message) {

//   if (WiFi.status() == WL_CONNECTED) {

//     WiFiClientSecure client;
//     client.setInsecure();

//     HTTPClient http;

//     // Encoding fix
//     message.replace(" ", "%20");
//     message.replace("\n", "%0A");
//     message.replace("!", "%21");

//     String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;

//     http.begin(client, url);
//     int httpResponseCode = http.GET();

//     Serial.print("Response: ");
//     Serial.println(httpResponseCode);

//     http.end();
//   }
// }

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Fix for macro conflicts between RoboEyes and ESP32 WiFi/SSL libraries
// The RoboEyes library defines 'N' and 'E', which conflict with mbedtls used by WiFi
#ifdef N
#undef N
#endif
#ifdef E
#undef E
#endif

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

// OLED and RoboEyes Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define FAN_PIN 4
#define RAIN_PIN 23
#define AIR_PIN 18

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

// Alert system
bool alertMode = false;
bool toggleState = false;

unsigned long lastToggle = 0;
int toggleInterval = 600;

int currentMood = DEFAULT;
String alertText = "";

// Idle system
unsigned long lastIdleChange = 0;
int idleInterval = 4000;
int idleState = 0;

// WiFi and Telegram Definitions
const char* ssid = "OPPO Reno11 5G";
const char* password = "Tejas1209111";

String botToken = "8448083892:AAE0bnE9NOENr3LoUxDc5DwtcQfJHk0zA4Y";
String chatID1 = "6680036635";
String chatID2 = "7021793764";

// Pins for Telegram System
int airPin = 18;    
int rainPin = 23;   
int loadPin = 4;    

// States for Telegram System
bool lastAirState = LOW;
bool lastRainState = LOW;
bool lastLoadState = LOW;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Pin modes
  pinMode(FAN_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  pinMode(AIR_PIN, INPUT);

  // Display Setup
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60);
  roboEyes.setAutoblinker(ON, 8, 2);
  roboEyes.setIdleMode(ON, 2, 1);
  roboEyes.setMood(DEFAULT);
  roboEyes.open();

  // WiFi Setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected ✅");
}

// ================= LOOP =================
void loop() {
  // --- Part 1: Telegram System Logic ---
  
  // 🌫 AIR QUALITY
  int airState = digitalRead(airPin);
  if (airState == HIGH && lastAirState == LOW) {
    Serial.println("🌫 Poor Air Detected");
    sendTelegramToAll("Poor Air Quality Detected! Window Opened Automatically");
  }
  if (airState == LOW && lastAirState == HIGH) {
    Serial.println("✅ Air Normal");
    sendTelegramToAll("Air Quality Normal. Window Closed");
  }
  lastAirState = airState;

  // 🌧 RAIN SYSTEM
  int rainState = digitalRead(rainPin);
  if (rainState == HIGH && lastRainState == LOW) {
    Serial.println("🌧 Rain Detected");
    sendTelegramToAll("Rain detected! Window Closed for Protection");
  }
  if (rainState == LOW && lastRainState == HIGH) {
    Serial.println("☀ Rain Stopped");
    sendTelegramToAll("Rain stopped. System back to normal");
  }
  lastRainState = rainState;

  // ⚡ LOAD ALERT
  int loadState = digitalRead(loadPin);
  if (loadState == HIGH && lastLoadState == LOW) {
    Serial.println("⚡ Load Alert");
    sendTelegramToAll("EMERGENCY ALERT! Abnormal fan load detected!");
  }
  lastLoadState = loadState;

  // --- Part 2: Display and RoboEyes Logic ---

  int newMood = DEFAULT;
  bool eventActive = false;

  // PRIORITY SYSTEM
  if (digitalRead(FAN_PIN) == HIGH) {
    newMood = ANGRY;
    alertText = "ALERT!";
    eventActive = true;
  } else if (digitalRead(RAIN_PIN) == HIGH) {
    newMood = TIRED;
    alertText = "RAIN!";
    eventActive = true;
  } else if (digitalRead(AIR_PIN) == HIGH) {
    newMood = TIRED;
    alertText = "BAD AIR!";
    eventActive = true;
  } 

  // ALERT MODE
  if (eventActive) {
    if (!alertMode) {
      alertMode = true;
      currentMood = newMood;
      roboEyes.setMood(newMood);
      if (newMood == ANGRY) roboEyes.blink();
      if (newMood == HAPPY) roboEyes.anim_laugh();
      if (newMood == TIRED) roboEyes.anim_confused();
    }

    if (millis() - lastToggle > toggleInterval) {
      toggleState = !toggleState;
      lastToggle = millis();
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);

    if (toggleState) {
      display.setCursor(10, 25);
      display.println(alertText);
    } else {
      display.setCursor(40, 20);
      if (currentMood == ANGRY) display.println(">:(");
      else if (currentMood == HAPPY) display.println(":)");
      else display.println("-_-");
    }
    display.display();
    delay(10); 
    return;
  }

  // NORMAL MODE
  alertMode = false;

  static int mode = 0;  // 0 = eyes, 1 = message
  static unsigned long lastSwitch = 0;
  static int msgIndex = 0;

  unsigned long eyesDuration = 5000;
  unsigned long msgDuration = 3000;

  if (mode == 0 && millis() - lastSwitch > eyesDuration) {
    mode = 1;
    lastSwitch = millis();
    msgIndex++;
    if (msgIndex > 4) msgIndex = 0;
  } else if (mode == 1 && millis() - lastSwitch > msgDuration) {
    mode = 0;
    lastSwitch = millis();
  }

  // EYES MODE
  if (mode == 0) {
    roboEyes.setMood(DEFAULT);
    roboEyes.update();
  } else {
    // MESSAGE MODE
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(15, 28);

    switch (msgIndex) {
      case 0: display.println("System Active"); break;
      case 1: display.println("Monitoring..."); break;
      case 2: display.println("Air Quality OK"); break;
      case 3: display.println("No Rain Detected"); break;
      case 4: display.println("Energy Saving Mode"); break;
    }
    display.display();
  }

  delay(10); 
}

// ================= TELEGRAM FUNCTIONS =================

void sendTelegramToAll(String message) {
  sendTelegram(chatID1, message);
  delay(500);
  sendTelegram(chatID2, message);
}

void sendTelegram(String chatID, String message) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    message.replace(" ", "%20" );
    message.replace("\n", "%0A");
    message.replace("!", "%21");

    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;

    http.begin(client, url );
    int httpResponseCode = http.GET( );
    Serial.print("Response: ");
    Serial.println(httpResponseCode );
    http.end( );
  }
}