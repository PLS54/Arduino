#include "RemoteForIntervalometer.h"

RemoteForIntervalometer::RemoteForIntervalometer(uint8_t irPin, Timer& intervalTimer, Timer& displayTimeout, DisplayForIntervalometer& display)
{
	this->intervalTimer = intervalTimer;
	this->displayTimeout = displayTimeout;
	this->display = display;
	irDetect = new IRrecv(irPin);
	irDetect->enableIRIn(); // Start the Receiver
}

void RemoteForIntervalometer::ProcessRemoteInput()
{
	if (irDetect->decode(&irIn)) {
		unsigned long irValue = irIn.value;
		irDetect->resume(); // Receive the next value
		remoteActions lastRemoteAction = GetAction(1);
		if (lastRemoteAction > RemoteForIntervalometer::none &&  lastRemoteAction < RemoteForIntervalometer::endMarker) {
			// Control display state timer - Reset it on a key press
			if (lastRemoteAction != RemoteForIntervalometer::toggleDiplay) {
				display.TurnDisplayOn();
			}
			displayTimeout.Restart();
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
			display.ChangeMode(DisplayForIntervalometer::input);
			display.SetNewValue(numToDisplay);
		} else {
		switch (lastRemoteAction) {
			case RemoteForIntervalometer::start:
			case RemoteForIntervalometer::startFast:
				// Set snapshoot interval
				if ((currentMode == input && numToDisplay > 0) || currentMode == paused) {
					displayTimeout.Start(60000);
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
						//**takePicture();
						numToDisplay = 1;
						this->intervalTimer.StartAuto(interval);
						display.ChangeMode(DisplayForIntervalometer::count);
						display.SetNewValue(numToDisplay);
					} 
					currentMode = running;
				}
				break;
			case RemoteForIntervalometer::stop:
				if (currentMode == running || currentMode == paused || currentMode == instant) {
					currentMode = input;
					numToDisplay = 0;
					display.ChangeMode(DisplayForIntervalometer::input);
					display.SetNewValue(numToDisplay);
				}
				displayTimeout.Stop();
				break;
			case RemoteForIntervalometer::pause:
				if (currentMode == running) {
					currentMode = paused;
					//**flashTimer.StartAuto(500);
				}
			break;
			case RemoteForIntervalometer::brightDown:
				display.DecreaseBrighness();
				break;
			case RemoteForIntervalometer::brightUp:
				display.IncreaseBrighness();
				break;
			case RemoteForIntervalometer::toggleDiplay:
				display.ToggleDisplayState();
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
				display.SetNewValue(numToDisplay);
				break;
				display.SetNewValue(numToDisplay);
			case RemoteForIntervalometer::resetDisplay:
				if (currentMode != input) {
					break;
				}
				numToDisplay = 0;
				display.SetNewValue(numToDisplay);
				break;
			case RemoteForIntervalometer::takePicture:
				displayTimeout.Stop();
				if (currentMode == input) {
					currentMode = instant;
					intervalTimer.Stop();
					if (numToDisplay > 0) {
						previousDisplay = numToDisplay;
						interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
						//** flashTimer.StartAuto(500);
						intervalTimer.StartAuto(interval - 100);
						display.ChangeMode(DisplayForIntervalometer::time);
						display.SetNewValue(numToDisplay);
					}
				}
				break;
			case RemoteForIntervalometer::none:
				//** delay(DELAY);
				break;
			}
		}
	}
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
	if (IsCommand(ENTER, irValue)) {
		return takePicture;
	}
	return none;
}

bool RemoteForIntervalometer::IsCommand(unsigned long command, unsigned long irValue, bool withRepeat)
{	
	return (irValue == command || (withRepeat && irValue == REPEAT && lastCommand == command));
}

