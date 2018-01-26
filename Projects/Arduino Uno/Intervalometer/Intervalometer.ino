/*
 *  Intervalometer by PLS Version 1.4
 */
#include <TM1637Display.h>
#include <Timer.h>
#include <IRremote.h>
#include <RemoteForIntervalometer.h>

#define DIO         8    //Set the DIO pin connection to the display
#define CLK         9   //Set the CLK pin connection to the display
#define RELAY_PIN   10
#define IR_PIN      11

#define DELAY 100

unsigned long numToDisplay = 0;  //Variable to interate
uint8_t bright = 4;
bool displayState = true;
unsigned long interval = 0;

TM1637Display display(CLK, DIO);  //set up the 4-Digit Display.
RemoteForIntervalometer myRemote(1);
Timer *intervalometer;
Timer displayTimeout;
IRrecv* irDetect;
decode_results irIn;
enum mode {input, running, paused};
mode currentMode = input;
bool inputError = false;
void setup()
{
  irDetect = new IRrecv(IR_PIN);
  irDetect->enableIRIn(); // Start the Receiver
  display.setBrightness(0x0);  //set the diplay to maximum brightness
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
  displayTimeout.Start(60000);
}


void loop()
{
  if (irDetect->decode(&irIn)) {
    Serial.println(irValue, HEX);
    unsigned long irValue = irIn.value;
    irDetect->resume(); // Receive the next value
    RemoteForIntervalometer::remoteActions lastRemoteAction = myRemote.GetAction(irValue);
    if (lastRemoteAction > RemoteForIntervalometer::none &&  lastRemoteAction < RemoteForIntervalometer::endMarker) {
      // Control display state timer - Reset it on a key press
      if (lastRemoteAction!= RemoteForIntervalometer::toggleDiplay) {
        displayState = true;
      }
      displayTimeout.Start(60000);
    }
    if (lastRemoteAction >= RemoteForIntervalometer::one && lastRemoteAction <= RemoteForIntervalometer::zero && currentMode == input ) {
      // Process digit in input mode
      if (numToDisplay <= 999) {
        inputError = false;
        if (lastRemoteAction == RemoteForIntervalometer::zero) {
          numToDisplay *= 10;              
        } else {
          numToDisplay = numToDisplay * 10 + lastRemoteAction;              
        }
      }
    } else {
      switch (lastRemoteAction) {
        case RemoteForIntervalometer::start:
        case RemoteForIntervalometer::startFast:
          // Set snapshoot interval
          if ((currentMode == input && numToDisplay > 0) || currentMode == paused) {
            if (currentMode == input) {   
              if (lastRemoteAction == RemoteForIntervalometer::startFast) {
                if (numToDisplay % 100 > 59) {
                  numToDisplay = 0;
                  inputError = true;
                  break;
                } else {
                  interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
                }
              } else {
                interval = numToDisplay * 10;
              }
              takePicture();
              numToDisplay = 1;
              intervalometer = new Timer(interval);
            } 
            currentMode = running;
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
          // Display on / off
          displayState = !displayState;
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
        case RemoteForIntervalometer::takePicture:
          // Take a snapshoot with set display delay
          if (currentMode != input) {
            break;
          }
          display.setBrightness(bright, !displayState);  //set the diplay to maximum brightness
          display.showNumberDecEx(numToDisplay, currentMode == input ? 0xFF : 0x00, true); //Display the Variable value;
          delay(numToDisplay * 1000);
          takePicture();
          if (numToDisplay != 0) {
            delay(250);
          }
          display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
          display.showNumberDecEx(numToDisplay, currentMode == input ? 0xFF : 0x00, true); //Display the Variable value;
          break;
        case RemoteForIntervalometer::none:
          delay(DELAY);
          break;
      }
    }
  }
  if (displayTimeout.IsElapse()) {
    // Turn display off
    displayState = false;
  }
  display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
  if (!(numToDisplay == 0 && currentMode == input)) {
    display.showNumberDecEx(numToDisplay, currentMode == input ? 0xFF : 0x00, currentMode == input); //Display the Variable value;
  } else {
    // Set display to ----
    uint8_t data[] = {0x40, 0x40, 0x40, 0x40};
    display.setSegments(data);
  }
  if (currentMode == paused || inputError) {
    displayState = !displayState;
    display.setBrightness(bright, displayState);  //set the diplay brightness and state
    delay(500);
  }
  bool timerElapse = false;
  if (intervalometer != NULL) {
    timerElapse = intervalometer->IsElapse();
  }
  if (currentMode == running && timerElapse) {
    // Snap picture when timer elapse
    takePicture();
    numToDisplay++;
  } else {
    if (currentMode != running) {
      delay(100);
    }
  }
}

void takePicture()
{
  digitalWrite(RELAY_PIN, HIGH);
  delayMicroseconds(100000);  //
  delay(100);  
  digitalWrite(RELAY_PIN, LOW); 
}


