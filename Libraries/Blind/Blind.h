#include <Arduino.h>
#include <Stepper.h>
#include <EEPROM.h>

class Blind
{
#define FULL_OPEN 2048
private:
	Stepper* stepper;
	unsigned int currentPosition = 0xFFFF;
	unsigned int fullyOpened;

	unsigned int PositionToSteps(unsigned int position);
	unsigned int StepsToPosition(unsigned int steps);
	
public:
	Blind(Stepper* stepper);
	void Move(int steps);
	void MoveTo(unsigned int value);
	unsigned int Blind::GetCurrentPosition();
	void SetUpperLimit();
	void ResetUpperLimit();
};
