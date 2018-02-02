
#include "RemoteForIntervalometer.h"

RemoteForIntervalometer::RemoteForIntervalometer(uint8_t irPin, Timer* pIntervalTimer, Timer* pDisplayTimeout, Timer* pFlashTimer, DisplayForIntervalometer* pDisplay, void (*callTakePicture)() )
{
	intervalTimer = pIntervalTimer;
	displayTimeout = pDisplayTimeout;
	flashTimer = pFlashTimer;
	display = pDisplay;
	this->callTakePicture = callTakePicture;
	irDetect = new IRrecv(irPin);
	irDetect->enableIRIn(); // Start the Receiver
}

bool RemoteForIntervalometer::ProcessRemoteInput()
{
	if (displayTimeout->IsElapse() && displayToogleStatus) {
		display->TurnDisplayOff();
	}
	if (GetCurrentMode() == RemoteForIntervalometer::paused || GetInputError()) {
		if (flashTimer->IsElapse()) {
			display->ToggleDisplayState();
		}
	}
	if (irDetect->decode(&irIn)) {
		unsigned long irValue = irIn.value;
		Serial.println(irValue, HEX);
		irDetect->resume(); // Receive the next value
		remoteActions lastRemoteAction = GetAction(irValue);
		if (lastRemoteAction > RemoteForIntervalometer::none &&  lastRemoteAction < RemoteForIntervalometer::endMarker) {
			// Control display state timer - Reset it on a key press
			if (lastRemoteAction != RemoteForIntervalometer::toggleDiplay) {
				display->TurnDisplayOn();
			}
			displayTimeout->Restart();
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
			display->ChangeMode(DisplayForIntervalometer::input);
			display->SetNewValue(numToDisplay);
		} else {
			switch (lastRemoteAction) {
				case RemoteForIntervalometer::start:
				case RemoteForIntervalometer::startFast:
					// Set snapshoot interval
					if ((currentMode == input && numToDisplay > 0) || currentMode == paused) {
						displayTimeout->Restart();
						if (currentMode == input) {   
							if (lastRemoteAction == RemoteForIntervalometer::startFast) {
								if (numToDisplay % 100 > 59) {
									inputError = true;
									break;
								} else {
								  interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
								}
							} else {
								interval = numToDisplay * 10;
							}
							(*callTakePicture)();
							numToDisplay = 1;
							intervalTimer->StartAuto(interval);
							display->ChangeMode(DisplayForIntervalometer::count);
							display->SetNewValue(numToDisplay);
						} 
						currentMode = running;
						display->ChangeMode(DisplayForIntervalometer::count);
					}
					break;
				case RemoteForIntervalometer::stop:
					if (currentMode == running || currentMode == paused || currentMode == instant) {
						currentMode = input;
						display->ChangeMode(DisplayForIntervalometer::input);
						numToDisplay = 0;
						display->ChangeMode(DisplayForIntervalometer::input);
						display->SetNewValue(numToDisplay);
					}
					displayTimeout->Stop();
					break;
				case RemoteForIntervalometer::pause:
					if (currentMode == running) {
						currentMode = paused;
						flashTimer->Restart();
					}
					break;
				case RemoteForIntervalometer::brightDown:
					display->DecreaseBrighness();
					break;
				case RemoteForIntervalometer::brightUp:
					display->IncreaseBrighness();
					break;
				case RemoteForIntervalometer::toggleDiplay:
					display->ToggleDisplayState();
					break;
				case RemoteForIntervalometer::disableDisplayToggle:
					displayToogleStatus = false;
					break;
				case RemoteForIntervalometer::enableDisplayToggle:
					displayToogleStatus = true;
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
					display->SetNewValue(numToDisplay);
					inputError = false;
					break;
					display->SetNewValue(numToDisplay);
				case RemoteForIntervalometer::resetDisplay:
					if (currentMode != input) {
						break;
					}
					numToDisplay = 0;
					inputError = false;
					display->SetNewValue(numToDisplay);
					break;
				case RemoteForIntervalometer::takePicture:
					displayTimeout->Stop();
					if (currentMode == input) {
						currentMode = instant;
						display->ChangeMode(DisplayForIntervalometer::time);
						intervalTimer->Stop();
						previousTime = numToDisplay;
						if (numToDisplay > 0) {
							interval = (((numToDisplay / 100) * 60) + numToDisplay % 100) * 1000;
							flashTimer->Restart();
							intervalTimer->StartAuto(interval);
							display->ChangeMode(DisplayForIntervalometer::time);
							display->SetNewValue(numToDisplay);
						}
					}
					break;
				case RemoteForIntervalometer::none:
					return false;
					break;
			}
		}
		return true;
	}
	return false;
}

RemoteForIntervalometer::mode RemoteForIntervalometer::GetCurrentMode()
{
	return currentMode;
}
void RemoteForIntervalometer::SetNewMode(RemoteForIntervalometer::mode newMode)
{
	currentMode = newMode;
}
bool RemoteForIntervalometer::GetInputError()
{
	return inputError;
}
void RemoteForIntervalometer::ResetToPreviousTime()
{
	numToDisplay = previousTime;
	currentMode = input;
	display->ChangeMode(DisplayForIntervalometer::input);
	display->SetNewValue(numToDisplay);
	display->TurnDisplayOn();
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
	if (IsCommand(RED, irValue)) {
		return disableDisplayToggle;
	}
	if (IsCommand(GREEN, irValue)) {
		return enableDisplayToggle;
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

