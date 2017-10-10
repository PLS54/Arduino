#include <Timer.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

#define TRIG_PIN 11
#define ECHO_PIN 10
#define DOOR_PIN 9

bool triggerDone = false;
Timer myTimer;
Timer sentinel(60000);
bool doorMoving = false;
void setup() 
{
  Serial.begin (9600);
  for (byte i = 0; i <= A5; i++) {
    pinMode (i, OUTPUT);    // changed as per below
    digitalWrite (i, LOW);  //     ditto
  }
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(DOOR_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(DOOR_PIN, HIGH);    // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  Serial.println("Setup complete");
}

void loop() 
{
  long distance = MeasureDistance(TRIG_PIN, ECHO_PIN);
  if (distance <= 10.0) {
    if (!triggerDone) {
      pulse();
      myTimer.Start(15000);
      triggerDone = true;
      doorMoving = true;
    }
  } else {
    triggerDone = false;
  } 
  if (myTimer.IsElapse()) {
    if (doorMoving) {
      Serial.println("");
    }
    doorMoving = false;
  } else {
    Serial.print(".");
    delay(500); // do not send report too often
  }
  if (sentinel.IsElapse()) {
    Serial.print("T");
  }
  delay(500);
}


void pulse()
{
  digitalWrite(DOOR_PIN, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  for (int i = 0; i < 10; i++) {
    delayMicroseconds(10000);    
  }
  digitalWrite(DOOR_PIN, HIGH);   
  digitalWrite(LED_BUILTIN, LOW); 
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

// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect

void loseOneSecond()
{
  // disable ADC
ADCSRA = 0;  

// clear various "reset" flags
MCUSR = 0;     
// allow changes, disable reset
WDTCSR = bit (WDCE) | bit (WDE);
// set interrupt mode and an interval 
WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1);    // set WDIE, and 1 second delay
wdt_reset();  // pat the dog

set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
noInterrupts ();           // timed sequence follows
sleep_enable();

// turn off brown-out enable in software
MCUCR = bit (BODS) | bit (BODSE);
MCUCR = bit (BODS); 
interrupts ();             // guarantees next instruction executed
sleep_cpu ();  

// cancel sleep as a precaution
sleep_disable();

}

