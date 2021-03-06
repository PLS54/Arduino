#define VERSION "2.3"
//
// Garage door opener
//
#include <Timer.h>

#define TRIG_PIN 11
#define ECHO_PIN 10
#define DOOR_PIN 9
#define LOOP_DELAY 500
#define DOOR_OPEN_TIME 15000
#define PULSE_WIDTH 200
#define CLOSE_DISTANCE 10.0
#define REPORT_TIME 300000

bool getDoorState();

bool currentDoorState = getDoorState();
bool doPulse = false;
Timer doorOpeningTimer;
Timer reporter(REPORT_TIME);

ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(getter, setter));

void setup() 
{
  Serial.begin(9600);
  pinMode(DOOR_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(DOOR_PIN, HIGH);    // turn the LED off by making the voltage LOW
  zunoSendReport(1);
}

void loop() 
{ 
  bool newDoorState = getDoorState();
  if (newDoorState != currentDoorState) {
    currentDoorState = newDoorState;
    zunoSendReport(1);
  }
  if (doPulse) {
    reportStatus();
    Serial.println("Pulsing door");
    pulse(PULSE_WIDTH);
    doPulse = false;
    doorOpeningTimer.Start(DOOR_OPEN_TIME);
  }
  if (!doorOpeningTimer.IsElapse()) {
    //
    // When door is moving send report to controller every 1/2 sec.
    //
    currentDoorState = getDoorState();
    zunoSendReport(1);
  }
  if (reporter.IsElapse()) {
    reportStatus();
    //
    currentDoorState = getDoorState();
    zunoSendReport(1);
  }
  delay(LOOP_DELAY);
}

void reportStatus()
{
  Serial.print("Garage Door Opener version ");
  Serial.println(VERSION);
  Serial.println(MeasureDistance(TRIG_PIN, ECHO_PIN));
}

bool getDoorState()
{
  return MeasureDistance(TRIG_PIN, ECHO_PIN) > CLOSE_DISTANCE ? true : false;
}

void pulse(int duration)
{
  digitalWrite(DOOR_PIN, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(duration);
  digitalWrite(DOOR_PIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}

float MeasureDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long pulseLength = pulseIn(echoPin, HIGH, 1000000
  );
  if (pulseLength == 0) {
    return 100.0;
  }
  return (pulseLength / 58.2);
}

void setter (byte value) 
{
  // Since delay caanot be used in a z uno function tell the loop to do it
  doPulse = true;
}
byte getter ()
{
  return currentDoorState ? 255 : 0;
}

