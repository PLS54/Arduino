#include "RemoteForBlinds.h"
RemoteForBlinds::RemoteForBlinds(unsigned int irPin)
{
	irDetect = new IRrecv(irPin);
	irDetect->enableIRIn(); // Start the Receiver
}

RemoteForBlinds::blindActions RemoteForBlinds::GetAction()
{
  if (irDetect->decode(&irIn)) {
	unsigned long irValue = irIn.value;
    Serial.println(irValue, HEX);
	irDetect->resume(); // Receive the next value
    if (irValue == ENTER && lastCommand == YELLOW) {
      remoteActive = true;
	  return remoteActivated;
    }
    if (irValue == ENTER && lastCommand == BLUE) {
      remoteActive = false;
	  return remoteDeactivated;
    }
    if (irValue != REPEAT) {
      lastCommand = irValue;
    }
    if (remoteActive) {
		if (IsCommand(ADJUST_DOWN, irValue)) {
			return adjustDown;
		}
		if (IsCommand(ADJUST_UP, irValue)) {
			return adjustUp;
		}
		if (IsCommand(MOVE_DOWN_10, irValue)) {
			return moveDown10;
		}
		if (IsCommand(MOVE_UP_10, irValue)) {
			return moveUp10;
		}
		if (irValue == MOVE_TO_0) {
		  return moveTo0;
		}
		if (irValue == PLAY) {
		  return moveTo50;
		}
		if (irValue == MOVE_TO_100) {
		  return moveTo100;
		}
    }
    if (irValue == REPORT_POSITION) {
		return reportPosition;
    }
  }
  return none;
}

bool RemoteForBlinds::IsCommand(unsigned long command, unsigned long irValue)
{	
	return (irValue == command || (irValue == REPEAT && lastCommand == command));
}

void RemoteForBlinds::Reactivate()
{
	remoteActive = true;
}