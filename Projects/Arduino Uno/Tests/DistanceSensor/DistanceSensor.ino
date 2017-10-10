/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
 560 ohm resistor to both LED NEG and GRD power rail
 More info at: http://goo.gl/kJ8Gl
 Original code improvements to the Ping sketch sourced from Trollmaker.com
 Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
 */

#define TRIG_PIN 9
#define ECHO_PIN 8

void setup() 
{
  Serial.begin (9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() 
{
  long distance = MeasureDistance(TRIG_PIN, ECHO_PIN);
  if (distance >= 20000 || distance <= 0)
  {
    Serial.println("Out of range");
  }  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(500);
}

long MeasureDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  return (pulseIn(echoPin, HIGH) / 58.2);
} 

