#include <Servo.h>

// pin definitions
int const SENSOR1_PIN     = A1;
int const SENSOR2_PIN     = A2;
int const SENSOR3_PIN     = A3;
int const SENSOR4_PIN     = A4;
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
// gate speed constant: higher sensor1ue = slower
const int GATE_SPEED = 30;

// buzzer beep speed: higher sensor1ue = slower 
const int BUZZER_SPEED = 250;

// trigger value for IR sensor (the lower the value the closer the object must be)
const int SENSOR_TRIGGER = 850;

int pos = OPEN_POS;    // variable to store the servo position


/*
 * trainState description
 *  0 = no train within controlled region
 *  1 = train entered first sensor, but not second
 *  2 = train entered first and second sensor
 *  3 = train left first sensor, but not second
*/
const int TS_NO_TRAIN = 0;
const int TS_ENTERED  = 1;
const int TS_INSIDE   = 2;
const int TS_LEAVING  = 3;
int       trainState  = TS_NO_TRAIN;

/*
 * gateState description
 *  0 = gates open
 *  1 = warning phase
 *  2 = gate closing
 *  2 = gate closed
 *  3 = gate opening
*/
const int GS_OPEN    = 0;
const int GS_WARNING = 1;
const int GS_CLOSING = 2;
const int GS_CLOSED  = 3;
const int GS_OPENING = 4;
int gateState = GS_OPEN;

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
  //pinMode(SONSOR_LED_PIN, OUTPUT);  // set ledPin as output
  pinMode(SENSOR1_PIN, INPUT);       // set sensor pin as input
  pinMode(SENSOR2_PIN, INPUT);       // set sensor pin as input
  pinMode(SENSOR3_PIN, INPUT);       // set sensor pin as input
  pinMode(SENSOR4_PIN, INPUT);       // set sensor pin as input
  digitalWrite(SENSOR1_PIN, HIGH);   // set pullup on sensor pin
  digitalWrite(SENSOR2_PIN, HIGH);   // set pullup on sensor pin
  digitalWrite(SENSOR3_PIN, HIGH);   // set pullup on sensor pin
  digitalWrite(SENSOR4_PIN, HIGH);   // set pullup on sensor pin
}

int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;

bool sensor1Triggered = false;
bool sensor2Triggered = false;
bool sensor3Triggered = false;
bool sensor4Triggered = false;

int toneDelay      = 0;
int warningCounter = 0;
bool toneOn        = false;

void loop() {
  sensor1 = analogRead(SENSOR1_PIN);
  sensor2 = analogRead(SENSOR2_PIN);
  sensor3 = analogRead(SENSOR3_PIN);
  sensor4 = analogRead(SENSOR4_PIN);
  
  sensor1Triggered = sensor1 < SENSOR_TRIGGER;
  sensor2Triggered = sensor2 < SENSOR_TRIGGER;
  sensor3Triggered = sensor3 < SENSOR_TRIGGER;
  sensor4Triggered = sensor4 < SENSOR_TRIGGER;

  // first check the train state
  switch(trainState)
  {
    case TS_NO_TRAIN:
      // if one of the entering sensor is triggered
      if(sensor1Triggered || sensor4Triggered) 
      {
        // a train has entered the secured area
        Serial.println("trainState = TS_ENTERED");
        trainState = TS_ENTERED;
        close();
        break;
      }
      // if one of the inside sensor is triggered 
      if(sensor2Triggered || sensor3Triggered) 
      {
        // since the track should be free, this marks an error
        error();
      }
      break;
    case TS_ENTERED:
      // if one of the inside sensor is triggered 
      if(sensor2Triggered && !sensor1Triggered || sensor3Triggered && !sensor4Triggered)
      {
        Serial.println("trainState = TS_INSIDE");
        trainState = TS_INSIDE;
        break;
      }
      // if neither of the entering sensor is triggered
      if(!sensor1Triggered && !sensor4Triggered)
      {
        // a train must have reversed and exited
        Serial.println("trainState = TS_NO_TRAIN");
        trainState = TS_NO_TRAIN;
        open();
      }
      break;
    case TS_INSIDE:
      if(sensor1Triggered || sensor4Triggered)
      {
        Serial.println("trainState = TS_LEAVING");
        trainState = TS_LEAVING;
      }
      break;
    case TS_LEAVING:
      // if nothing is triggered anymore
      if(!sensor1Triggered && !sensor2Triggered && !sensor3Triggered && !sensor4Triggered)
      {
        Serial.println("trainState = TS_NO_TRAIN");
        trainState = TS_NO_TRAIN;
        open();
      }
      break;
    default:
      error();
  }

  switch(gateState)
  {
    case GS_OPEN:
      // stop sound...
      noTone(BUZZER_PIN);
      toneOn = false;
      toneDelay = 0;
      // turn off led
      digitalWrite(CROSSING_LED_1,LOW);
      digitalWrite(CROSSING_LED_2,LOW);
      break;
    case GS_OPENING:
      if(pos <= OPEN_POS)
      {
        pos += 1;
        myservo.write(pos);

        if(toneDelay > BUZZER_SPEED)
        {
          // reset tone delay counter
          toneDelay = 0;
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
      }
      else
      {
        gateState = GS_OPEN;
        Serial.println("gateState = GS_OPEN");
      }
      break;
    case GS_WARNING:
      if(toneDelay > BUZZER_SPEED)
      {
        // reset tone delay counter
        toneDelay = 0;
        if(toneOn)
        {
          noTone();
          toneOn = false;
          warningCounter++;
        }
        else
        {
          tone();
          toneOn = true;
        }
      }
      if(warningCounter >= 2)
      {
        Serial.println("gateState = GS_CLOSING");
        gateState = GS_CLOSING;
        warningCounter = 0;
      }
      break;
    case GS_CLOSING:
      if(pos >= CLOSE_POS)
      {
        pos -= 1;
        myservo.write(pos);

        if(toneDelay > BUZZER_SPEED)
        {
          // reset tone delay counter
          toneDelay = 0;
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
      }
      else
      {
        Serial.println("gateState = GS_CLOSED");
        gateState = GS_CLOSED;
      }
      break;
    case GS_CLOSED:
      // stop sound...
      noTone(BUZZER_PIN);
      // continue blinking
      if(toneDelay > BUZZER_SPEED)
      {
        // reset tone delay counter
        toneDelay = 0;
        if(toneOn)
        {
          digitalWrite(CROSSING_LED_1,LOW);
          digitalWrite(CROSSING_LED_2,HIGH);
          toneOn=false;
        }
        else
        {
          digitalWrite(CROSSING_LED_1,HIGH);
          digitalWrite(CROSSING_LED_2,LOW);
          toneOn=true;
        }
      }
      break;      
    default:
      error();
  }
  toneDelay+=GATE_SPEED;  
  delay(GATE_SPEED);
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
  switch(gateState)
  {
    case GS_OPEN:
    case GS_OPENING:
      break;
    case GS_CLOSED:
    case GS_WARNING:
    case GS_CLOSING:
      Serial.println("gateState = GS_OPENING");
      gateState = GS_OPENING;
      break;
    default:
      error();
  }
}

void close()
{
  switch(gateState)
  {
    // if gate open or currently opening
    case GS_OPEN:
    case GS_OPENING:
      // start closing it
      Serial.println("gateState = GS_WARNING");
      gateState = GS_WARNING;
      break;
    // otherwise, we are done
    case GS_CLOSED:
    case GS_WARNING:
    case GS_CLOSING:
      break;
    default:
      error();
  }
}

void error()
{
  Serial.println("ERROR!");  
  digitalWrite(CROSSING_LED_1,HIGH);
  digitalWrite(CROSSING_LED_2,HIGH);
  while(true);  // loop of doom
}

