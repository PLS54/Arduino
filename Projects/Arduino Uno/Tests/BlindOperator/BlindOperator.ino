#include <IRremote.h>
#include <Stepper.h>
#include "Blind.h"
#include "Timer.h"

#define DOOR_PIN 9
#define DELAY 250
#define STEPS_PER_MOTOR_REVOLUTION 32   
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048  
#define IR_PIN 13
//
#define FAST_BACKWARD   0xCC108
#define PLAY            0x200B
#define FAST_FORWARD    0x2C108
#define DOWN_ARROW      0x9E108
#define UP_ARROW        0x1E108
#define LEFT_ARROW      0x5E108
#define RIGHT_ARROW     0xDE108
#define RETURN          0xBE108
#define YELLOW          0xE010C
#define BLUE            0x2010C
#define ENTER           0x3E108

#define MOVE_TO_0       FAST_BACKWARD
#define MOVE_TO_50      PLAY
#define MOVE_TO_100     FAST_FORWARD
#define MOVE_DOWN_10    DOWN_ARROW
#define MOVE_UP_10      UP_ARROW
#define ADJUST_DOWN     LEFT_ARROW
#define ADJUST_UP       RIGHT_ARROW
#define REPORT_POSITION RETURN
//
#define REPEAT  0xFFFFFFFF
//
Stepper blindMotor(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);
Blind myBlind(&blindMotor, 400);
IRrecv irDetect(IR_PIN);
decode_results irIn;
Timer remoteInactive;
unsigned long lastCommand = 0;
bool remoteActive = true;

void setup()
{
  Serial.begin(9600);
  pinMode(DOOR_PIN, OUTPUT);
  irDetect.enableIRIn(); // Start the Receiver
  blindMotor.setSpeed(700);   
}

void loop() {
  if (irDetect.decode(&irIn)) {
//    Serial.println(irIn.value, HEX);
    irDetect.resume(); // Receive the next value
    if (remoteInactive.IsElapse()) {
      remoteActive = true;
    }
    if (irIn.value == ENTER && lastCommand == YELLOW) {
      remoteActive = true;
    }
    if (irIn.value == ENTER && lastCommand == BLUE) {
      remoteInactive.Start(10000);
      remoteActive = false;
    }
    if (irIn.value != REPEAT) {
      lastCommand = irIn.value;
    }
    if (remoteActive) {
      if (irIn.value == ADJUST_DOWN || (irIn.value == REPEAT && lastCommand == ADJUST_DOWN)) {
        blindMotor.step(-1);
      }
      if (irIn.value == ADJUST_UP || (irIn.value == REPEAT && lastCommand == ADJUST_UP)) {
        blindMotor.step(1);
      }
      if (irIn.value == MOVE_DOWN_10 || (irIn.value == REPEAT && lastCommand == MOVE_DOWN_10)) {
        myBlind.Move(-10);
      }
      if (irIn.value == MOVE_UP_10 || (irIn.value == REPEAT && lastCommand == MOVE_UP_10)) {
        myBlind.Move(10);
      }
      if (irIn.value == MOVE_TO_0) {
        myBlind.MoveTo(0);
      }
      if (irIn.value == PLAY) {
        myBlind.MoveTo(50);
      }
      if (irIn.value == MOVE_TO_100) {
        myBlind.MoveTo(100);
      }
    }
    if (irIn.value == REPORT_POSITION) {
      unsigned int pos = myBlind.GetCurrentPosition();
      Serial.print("Position: ");
      Serial.println(pos);
    }
  }
  if (lastCommand == 0) {
    delay(100);
  }
}

