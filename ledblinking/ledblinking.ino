void setup(){
pinMode(8,OUTPUT);
pinMode(12,OUTPUT);
}

void loop(){
digitalWrite(8,HIGH);
digitalWrite(12,LOW);
delay(150);
digitalWrite(8,LOW);
digitalWrite(12,HIGH);
delay(150);
}
