#include <TM1637Display.h>
#include <Timer.h>
#include <IRremote.h>
#include <RemoteForIntervalometer.h>

const int CLK = 9; //Set the CLK pin connection to the display
const int DIO = 8; //Set the DIO pin connection to the display
#define IR_PIN 12
#define DELAY 100
#define RELAY_PIN 11


int numToDisplay = 0;  //Variable to interate
uint8_t bright = 4;
bool state = true;
unsigned long interval = 0;

TM1637Display display(CLK, DIO);  //set up the 4-Digit Display.
RemoteForIntervalometer myRemote(1);
Timer *intervalometer;
IRrecv* irDetect;
decode_results irIn;
enum mode {input, running, paused};
mode currentMode = input;
bool error = false;
void setup()
{
  irDetect = new IRrecv(IR_PIN);
  irDetect->enableIRIn(); // Start the Receiver
  display.setBrightness(0x0);  //set the diplay to maximum brightness
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
}


void loop()
{
  if (irDetect->decode(&irIn)) {
    unsigned long irValue = irIn.value;
    irDetect->resume(); // Receive the next value
    if (myRemote.GetAction(irValue) >= RemoteForIntervalometer::one && myRemote.GetAction(irValue) <= RemoteForIntervalometer::zero && currentMode == input ) {
      if (numToDisplay <= 999) {
        error = false;
        if (myRemote.GetAction(irValue) == RemoteForIntervalometer::zero) {
          numToDisplay *= 10;              
        } else {
          numToDisplay = numToDisplay * 10 + myRemote.GetAction(irValue);              
        }
      }
    } else {
      switch (myRemote.GetAction(irValue)) {
        case RemoteForIntervalometer::start:
        case RemoteForIntervalometer::startFast:
          if ((currentMode == input && numToDisplay > 0) || currentMode == paused) {
            if (currentMode == input) {   
              if (myRemote.GetAction(irValue) == RemoteForIntervalometer::startFast) {
                if (numToDisplay % 100 > 59) {
                  numToDisplay = 0;
                  error = true;
                  break;
                } else {
                  interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
                }
              } else {
                interval = numToDisplay * 10;
              }
              numToDisplay = 0;
            }
            state = true;
            currentMode = running;
            intervalometer = new Timer(interval);
          }
          break;
        case RemoteForIntervalometer::stop:
          if (currentMode == running || currentMode == paused) {
            currentMode = input;
            numToDisplay = 0;
          }
          break;
        case RemoteForIntervalometer::pause:
          if (currentMode == running) {
            currentMode = paused;
          }
          break;
        case RemoteForIntervalometer::brightDown:
          if (bright > 0) {
            bright--;
            delay(500);
          }
          break;
        case RemoteForIntervalometer::brightUp:
          if (bright < 7) {
            bright++;
            delay(500);
          }
          break;
        case RemoteForIntervalometer::toggleDiplay:
        state = !state;
          break;
        case RemoteForIntervalometer::deleteLastChar:
          if (numToDisplay >= 10) {
            numToDisplay /= 10;
          } else {
            numToDisplay = 0;
          }
          break;
        case RemoteForIntervalometer::resetDisplay:
          numToDisplay = 0;
          break;
        case RemoteForIntervalometer::none:
          delay(DELAY);
          break;
      }
    }
  }
  display.setBrightness(bright, state);  //set the diplay to maximum brightness
  if (!(numToDisplay == 0 && currentMode == input)) {
    display.showNumberDecEx(numToDisplay, currentMode == input ? 0xFF : 0x00, true); //Display the Variable value;
  } else {
    uint8_t data[] = {0x40, 0x40, 0x40, 0x40};
    display.setSegments(data);
  }
  if (currentMode == paused || error) {
    state = !state;
    display.setBrightness(bright, state);  //set the diplay to maximum brightness
    delay(500);
  }
  if (currentMode == running && intervalometer->IsElapse()) {
    pulse();
    numToDisplay++;
  } else {
    if (currentMode != running) {
      delay(100);
    }
  }
}

void pulse()
{
  digitalWrite(RELAY_PIN, HIGH);
  delayMicroseconds(10000);    
  digitalWrite(RELAY_PIN, LOW); 
}


