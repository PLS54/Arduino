/*
 *  intervalTimer by PLS Version 1.7
 */
#include <Timer.h>
#include <RemoteForIntervalometer.h>
#include <DisplayForIntervalometer.h>

#define DIO               8    //Set the DIO pin connection to the display
#define CLK               9   //Set the CLK pin connection to the display
#define RELAY_PIN         10
#define IR_PIN            11
#define LOOP_DELAY        1
#define DISPLAY_TIMEOUT   10000
#define FLASHES           250

Timer* intervalTimer;
Timer* displayTimeout;
Timer* flashTimer(500);
DisplayForIntervalometer* myDisplay;
RemoteForIntervalometer* myRemote;

void takePicture()
{
  digitalWrite(RELAY_PIN, HIGH);
  delayMicroseconds(100000);  //
  delay(100);  
  digitalWrite(RELAY_PIN, LOW); 
}

void setup()
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  intervalTimer = new Timer();
  displayTimeout = new Timer();
  displayTimeout->Start(DISPLAY_TIMEOUT);
  flashTimer = new Timer(FLASHES);
  myDisplay = new DisplayForIntervalometer(CLK, DIO);
  myRemote = new RemoteForIntervalometer(IR_PIN, intervalTimer, displayTimeout, flashTimer, myDisplay, takePicture);
  //
  // Even if this is not used here it must be done for the class RemoteForIntervalometer to work wirh IR receiver
  //
  IRrecv irDetect(IR_PIN);
  irDetect.enableIRIn(); // Start the Receiver
}

void loop()
{
  if (!myRemote->ProcessRemoteInput()) {
      delay(LOOP_DELAY);
  }
  if (myRemote->GetCurrentMode() == RemoteForIntervalometer::paused) {
    intervalTimer->IsElapse(); // pump the interval timer
  }
  if (myRemote->GetCurrentMode() == RemoteForIntervalometer::running && intervalTimer->IsElapse()) {
    // Snap picture when timer elapse
    takePicture();
    myDisplay->SetNewValue(myDisplay->GetCurrentValue() + 1);
  } else if (myRemote->GetCurrentMode() == RemoteForIntervalometer::instant) {
    ProcessIntantMode();
  } else {
    if (myRemote->GetCurrentMode() != RemoteForIntervalometer::running) {
      delay(LOOP_DELAY);
    }
  }
}


void ProcessIntantMode()
{
  if (!intervalTimer->Running()) {
    myDisplay->ToggleDisplayState();
    Serial.println("Not Here");
    takePicture();
    myRemote->ResetToPreviousTime();
  } else {
    bool timerState = intervalTimer->IsElapse();
    Serial.print("Timer state: ");
    Serial.println(timerState);
    if (timerState) {
      Serial.println("HERE");
      myDisplay->TurnDisplayOn();
      myDisplay->SetNewValue(0);
      takePicture();  
      myRemote->ResetToPreviousTime();
      //intervalTimer->Stop();
    } else {
      int numToDisplay = intervalTimer->GetTimeLeftBeforeTrigger() / 1000 + 1;
      myDisplay->SetNewValue(numToDisplay);
    }
    if (flashTimer->IsElapse() && !timerState) {
      myDisplay->ToggleDisplayState();
    }
  }
}

