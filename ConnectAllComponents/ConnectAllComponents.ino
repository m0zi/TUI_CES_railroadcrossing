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
// gate speed constant: higher value = slower
const int GATE_SPEED = 30;

// buzzer beep speed: higher value = slower 
const int BUZZER_SPEED = 250;

int pos = OPEN_POS;    // variable to store the servo position


/*
 * state description
 *  0 = gates open, no train within controlled region
 *  1 = train entered region, gates are about to close
 *  2 = train within region, gates closed
 *  3 = train left region, gates are opening
 * -1 = errornous state, system will reset
*/
int state = 0;

void setup() {
  Serial.begin(9600);
  
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

  Serial.print("IR Sensor Value = ");
  Serial.println(val);
  Serial.println("");

  switch(state)
  {
    case 0:
      if(val < 750) 
      {
        digitalWrite(SONSOR_LED_PIN, HIGH);
        close();
        digitalWrite(CROSSING_LED_1,LOW);
        digitalWrite(CROSSING_LED_2,LOW);
        state = 2;
      }
      break;
    case 2:
      if(val > 750) 
      {
        digitalWrite(SONSOR_LED_PIN, LOW);
        open();
        digitalWrite(CROSSING_LED_1,LOW);
        digitalWrite(CROSSING_LED_2,LOW);
        state = 0;
      }
      break;
    default:
      delay(15);
  }
}

void tone()
{
  // send 1kHz signal...
  tone(BUZZER_PIN, 1000);
  digitalWrite(CROSSING_LED_1,HIGH);
  digitalWrite(CROSSING_LED_2,LOW);
}

void noTone()
{
  // stop sound...  
  noTone(BUZZER_PIN);
  digitalWrite(CROSSING_LED_1,LOW);
  digitalWrite(CROSSING_LED_2,HIGH);
}


void open()
{
  // warning signal that crossing will close soon
  int i;
  for(i = 0; i < 2; i++)
  {
    tone();
    delay(BUZZER_SPEED);
    noTone();
    delay(BUZZER_SPEED);
  }
  
  int toneDeplay = 0;
  bool toneOn = true;
  tone(BUZZER_PIN, 1000);           // send 1kHz signal...
  for (; pos <= OPEN_POS; pos += 1)
  {
    myservo.write(pos);

    // generate sound
    if(toneDeplay > BUZZER_SPEED)
    {
      // reset tone delay counter
      toneDeplay = 0;
      if(toneOn)
      {
        noTone();
        toneOn=false;
      }
      else
      {
        tone();
        toneOn=true;
      }
    }
    toneDeplay+=GATE_SPEED;
    delay(GATE_SPEED);
  }

  // fading out
  for(i = 0; i < 2; i++)
  {
    tone();
    delay(BUZZER_SPEED);
    noTone();
    delay(BUZZER_SPEED);
  }
}

void close()
{
  // warning signal that crossing will close soon
  int i;
  for(i = 0; i < 2; i++)
  {
    tone();
    delay(BUZZER_SPEED);
    noTone();
    delay(BUZZER_SPEED);
  }

  int toneDeplay = 0;
  bool toneOn = true;
  tone(BUZZER_PIN, 1000);           // send 1kHz signal...
  for (; pos >= CLOSE_POS; pos -= 1)
  {
    myservo.write(pos);
    // generate sound
    if(toneDeplay > BUZZER_SPEED)
    {
      // reset tone delay counter
      toneDeplay = 0;
      if(toneOn)
      {
        noTone();
        toneOn=false;
      }
      else
      {
        tone();
        toneOn=true;
      }
    }
    toneDeplay+=GATE_SPEED;
    delay(GATE_SPEED);
  }

  // fading out
  for(i = 0; i < 2; i++)
  {
    tone();
    delay(BUZZER_SPEED);
    noTone();
    delay(BUZZER_SPEED);
  }
}
