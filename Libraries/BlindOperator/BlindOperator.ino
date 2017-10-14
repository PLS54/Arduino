#include <IRremote.h>
#include <Stepper.h>
#include "Blind.h"

#define DOOR_PIN 9
#define DELAY 250
#define STEPS_PER_MOTOR_REVOLUTION 32   
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048  
#define IR_PIN 13

Stepper blindMotor(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);
Blind myBlind(&blindMotor, 400);
IRrecv irDetect(IR_PIN);
decode_results irIn;
int direction = 1;
unsigned long lastCommand = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(DOOR_PIN, OUTPUT);
  irDetect.enableIRIn(); // Start the Receiver
  blindMotor.setSpeed(700);   
}

void loop() {
  if (irDetect.decode(&irIn)) {
    Serial.println(irIn.value, HEX);
    irDetect.resume(); // Receive the next value
    if (irIn.value != 0xFFFFFFFF) {
      lastCommand = irIn.value;
    }
    if (irIn.value == 0x37810F) {
      blindMotor.step(-1);
      Serial.println("starting reset");
    } 
    if (irIn.value == 0x37A10B) {
      myBlind.Move(-10);
      Serial.println("starting move");
    }
    if (irIn.value == 0x36812F) {
      myBlind.Move(10);
      Serial.println("starting move");
    }
    if (irIn.value == 0xFFFFFFFF && lastCommand == 0x37810F) {
      blindMotor.step(-1);
      //Serial.println("repeat");
    }
    if (irIn.value == 0xFFFFFFFF && lastCommand == 0x37A10B) {
      myBlind.Move(-10);
      //Serial.println("repeat");
    }
    if (irIn.value == 0xFFFFFFFF && lastCommand == 0x36812F) {
      myBlind.Move(10);
      //Serial.println("repeat");
    }
    if (lastCommand == 0) {
      Serial.println(irIn.value, HEX);
    }
    if (irIn.value == 0x37E103) {
      direction = 1;
    }
    if (irIn.value == 0x36B928) {
      direction = -1;
    }

    if (irIn.value == 0x36113D) {
      myBlind.MoveTo(0);
    }
    if (irIn.value == 0x37111D) {
      myBlind.MoveTo(50);
    }
    if (irIn.value == 0x36912D) {
      myBlind.MoveTo(100);
    }
    if (irIn.value == 0x37E902) {
      unsigned int pos = myBlind.GetCurrentPosition();
      Serial.print("Position: ");
      Serial.println(pos);
    }
  }
  if (lastCommand == 0) {
    delay(100);
  }
}

