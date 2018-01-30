#include <DisplayForIntervalometer.h>

DisplayForIntervalometer::DisplayForIntervalometer()
{
	
}

DisplayForIntervalometer::DisplayForIntervalometer(uint8_t pinClock, uint8_t pinIO)
{
	Serial.begin(9600);
	display = new TM1637Display(pinClock, pinIO);
	display->setBrightness(brightness, displayState);  
	DisplayValue();
}

void DisplayForIntervalometer::ToggleDisplayState()
{
	displayState = !displayState;
	Serial.print("In toggle DisplayState: ");
	Serial.println(displayState);
	display->setBrightness(brightness, displayState);  
	DisplayValue();
}

void DisplayForIntervalometer::TurnDisplayOff()
{
	if (displayState == true) {
		Serial.println("Turn display off ");
		displayState = false;
		display->setBrightness(brightness, displayState);  
		DisplayValue();
	}
}

void DisplayForIntervalometer::TurnDisplayOn()
{
	if (displayState == false) {
		Serial.println("Turn display on ");
		displayState = true;
		display->setBrightness(brightness, displayState);  
		DisplayValue();
	}
}

void DisplayForIntervalometer::SetNewValue(int newValue)
{
	if(newValue != value) {
		Serial.print("New value: ");
		Serial.println(newValue);
		value = newValue;
		DisplayValue();
	}
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
		Serial.print("New Mode: ");
		Serial.println(newMode);
		theMode = newMode;
		TurnDisplayOn();
		DisplayValue();
	}
}

void DisplayForIntervalometer::DisplayValue()
{
	
	Serial.print("   In DisplayValue: ");
	Serial.println(value);
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
