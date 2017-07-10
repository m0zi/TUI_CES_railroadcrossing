#include <Servo.h>

// pin definitions
int const SENSOR_PIN     = A1;
int const BUZZER_PIN     =  8;
int const SERVO_PIN      =  9;
int const CROSSING_LED_1 = 11;
int const CROSSING_LED_2 = 12;
int const SONSOR_LED_PIN = 13;

// create servo object to control a servo
Servo myservo;

// servo's open and close position
const int CLOSE_POS =  53;
const int OPEN_POS  = 139;

int pos = OPEN_POS;    // variable to store the servo position


void setup() {
  // initialize servo
  myservo.attach(SERVO_PIN);  // attaches the servo on its defined pin to the servo object
  myservo.write(pos);

  // initialize buzzer and crossing blink leds
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(CROSSING_LED_1,OUTPUT);
  pinMode(CROSSING_LED_2,OUTPUT);

  // initialize sensor and its debugging led
  analogReference(DEFAULT);         // Analog Reference of 5V is default, I know not needed.
  pinMode(SONSOR_LED_PIN, OUTPUT);  // set ledPin as output
  pinMode(SENSOR_PIN, INPUT);       // set sensor pin as input
  digitalWrite(SENSOR_PIN, HIGH);   // set pullup on sensor pin
}

int val = 0;

void loop() {
  val = analogRead(SENSOR_PIN);

  if(val < 1000) 
  {
    digitalWrite(SONSOR_LED_PIN, HIGH);       // value less then 1000, turn on led
    close();
  }
  else 
  {
    digitalWrite(SONSOR_LED_PIN, LOW);        // value greater then 1000, turn off led
    open();
  }

}

void open()
{
  for (; pos <= OPEN_POS; pos += 1)
  {
    myservo.write(pos);
    delay(15);
  }
}

void close()
{
  for (; pos >= CLOSE_POS; pos -= 1)
  {
    myservo.write(pos);
    delay(15);
  }
}

