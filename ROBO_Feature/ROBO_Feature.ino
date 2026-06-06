
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define FAN_PIN 4
#define RAIN_PIN 23
#define AIR_PIN 18


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

// 🔥 Alert system
bool alertMode = false;
bool toggleState = false;

unsigned long lastToggle = 0;
int toggleInterval = 600;

int currentMood = DEFAULT;
String alertText = "";

// 🔥 Idle system
unsigned long lastIdleChange = 0;
int idleInterval = 4000;
int idleState = 0;

// ================= SETUP =================
void setup() {
  pinMode(FAN_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  pinMode(AIR_PIN, INPUT);

  Wire.begin(21, 22);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 60);
  roboEyes.setAutoblinker(ON, 8, 2);
  roboEyes.setIdleMode(ON, 2, 1);
  roboEyes.setMood(DEFAULT);
  roboEyes.open();
}

// ================= LOOP =================
void loop() {

  int newMood = DEFAULT;
  bool eventActive = false;

  // 🔥 PRIORITY SYSTEM
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

  // ================= ALERT MODE =================
  if (eventActive) {

    if (!alertMode) {
      alertMode = true;
      currentMood = newMood;

      roboEyes.setMood(newMood);

      if (newMood == ANGRY) roboEyes.blink();
      if (newMood == HAPPY) roboEyes.anim_laugh();
      if (newMood == TIRED) roboEyes.anim_confused();
    }

    // toggle animation
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
    return;
  }

  // ================= NORMAL MODE =================
  alertMode = false;

  // 🔥 Mode control
  static int mode = 0;  // 0 = eyes, 1 = message
  static unsigned long lastSwitch = 0;
  static int msgIndex = 0;

  unsigned long eyesDuration = 5000;
  unsigned long msgDuration = 3000;

  // 🔥 switch mode
  if (mode == 0 && millis() - lastSwitch > eyesDuration) {
    mode = 1;
    lastSwitch = millis();

    // ✅ message change ONLY here
    msgIndex++;
    if (msgIndex > 4) msgIndex = 0;
  } else if (mode == 1 && millis() - lastSwitch > msgDuration) {
    mode = 0;
    lastSwitch = millis();
  }

  // ================= EYES MODE =================
  if (mode == 0) {
    roboEyes.setMood(DEFAULT);
    roboEyes.update();
    return;
  }

  // ================= MESSAGE MODE =================
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

  // 🔥 STOP HERE (no eyes)
  return;
}