const int BUZZER_PIN = 8;

void setup() {
  // set buzzer pin to output mode
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {

  tone(BUZZER_PIN, 1000); // send 1kHz signal...
  delay(250);            // ... for 1 sec...
  noTone(BUZZER_PIN);     // stop sound...
  delay(250);            // ... for 1 sec
}
