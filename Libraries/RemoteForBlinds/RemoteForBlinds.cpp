#include "RemoteForBlinds.h"
RemoteForBlinds::RemoteForBlinds(unsigned int irPin)
{
//	irDetect = new IRrecv(irPin);
//	irDetect->enableIRIn(); // Start the Receiver

}

RemoteForBlinds::blindActions RemoteForBlinds::GetAction(unsigned long irValue)
{
  if (true) {
//  if (irDetect->decode(&irIn)) {
//	unsigned int irValue = irIn.value;
//    Serial.println(irValue, HEX);
 //   irDetect->resume(); // Receive the next value
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
      if (irValue == ADJUST_DOWN || (irValue == REPEAT && lastCommand == ADJUST_DOWN)) {
		  return adjustDown;
      }
      if (irValue == ADJUST_UP || (irValue == REPEAT && lastCommand == ADJUST_UP)) {
		  return adjustUp;
      }
      if (irValue == MOVE_DOWN_10 || (irValue == REPEAT && lastCommand == MOVE_DOWN_10)) {
		  return moveDown10;
      }
      if (irValue == MOVE_UP_10 || (irValue == REPEAT && lastCommand == MOVE_UP_10)) {
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