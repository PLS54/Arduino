/*
 *  Intervalometer by PLS Version 1.7
 */
#include <Timer.h>
#include <IRremote.h>
#include <RemoteForIntervalometer.h>
#include <DisplayForIntervalometer.h>

#define DIO         8    //Set the DIO pin connection to the display
#define CLK         9   //Set the CLK pin connection to the display
#define RELAY_PIN   10
#define IR_PIN      11

#define DELAY 100

unsigned long numToDisplay = 0;  //Variable to interate
uint8_t bright = 4;
bool displayState = true;
unsigned long interval = 0;

RemoteForIntervalometer myRemote(1);
DisplayForIntervalometer myDisplay(CLK, DIO);
Timer intervalometer;
Timer displayTimeout;
Timer flashTimer(500);
IRrecv* irDetect;
decode_results irIn;
enum mode {initial, input, instant, running, paused};
mode currentMode = input;
bool inputError = false;
unsigned int previousDisplay = 0;



void setup()
{
  Serial.begin(9600);
  irDetect = new IRrecv(IR_PIN);
  irDetect->enableIRIn(); // Start the Receiver
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
  displayTimeout.Start(60000);
}


void loop()
{
  if (irDetect->decode(&irIn)) {
    unsigned long irValue = irIn.value;
    irDetect->resume(); // Receive the next value
    RemoteForIntervalometer::remoteActions lastRemoteAction = myRemote.GetAction(irValue);
    if (lastRemoteAction > RemoteForIntervalometer::none &&  lastRemoteAction < RemoteForIntervalometer::endMarker) {
      // Control display state timer - Reset it on a key press
      if (lastRemoteAction != RemoteForIntervalometer::toggleDiplay) {
        myDisplay.TurnDisplayOn();
      }
      displayTimeout.Restart();
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
      myDisplay.ChangeMode(DisplayForIntervalometer::input);
      myDisplay.SetNewValue(numToDisplay);
    } else {
      switch (lastRemoteAction) {
        case RemoteForIntervalometer::start:
        case RemoteForIntervalometer::startFast:
          // Set snapshoot interval
          if ((currentMode == input && numToDisplay > 0) || currentMode == paused) {
            displayTimeout.Start(60000);
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
              intervalometer.StartAuto(interval);
              myDisplay.ChangeMode(DisplayForIntervalometer::count);
              myDisplay.SetNewValue(numToDisplay);
            } 
            currentMode = running;
          }
          break;
        case RemoteForIntervalometer::stop:
          if (currentMode == running || currentMode == paused || currentMode == instant) {
            currentMode = input;
            numToDisplay = 0;
            myDisplay.ChangeMode(DisplayForIntervalometer::input);
            myDisplay.SetNewValue(numToDisplay);
          }
          displayTimeout.Stop();
          break;
        case RemoteForIntervalometer::pause:
          if (currentMode == running) {
            currentMode = paused;
            flashTimer.StartAuto(500);
          }
          break;
        case RemoteForIntervalometer::brightDown:
          myDisplay.DecreaseBrighness();
          break;
        case RemoteForIntervalometer::brightUp:
          myDisplay.IncreaseBrighness();
          break;
        case RemoteForIntervalometer::toggleDiplay:
          myDisplay.ToggleDisplayState();
          break;
        case RemoteForIntervalometer::deleteLastChar:
          if (currentMode != input) {
            break;
          }
          if (numToDisplay >= 10) {
            numToDisplay /= 10;
          } else {
            numToDisplay = 0;
          }
          myDisplay.SetNewValue(numToDisplay);
          break;
        myDisplay.SetNewValue(numToDisplay);
        case RemoteForIntervalometer::resetDisplay:
          if (currentMode != input) {
            break;
          }
          numToDisplay = 0;
          myDisplay.SetNewValue(numToDisplay);
          break;
        case RemoteForIntervalometer::takePicture:
          displayTimeout.Stop();
          if (currentMode == input) {
            currentMode = instant;
            intervalometer.Stop();
            if (numToDisplay > 0) {
              previousDisplay = numToDisplay;
              interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
              flashTimer.StartAuto(500);
              intervalometer.StartAuto(interval - 100);
              myDisplay.ChangeMode(DisplayForIntervalometer::time);
              myDisplay.SetNewValue(numToDisplay);
            }
          }
          break;
        case RemoteForIntervalometer::none:
          delay(DELAY);
          break;
      }
    }
  }
  if (displayTimeout.IsElapse()) {
    //myDisplay.TurnDisplayOff();
  }
  if (currentMode == paused || inputError) {
    if (flashTimer.IsElapse()) {
      myDisplay.ToggleDisplayState();
    }
  }
  if (currentMode == running && intervalometer.IsElapse()) {
    // Snap picture when timer elapse
    takePicture();
    numToDisplay++;
    myDisplay.SetNewValue(numToDisplay);
  } else if (currentMode == instant) {
    ProcessIntantMode();
  } else {
    if (currentMode != running) {
      delay(100);
    }
  }
  if (currentMode == input) {
    myDisplay.ChangeMode(DisplayForIntervalometer::input);
  }
  if (currentMode == instant) {
    myDisplay.ChangeMode(DisplayForIntervalometer::time);
  }
  if (currentMode == running) {
    myDisplay.ChangeMode(DisplayForIntervalometer::count);
  }
}

void takePicture()
{
  digitalWrite(RELAY_PIN, HIGH);
  delayMicroseconds(100000);  //
  delay(100);  
  digitalWrite(RELAY_PIN, LOW); 
}

void ProcessIntantMode()
{
  if (!intervalometer.Running()) {
    myDisplay.ToggleDisplayState();
    takePicture();
    myDisplay.ToggleDisplayState();
    currentMode = input;
  } else {
    if (intervalometer.IsElapse()) {
      myDisplay.TurnDisplayOn();
      myDisplay.SetNewValue(0);
      delay(100);
      takePicture();  
      currentMode = input;
      numToDisplay = previousDisplay;
      myDisplay.SetNewValue(numToDisplay);
    } else {
      numToDisplay = intervalometer.GetTimeLeftBeforeTrigger() / 1000 + 1;
      myDisplay.SetNewValue(numToDisplay);
    }
    if (flashTimer.IsElapse() && !intervalometer.IsElapse()) {
      myDisplay.ToggleDisplayState();
    }
  }
}

