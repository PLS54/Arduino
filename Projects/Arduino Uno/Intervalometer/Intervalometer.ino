/*
 *  Intervalometer by PLS Version 1.7
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
  display.setBrightness(0x0);  //set the diplay to maximum brightness
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
          if (currentMode == running || currentMode == paused || currentMode == instant) {
            currentMode = input;
            numToDisplay = 0;
          }
          break;
        case RemoteForIntervalometer::pause:
          if (currentMode == running) {
            currentMode = paused;
            flashTimer.StartAuto(500);
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
          if (currentMode != input) {
            break;
          }
          if (numToDisplay >= 10) {
            numToDisplay /= 10;
          } else {
            numToDisplay = 0;
          }
          break;
        case RemoteForIntervalometer::resetDisplay:
          if (currentMode != input) {
            break;
          }
          numToDisplay = 0;
          break;
        case RemoteForIntervalometer::takePicture:
          if (currentMode == input) {
            currentMode = instant;
            intervalometer = NULL; 
            if (numToDisplay > 0) {
              previousDisplay = numToDisplay;
              interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
              flashTimer.StartAuto(500);
              intervalometer = new Timer(interval - 100);
              displayState = !displayState;
              display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
              DisplayValue(display, numToDisplay, currentMode);
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
    // Turn display off
    displayState = false;
  }
  display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
  DisplayValue(display, numToDisplay, currentMode);
  if (currentMode == paused || inputError) {
    if (flashTimer.IsElapse()) {
      displayState = !displayState;
      display.setBrightness(bright, displayState);  //set the diplay brightness and state
      delay(100);
    }
  }
  bool timerElapse = false;
  if (intervalometer != NULL) {
    timerElapse = intervalometer->IsElapse();
  }
  if (currentMode == running && timerElapse) {
    // Snap picture when timer elapse
    takePicture();
    numToDisplay++;
  } else if (currentMode == instant) {
    if (intervalometer == NULL) {
      display.setBrightness(bright, !displayState);  //set the diplay to maximum brightness
      takePicture();
      display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
      currentMode = input;
    } else {
      if (timerElapse) {
        displayState = true;
        display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
        DisplayValue(display, 0, currentMode);
        delay(100);
        takePicture();
        currentMode = input;
        numToDisplay = previousDisplay;
        DisplayValue(display, numToDisplay, currentMode);
      } else {
        numToDisplay = intervalometer->GetTimeLeftBeforeTrigger() / 1000 + 1;
        DisplayValue(display, numToDisplay, currentMode);
        display.showNumberDecEx(numToDisplay, ((currentMode == input) || (currentMode== instant)) ? 0xFF : 0x00, true); //Display the Variable value;
      }
      if (flashTimer.IsElapse() && !timerElapse) {
        displayState = !displayState;
        display.setBrightness(bright, displayState);  //set the diplay to maximum brightness
        DisplayValue(display, numToDisplay, currentMode);
      }
    }
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

mode lastMode = initial;
int lastValue = -1;
void DisplayValue(TM1637Display theDisplay, int value, mode currentMode)
{
  if ((lastValue == value) && lastMode == currentMode) {
    return;
  }
  if (!(value == 0 && currentMode == input)) {
    display.showNumberDecEx(value, ((currentMode == input) || (currentMode== instant)) ? 0xFF : 0x00, currentMode != running && currentMode != paused); //Display the Variable value;
  } else {
    // Set display to ----
    uint8_t data[] = {0x40, 0x40, 0x40, 0x40};
    display.setSegments(data);
  }
}

