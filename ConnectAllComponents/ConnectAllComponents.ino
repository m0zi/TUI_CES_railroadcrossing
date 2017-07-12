#include <Servo.h>

// pin definitions
int const SENSOR1_PIN     = A0;
int const SENSOR2_PIN     = A1;
int const SENSOR3_PIN     = A2;
int const SENSOR4_PIN     = A3;
int const BUZZER_PIN     =  8;
int const SERVO_PIN      =  9;
int const CROSSING_LED_1 = 11;
int const CROSSING_LED_2 = 12;

// gate speed constant: higher value = slower
const int GATE_SPEED = 10;

// buzzer beep speed: higher value = slower 
const int BUZZER_SPEED = 250;

// trigger value for IR sensor: higher value = more distance
const int SENSOR_TRIGGER = 850;

// servo's open and close position
const int CLOSE_POS =  53;
const int OPEN_POS  = 139;

// create servo object to control a servo
Servo myservo;

// variable to store the servo position
int pos = OPEN_POS;

/*
 * trainState description
 *  0 = no train within controlled region
 *  1 = train entered secured area
 *  2 = train is inside secured area
 *  3 = train can only leave the area
*/
const int TS_NO_TRAIN = 0;
const int TS_ENTERED  = 1;
const int TS_INSIDE   = 2;
const int TS_LEAVING  = 3;
int       trainState  = TS_NO_TRAIN;

/*
 * gateState description
 *  0 = gates open
 *  1 = warning phase before gate starts to close
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
  myservo.write(pos);         // initialize position

  // initialize buzzer and crossing blink leds
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(CROSSING_LED_1,OUTPUT);
  pinMode(CROSSING_LED_2,OUTPUT);

  // initialize sensor and its debugging led
  analogReference(DEFAULT);         // Analog Reference of 5V is default, I know not needed.
  pinMode(SENSOR1_PIN, INPUT);       // set sensor pin as input
  pinMode(SENSOR2_PIN, INPUT);       // set sensor pin as input
  pinMode(SENSOR3_PIN, INPUT);       // set sensor pin as input
  pinMode(SENSOR4_PIN, INPUT);       // set sensor pin as input
  digitalWrite(SENSOR1_PIN, HIGH);   // set pullup on sensor pin
  digitalWrite(SENSOR2_PIN, HIGH);   // set pullup on sensor pin
  digitalWrite(SENSOR3_PIN, HIGH);   // set pullup on sensor pin
  digitalWrite(SENSOR4_PIN, HIGH);   // set pullup on sensor pin
}

// initialize sensor value
int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;
int sensor4 = 0;

// initialize sensor triggers
bool sensor1Triggered = false;
bool sensor2Triggered = false;
bool sensor3Triggered = false;
bool sensor4Triggered = false;

// added delay for tone and blinking (or in closed state blinking only)
int toneDelay      = 0;
// counts the number of warning tones and blinkings
int warningCounter = 0;
// describes whether tone is one or of (also applies to binking state)
bool toneOn        = false;

void loop() {
  // read all sensor values
  sensor1 = analogRead(SENSOR1_PIN);
  sensor2 = analogRead(SENSOR2_PIN);
  sensor3 = analogRead(SENSOR3_PIN);
  sensor4 = analogRead(SENSOR4_PIN);

  // decide trigger or not
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
        printSensors();
        trainState = TS_ENTERED;
        printTrainState();
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
      // if one of the inside sensor is triggered while the entering is still triggered
      if(sensor2Triggered && !sensor1Triggered || sensor3Triggered && !sensor4Triggered)
      {
        printSensors();
        trainState = TS_INSIDE;
        printTrainState();
        break;
      }
      // if neither of the entering sensor is triggered
      if(!sensor1Triggered && !sensor4Triggered)
      {
        // a train must have reversed and exited
        printSensors();
        trainState = TS_NO_TRAIN;
        printTrainState();
        open();
        break;
      }
      break;
    case TS_INSIDE:
      if(sensor1Triggered && !sensor2Triggered || !sensor3Triggered && sensor4Triggered)
      {
        printSensors();
        trainState = TS_LEAVING;
        printTrainState();
        break;
      }
      break;
    case TS_LEAVING:
      // if nothing is triggered anymore
      if(!sensor1Triggered && !sensor2Triggered && !sensor3Triggered && !sensor4Triggered)
      {\
        printSensors();
        trainState = TS_NO_TRAIN;
        printTrainState();
        open();
        break;
      }
      // if one of the inside sensor is triggered while the entering is still triggered
      if(!sensor1Triggered && sensor2Triggered || sensor3Triggered && !sensor4Triggered)
      {
        printSensors();
        trainState = TS_INSIDE;
        printTrainState();
        break;
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
      // open the gate
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
      // gate is completely opened
      else
      {
        gateState = GS_OPEN;
        printGateState();
      }
      break;
    case GS_WARNING:
      // send warning signal...
      if(toneDelay > BUZZER_SPEED)
      {
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
      // ... not more than twice
      if(warningCounter >= 1)
      {
        gateState = GS_CLOSING;
        printGateState();
        warningCounter = 0;
      }
      break;
    case GS_CLOSING:
      // close the gate
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
      // gate in completely closed
      else
      {
        gateState = GS_CLOSED;
        printGateState();
      }
      break;
    case GS_CLOSED:
      // stop sound...
      noTone(BUZZER_PIN);
      // continue blinking (no sound)
      if(toneDelay > BUZZER_SPEED)
      {
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
      error();  // shouldn't happen
  }
  // increase delay counter by gcd (kind of)
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
      gateState = GS_OPENING;
      printGateState();
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
      gateState = GS_WARNING;
      printGateState();
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

void printSensors()
{
  /*
  Serial.println("Sensors values: ");
  Serial.print(sensor1);
  Serial.print(" ");
  Serial.print(sensor2);
  Serial.print(" ");
  Serial.print(sensor3);
  Serial.print(" ");
  Serial.print(sensor4);
  Serial.println(" -");
  //*/
}

void printTrainState()
{
  ///*
  Serial.print("trainState = ");
  switch (trainState)
  {
    case TS_NO_TRAIN:
      Serial.println("TS_NO_TRAIN");
      break;
    case TS_ENTERED:
      Serial.println("TS_ENTERED");
      break;
    case TS_INSIDE:
      Serial.println("TS_INSIDE");
      break;
    case TS_LEAVING:
      Serial.println("TS_LEAVING");
      break;
    default:
      Serial.println("unknown");
      break;
  }
  //*/
}

void printGateState()
{
  /*
  Serial.print("gateState = ");
  switch (gateState)
  {
    case GS_OPEN:
      Serial.println("GS_OPEN");
      break;
    case GS_OPENING:
      Serial.println("GS_OPENING");
      break;
    case GS_WARNING:
      Serial.println("GS_WARNING");
      break;
    case GS_CLOSING:
      Serial.println("GS_CLOSING");
      break;
    case GS_CLOSED:
      Serial.println("GS_CLOSED");
      break;
    default:
      Serial.println("unknown");
      break;
  }
  //*/
}

void error()
{
  Serial.println("ERROR!");  
  digitalWrite(CROSSING_LED_1,HIGH);
  digitalWrite(CROSSING_LED_2,HIGH);
  while(true);  // loop of doom
}

