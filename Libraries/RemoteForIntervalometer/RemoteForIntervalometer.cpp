
#include "RemoteForIntervalometer.h"

RemoteForIntervalometer::RemoteForIntervalometer(uint8_t irPin, Timer* pIntervalTimer, Timer* pFlashTimer, DisplayForIntervalometer* pDisplay, void (*callTakePicture)() )
{
	intervalTimer = pIntervalTimer;
	displayTimeout.Start(DISPLAY_TIMEOUT);
	flashTimer = pFlashTimer;
	display = pDisplay;
	this->callTakePicture = callTakePicture;
	irDetect = new IRrecv(irPin);
	irDetect->enableIRIn(); // Start the Receiver
}

bool RemoteForIntervalometer::ProcessRemoteInput()
{
	if (displayTimeout.IsElapse() && displayToogleStatus) {
		display->TurnDisplayOff();
	}
	if (GetCurrentMode() == RemoteForIntervalometer::paused || GetInputError()) {
		if (flashTimer->IsElapse()) {
			display->ToggleDisplayState();
		}
	}
	if (irDetect->decode(&irIn)) {
		unsigned long irValue = irIn.value;
		irDetect->resume(); // Receive the next value
		remoteActions lastRemoteAction = GetAction(irValue);
		if (lastRemoteAction > RemoteForIntervalometer::none &&  lastRemoteAction < RemoteForIntervalometer::endMarker) {
			//
			// Control display state timer - Reset it on a key press
			//
			if (lastRemoteAction != RemoteForIntervalometer::toggleDiplay) {
				display->TurnDisplayOn();
			}
			displayTimeout.Restart();
		}
		if (lastRemoteAction >= RemoteForIntervalometer::one && lastRemoteAction <= RemoteForIntervalometer::zero && currentMode == input ) {
			//
			// Process digit in input mode
			//
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
					//
					// Set snapshoot interval
					//
					if ((currentMode == input && numToDisplay > 0) || currentMode == paused) {
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
					if (displayToogleStatus) {
						FlashDisplay();
					}
					displayToogleStatus = false;
					break;
				case RemoteForIntervalometer::enableDisplayToggle:
					if (!displayToogleStatus) {
						FlashDisplay();
					}
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
	static lookuptab tab[] = {
		{0x0200B, 0x37990C, start , "Start"},
		{0x0400B, 0x374117, pause, "Pause"},
		{0x0000B, 0x365934, stop, "Stop"},
		{0x2C108, 0x36293A, startFast, "Start fast"},
		{0x9E108, 0x37A10B, brightDown, "Brighness down"},
		{0x1E108, 0x36812F, brightUp, "Up arrow"},
		{0xE010C, 0x37E902, deleteLastChar, "Yellow"},
		{0x2010C, 0x36193C, resetDisplay, "Blue"},
		{0xA010C, 0x37191C, disableDisplayToggle, "Red"},
		{0x6010C, 0x0F7E10, enableDisplayToggle, "Green"},
		{0x3E108, 0x366133, takePicture, "Enter"},
		{0x0140A, 0x36F920, toggleDiplay, "Muting"},
		{0x00108, 0x36113D, one, "One"},
		{0x80108, 0x37111D, two, "Two"},
		{0x40108, 0x36912D, three, "Three"},
		{0xC0108, 0x37910D, four, "Four"},
		{0x20108, 0x365135, five, "Five"},
		{0xA0108, 0x375115, six, "Six"},
		{0x60108, 0x36D125, seven, "Seven"},
		{0xE0108, 0x37D105, eight, "Eight"},
		{0x10108, 0x363139, nine, "Nine"},
		{0x90108, 0x373119, zero, "Zero"}
	};

    if (irValue != REPEAT) {
      lastCommand = irValue;
    }
	if (irValue == SONY_ON){
		FlashDisplay();
		displayTimeout.Restart();
		display->TurnDisplayOn();
		sonyRemoteOn = !sonyRemoteOn;
	}
	if (irValue == VIDEOTRON_ON){
		FlashDisplay();
		displayTimeout.Restart();
		display->TurnDisplayOn();
		videotronRemoteOn = !videotronRemoteOn;
	}
	for (int i = 0; i < sizeof(tab) / sizeof(*tab); i++) {
		if ((sonyRemoteOn && (irValue == tab[i].sonyCode)) || (videotronRemoteOn && (irValue == tab[i].videotronCode))) {
			return tab[i].action;
		}
	}
	return none;
}

void RemoteForIntervalometer::FlashDisplay()
{
	if (display->GetCurrentState()) {
		flashTimer->Restart();
		display->TurnDisplayOff();
		do {} while (!flashTimer->IsElapse());
		display->TurnDisplayOn();
	}
}