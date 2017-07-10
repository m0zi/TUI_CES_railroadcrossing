#include <Servo.h>

// pin definitions
const int SERVO_PIN = 9;

// create servo object to control a servo
Servo myservo;

// servo's open and close position
const int CLOSE_POS =  53;
const int OPEN_POS  = 139;

int pos = OPEN_POS;    // variable to store the servo position

void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on its defined pin to the servo object
  myservo.write(pos);
  delay(1000);
}

void loop() {

  open();
  delay(2000);
  close();
  delay(1000);
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
