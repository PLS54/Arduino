/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#include "Potentiometer.h"

float oldVoltage = -1.0;
int counter = 1;
Potentiometer pot(A2, 1, 10);
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Blick tester");
}

// the loop function runs over and over again forever
void loop()
{
  //Serial.println("Start of loop");
  //float voltage = pot.GetValue(0.005);
  //if (voltage  != oldVoltage) {
//    // print out the value you read:
//    char tst[50];
//    sprintf(tst, "%d - value %d.%d", counter++, (int)voltage, (int)(voltage * 1000.0) % 1000); 
//    Serial.println(tst);
//    oldVoltage = voltage;
//    if (Potentiometer::InBetween(voltage, 4.99, 5.0f))
//    {
//    }
//  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  pinMode(LED_BUILTIN, INPUT);
  delay(990);                       // wait for a second
}

float roundNearest(float value, float decimal)
{
  return round(value * (1.0 / decimal)) / (1.0 / decimal);
}

void flashSOS()
{
  flashS();
  flashO();
  flashS();
}
void flashS()
{
  shortDash();
  shortDash();
  shortDash();
}
void flashO()
{
  longDash();
  longDash();
  longDash();
}
#define DELAY 500
#define LONG_DASH 500
#define SHORT_DASH LONG_DASH / 2

void longDash()
{
  if (true) return;
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(LONG_DASH);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(DELAY);                       // wait for a second
}

void shortDash()
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(SHORT_DASH);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(DELAY);                       // wait for a second
}

