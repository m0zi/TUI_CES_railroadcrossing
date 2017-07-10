const int BUZZER_PIN = 8;

void setup() {
  // set buzzer pin to output mode
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(11,OUTPUT); // led blinking only
  pinMode(12,OUTPUT); // led blinking only

}

void loop() {

  
  digitalWrite(11,HIGH);  // for led blinking
  digitalWrite(12,LOW);   
  tone(BUZZER_PIN, 1000); // send 1kHz signal...
  delay(250);            // ... for 1 sec...
  digitalWrite(11,LOW);
  digitalWrite(12,HIGH);
  noTone(BUZZER_PIN);     // stop sound...
  delay(250);            // ... for 1 sec

}
