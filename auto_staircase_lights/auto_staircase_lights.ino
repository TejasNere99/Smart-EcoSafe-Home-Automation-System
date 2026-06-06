// LED pins (bottom → top)
int leds[] = {9, 10, 11};
int totalLeds = 3;

// IR sensor pins
int irBottom = 6;   // detects person going UP
int irTop = 7;      // detects person going DOWN

void setup() {
  // Set LED pins as OUTPUT
  for (int i = 0; i < totalLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }

  // Set IR pins as INPUT
  pinMode(irBottom, INPUT);
  pinMode(irTop, INPUT);
}

void loop() {

  // 🔺 Person going UP (bottom → top)
  if (digitalRead(irBottom) == LOW) {
    
    // LEDs ON one by one (D9 → D10 → D11)
    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], HIGH);
      delay(500);
    }

    delay(3000); // keep ON

    // Turn OFF all LEDs
    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], LOW);
    }
  }

  // 🔻 Person going DOWN (top → bottom)
  if (digitalRead(irTop) == LOW) {
    
    // LEDs ON reverse (D11 → D10 → D9)
    for (int i = totalLeds - 1; i >= 0; i--) {
      digitalWrite(leds[i], HIGH);
      delay(500);
    }

    delay(3000); // keep ON

    // Turn OFF all LEDs
    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], LOW);
    }
  }
}