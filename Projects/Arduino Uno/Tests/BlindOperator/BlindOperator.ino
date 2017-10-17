#include <Stepper.h>
#include <EEPROM.h>
#include <Blind.h>
#include <Timer.h>
#include <RemoteForBlinds.h>

#define DELAY 100
#define STEPS_PER_MOTOR_REVOLUTION 32   
#define IR_PIN 13
//
//
#define REPEAT  0xFFFFFFFF
//
Stepper blindMotor(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);
Blind myBlind(&blindMotor);
RemoteForBlinds myRemote(IR_PIN);
Timer remoteInactiveTimer;

void setup()
{

  Serial.begin(9600);
  blindMotor.setSpeed(700);   
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
  switch (myRemote.GetAction()) {
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
      delay(DELAY);
      break;
  }
}

