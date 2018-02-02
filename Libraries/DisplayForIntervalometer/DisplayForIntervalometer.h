#ifndef DisplayForIntervalometer_h
#define DisplayForIntervalometer_h

#include <Arduino.h>
#include <TM1637Display.h>

class DisplayForIntervalometer
{
	public:

		enum mode {input, time, count};
		DisplayForIntervalometer();
		DisplayForIntervalometer(uint8_t pinClock, uint8_t pinIO);

		void ToggleDisplayState();
		void TurnDisplayOff();
		void TurnDisplayOn();
		void SetNewValue(int newValue);
		int GetCurrentValue();
		void IncreaseBrighness();
		void DecreaseBrighness();
		void ChangeMode(mode newMode);

	private:
		TM1637Display *display = NULL;
			
		int value = 0;
		mode theMode = input;
		bool displayState = true;
		uint8_t brightness = 0;
	
		void DisplayValue();
};
#endif