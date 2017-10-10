#define TRIG_PIN 11
#define ECHO_PIN 10
#define DOOR_PIN 9
#define MIN_DIST 4.0
#define MAX_DIST 200.0

bool currentDoorState = false;;
bool doPulse = false;
byte loopCounter = 0;

ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(getter, setter));

void setup() 
{
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
    pulse(200);
    doPulse = false;
    delay(1000);
    zunoSendReport(1);
  }
  delay(500);
  loopCounter++;
  if (loopCounter >= 10) {
    // sanity check every 10 sec.
    loopCounter = 0;
    currentDoorState = getDoorState();
    zunoSendReport(1);
  }
}

bool getDoorState()
{
  return MeasureDistance(TRIG_PIN, ECHO_PIN) > 10 ? true : false;
}

void pulse(int duration)
{
  digitalWrite(DOOR_PIN, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(duration);
  digitalWrite(DOOR_PIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
}

long MeasureDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return (pulseIn(echoPin, HIGH, 20000) / 58.2);
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

