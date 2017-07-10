#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

const int CLOSE_POS =  53;
const int OPEN_POS  = 139;

const int buzzer = 14;

int pos = OPEN_POS;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(pos);
  delay(1000);
  
  pinMode(buzzer, OUTPUT);
  beep(50);
  beep(50);
}

void loop() {

  open();
  delay(2000);
  close();
  delay(1000);
/*
myservo.write(50);
delay(1000);
 
myservo.write(139);
delay(5000);
*/
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

void beep(unsigned char delayms) { //creating function
  analogWrite(buzzer, 20); //Setting pin to high
  delay(delayms); //Delaying
  analogWrite(buzzer ,0); //Setting pin to LOW
  delay(delayms); //Delaying
  
}

