#include <Stepper.h>
#include <EEPROM.h>
#include <Blind.h>
#include <Timer.h>
#include <IRremote.h>
#include <RemoteForBlinds.h>

#define DELAY 100
#define STEPS_PER_MOTOR_REVOLUTION 32   
#define IR_PIN 12





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
  blindMotor.setSpeed(700);   
  Serial.print("Starting position: ");
  Serial.println(myBlind.GetCurrentPosition());
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
      case RemoteForBlinds::setUpperLimit:
        myBlind.SetUpperLimit();
        break;
      case RemoteForBlinds::resetUpperLimit:
        myBlind.ResetUpperLimit();
        break;
      case RemoteForBlinds::none:
        delay(DELAY);
        break;
    }
  }
}

