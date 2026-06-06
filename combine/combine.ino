// ===================== STAIRCASE SETUP =====================

// LED pins (bottom → top)
int leds[] = {9, 10, 11};
int totalLeds = 3;

// IR sensor pins
int irBottom = 6;   // UP
int irTop = 7;      // DOWN


// ===================== SAFETY SYSTEM =====================

int sensePin = 2;
int buzzerPin = 8;
int alertLed = 5;   // separate LED for alert


// ===================== SETUP =====================

void setup() {

  // Staircase LEDs
  for (int i = 0; i < totalLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }

  pinMode(irBottom, INPUT);
  pinMode(irTop, INPUT);

  // Safety system
  pinMode(sensePin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(alertLed, OUTPUT);
}


// ===================== LOOP =====================

void loop() {

  // ===== SAFETY SYSTEM (ALWAYS RUNNING) =====
  int state = digitalRead(sensePin);

  if (state == HIGH) {
    tone(buzzerPin, 1000);
    digitalWrite(alertLed, HIGH);
  }
  else {
    noTone(buzzerPin);
    pinMode(buzzerPin, INPUT);   // heat fix
    digitalWrite(alertLed, LOW);

    pinMode(buzzerPin, OUTPUT);  // restore
  }
  

  // ===== STAIRCASE UP =====
  if (digitalRead(irBottom) == LOW) {

    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], HIGH);
      delay(500);
    }

    delay(3000);

    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], LOW);
    }
  }


  // ===== STAIRCASE DOWN =====
  if (digitalRead(irTop) == LOW) {

    for (int i = totalLeds - 1; i >= 0; i--) {
      digitalWrite(leds[i], HIGH);
      delay(500);
    }

    delay(3000);

    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], LOW);
    }
  }
}