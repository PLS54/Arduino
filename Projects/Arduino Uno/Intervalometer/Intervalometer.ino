/*
 *  intervalTimer by PLS Version 1.7
 */
#include <Timer.h>
#include <RemoteForIntervalometer.h>
#include <DisplayForIntervalometer.h>

#define DIO         8    //Set the DIO pin connection to the display
#define CLK         9   //Set the CLK pin connection to the display
#define RELAY_PIN   10
#define IR_PIN      11

#define DELAY 100


Timer intervalTimer;
Timer displayTimeout;
Timer flashTimer(500);
DisplayForIntervalometer myDisplay(CLK, DIO);
RemoteForIntervalometer myRemote(IR_PIN, intervalTimer, displayTimeout, myDisplay);



void setup()
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 
  displayTimeout.Start(60000);
}


void loop()
{
 
    myRemote.ProcessRemoteInput();
/*  if (displayTimeout.IsElapse()) {
    //myDisplay.TurnDisplayOff();
  }
  if (currentMode == paused || inputError) {
    if (flashTimer.IsElapse()) {
      myDisplay.ToggleDisplayState();
    }
  }
  if (currentMode == running && intervalTimer.IsElapse()) {
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
    myDisplay.ChangeMode(DisplayForintervalTimer::input);
  }
  if (currentMode == instant) {
    myDisplay.ChangeMode(DisplayForintervalTimer::time);
  }
  if (currentMode == running) {
    myDisplay.ChangeMode(DisplayForintervalTimer::count);
  }
*/
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
  if (!intervalTimer.Running()) {
    myDisplay.ToggleDisplayState();
    takePicture();
    myDisplay.ToggleDisplayState();
    //**currentMode = input;
  } else {
    if (intervalTimer.IsElapse()) {
      myDisplay.TurnDisplayOn();
      myDisplay.SetNewValue(0);
      delay(100);
      takePicture();  
      //**currentMode = input;
//**      numToDisplay = previousDisplay;
//**      myDisplay.SetNewValue(numToDisplay);
    } else {
      int numToDisplay = intervalTimer.GetTimeLeftBeforeTrigger() / 1000 + 1;
      myDisplay.SetNewValue(numToDisplay);
    }
    if (flashTimer.IsElapse() && !intervalTimer.IsElapse()) {
      myDisplay.ToggleDisplayState();
    }
  }
}

