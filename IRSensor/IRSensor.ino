 
int sensePin = A1;
int ledPin = 13;

void setup() {
  analogReference(DEFAULT);           // Analog Reference of 5V is default, I know not needed.
  pinMode(ledPin, OUTPUT);            // set ledPin as output
  pinMode(sensePin, INPUT);           // set sensePin as input
  digitalWrite(sensePin, HIGH);       // set pullup on analog 1
}

void loop() {
  int val = analogRead(sensePin);

  if(val < 1000) 
    digitalWrite(ledPin, HIGH);       // value less then 1000, turn on led
  else 
    digitalWrite(ledPin, LOW);        // value greater then 1000, turn off led
  delay(50);
} 








