 
int sensePin = A1;
int ledPin = 13;

void setup() {
  Serial.begin(9600);
  
  analogReference(DEFAULT);           // Analog Reference of 5V is default, I know not needed.
  pinMode(ledPin, OUTPUT);            // set ledPin as output
  pinMode(sensePin, INPUT);           // set sensePin as input
  digitalWrite(sensePin, HIGH);       // set pullup on analog 1
}

void loop() {
  int val = analogRead(sensePin);

  Serial.print("IR Sensor Value = ");
  Serial.println(val);
  Serial.println("");

  if(val < 750) 
    digitalWrite(ledPin, HIGH);       // value less then 1000, turn on led
  else 
    digitalWrite(ledPin, LOW);        // value greater then 1000, turn off led
  delay(50);
  
} 








