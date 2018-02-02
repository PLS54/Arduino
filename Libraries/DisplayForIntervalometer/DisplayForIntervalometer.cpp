#include <DisplayForIntervalometer.h>

DisplayForIntervalometer::DisplayForIntervalometer()
{
	
}

DisplayForIntervalometer::DisplayForIntervalometer(uint8_t pinClock, uint8_t pinIO)
{
	display = new TM1637Display(pinClock, pinIO);
	display->setBrightness(brightness, displayState);  
	DisplayValue();
}

void DisplayForIntervalometer::ToggleDisplayState()
{
	displayState = !displayState;
	display->setBrightness(brightness, displayState);  
	DisplayValue();
}

void DisplayForIntervalometer::TurnDisplayOff()
{
	if (displayState == true) {
		displayState = false;
		display->setBrightness(brightness, displayState);  
		DisplayValue();
	}
}

void DisplayForIntervalometer::TurnDisplayOn()
{
	if (displayState == false) {
		displayState = true;
		display->setBrightness(brightness, displayState);  
		DisplayValue();
	}
}

void DisplayForIntervalometer::SetNewValue(int newValue)
{
	if(newValue != value) {
		value = newValue;
		DisplayValue();
	}
}

int DisplayForIntervalometer::GetCurrentValue()
{
	return value;
}

void DisplayForIntervalometer::IncreaseBrighness()
{
	if (brightness < 7) {
		brightness++;
		display->setBrightness(brightness, displayState);  
		DisplayValue();
	}
}

void DisplayForIntervalometer::DecreaseBrighness()
{
	if (brightness > 0) {
		brightness--;
		display->setBrightness(brightness, displayState);  
		DisplayValue();
	}
}

void DisplayForIntervalometer::ChangeMode(mode newMode)
{
	if( newMode != theMode) {
		theMode = newMode;
		TurnDisplayOn();
		DisplayValue();
	}
}

void DisplayForIntervalometer::DisplayValue()
{
	
	if (!(value == 0 && theMode == input)) {
		int toDisplay = value;
		if (theMode == time) {
			toDisplay = ((value / 60) * 100) + (value % 60); // convert second to mm:ss
		}
		display->showNumberDecEx(toDisplay, (theMode != count) ? 0xFF : 0x00, theMode != count);
	} else {
		// Set display to ----
		uint8_t data[] = {0x40, 0x40, 0x40, 0x40};
		display->setSegments(data);
	}
}
