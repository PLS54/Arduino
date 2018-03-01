#define VERSION "1.9"
/*
 *  intervalTimer by PLS Version
 */
#include <Timer.h>
#include <RemoteForIntervalometer.h>
#include <DisplayForIntervalometer.h>

#define DIO               8    //Set the DIO pin connection to the display
#define CLK               9   //Set the CLK pin connection to the display
#define RELAY_PIN         10
#define IR_PIN            11
#define LOOP_DELAY        1
#define FLASHES           250

Timer* intervalTimer;
Timer* flashTimer(500);
DisplayForIntervalometer* myDisplay;
RemoteForIntervalometer* myRemote;
void takePicture();

void setup()
{
  Serial.begin(9600);
  Serial.printf("Experimental Intervalometer version: %s\n", VERSION);
  delay(1000);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  myDisplay = new DisplayForIntervalometer(CLK, DIO);
  myDisplay->ChangeMode(DisplayForIntervalometer::count);
  myDisplay->SetNewValue(0);

  if (false) {
    intervalTimer = new Timer();
    flashTimer = new Timer(FLASHES);
    myDisplay = new DisplayForIntervalometer(CLK, DIO);
    myRemote = new RemoteForIntervalometer(IR_PIN, intervalTimer, flashTimer, myDisplay, takePicture);
    //
    // Even if this is not used here it must be done for the class RemoteForIntervalometer to work wirh IR receiver
    //
    IRrecv irDetect(IR_PIN);
    irDetect.enableIRIn(); // Start the Receiver
  }  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void loop()
{
  if (true) return;
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
    takePicture();
    myRemote->ResetToPreviousTime();
  } else {
    bool timerState = intervalTimer->IsElapse();
    if (timerState) {
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

void takePicture()
{
  digitalWrite(RELAY_PIN, HIGH);
  delayMicroseconds(100000);  //
  delay(100);  
  digitalWrite(RELAY_PIN, LOW); 
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) 
{
  myDisplay->SetNewValue(  myDisplay->GetCurrentValue() + 1);
} 

