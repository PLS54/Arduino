#include "RemoteForIntervalometer.h"
RemoteForIntervalometer::RemoteForIntervalometer(int i)
{
	
}

RemoteForIntervalometer::remoteActions RemoteForIntervalometer::GetAction(unsigned long irValue)
{
    if (irValue != REPEAT) {
      lastCommand = irValue;
    }
	if (IsCommand(ONE, irValue)) {
		return one;
	}
	if (IsCommand(TWO, irValue)) {
		return two;
	}
	if (IsCommand(THREE, irValue)) {
		return three;
	}
	if (IsCommand(FOUR, irValue)) {
		return four;
	}
	if (IsCommand(FIVE, irValue)) {
		return five;
	}
	if (IsCommand(SIX, irValue)) {
		return six;
	}
	if (IsCommand(SEVEN, irValue)) {
		return seven;
	}
	if (IsCommand(EIGHT, irValue)) {
		return eight;
	}
	if (IsCommand(NINE, irValue)) {
		return nine;
	}
	if (IsCommand(ZERO, irValue)) {
		return zero;
	}
	if (IsCommand(BLUE, irValue)) {
		return resetDisplay;
	}
	if (IsCommand(YELLOW, irValue)) {
		return deleteLastChar;
	}
	if (IsCommand(PLAY, irValue)) {
		return start;
	}
	if (IsCommand(STOP, irValue)) {
		return stop;
	}
	if (IsCommand(FAST_FORWARD, irValue)) {
		return startFast;
	}
	if (IsCommand(PAUSE, irValue)) {
		return pause;
	}
	if (IsCommand(DOWN_ARROW, irValue, true)) {
		return brightDown;
	}
	if (IsCommand(UP_ARROW, irValue, true)) {
		return brightUp;
	}
	if (IsCommand(MUTING, irValue)) {
		return toggleDiplay;
	}
	return none;
}

bool RemoteForIntervalometer::IsCommand(unsigned long command, unsigned long irValue, bool withRepeat)
{	
	return (irValue == command || (withRepeat && irValue == REPEAT && lastCommand == command));
}

