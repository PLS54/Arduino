#include <Stepper.h>
#include <EEPROM.h>
#include "Blind.h"
#include "Timer.h"
#include <IRremote.h>
#include "RemoteForBlinds.h"

#define DOOR_PIN 9
#define DELAY 250
#define STEPS_PER_MOTOR_REVOLUTION 32   
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64  //2048  
#define IR_PIN 13
//
//
#define REPEAT  0xFFFFFFFF
//
Stepper blindMotor(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);
Blind myBlind(&blindMotor);
RemoteForBlinds myRemote(IR_PIN);
Timer remoteInactiveTimer;

  IRrecv* irDetect;
  decode_results irIn;


void setup()
{

  irDetect = new IRrecv(IR_PIN);
  irDetect->enableIRIn(); // Start the Receiver

  
  Serial.begin(9600);
  pinMode(DOOR_PIN, OUTPUT);
  blindMotor.setSpeed(700);   
  Serial.print("Starting: 0x");
  Serial.println(EEPROM.read(0), HEX);
  if (EEPROM.read(0) != 0xBE) {
    EEPROM.write(0, 0xBE);
    EEPROM.write(1, 0);
  }
  Serial.print("Starting position: ");
  Serial.println(EEPROM.read(1));
  myBlind.SetPosition(EEPROM.read(1));
}

void loop() {
  if (remoteInactiveTimer.IsElapse()) {
    myRemote.Reactivate();
  }
  if (irDetect->decode(&irIn)) {
    unsigned long irValue = irIn.value;
    irDetect->resume(); // Receive the next value
    switch (myRemote.GetAction(irValue)) {
      case RemoteForBlinds::moveTo0:
        myBlind.MoveTo(0);
        break;
      case RemoteForBlinds::moveTo50:
        myBlind.MoveTo(50);
        break;
      case RemoteForBlinds::moveTo100:
        myBlind.MoveTo(100);
        break;
      case RemoteForBlinds::moveDown10:
        myBlind.Move(-10);
        break;
      case RemoteForBlinds::moveUp10:
        myBlind.Move(10);
        break;
      case RemoteForBlinds::adjustDown:
        blindMotor.step(-1);
        break;
      case RemoteForBlinds::adjustUp:
        blindMotor.step(1);
        break;
      case RemoteForBlinds::reportPosition:
        Serial.print("Position: ");
        Serial.println(myBlind.GetCurrentPosition());
        break;
      case RemoteForBlinds::remoteDeactivated:
        remoteInactiveTimer.Start(10000);
        break;
      case RemoteForBlinds::none:
        delay(100);
        break;
    }
  }
}

