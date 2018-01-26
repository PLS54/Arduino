#include "Blind.h"

Blind::Blind(Stepper* stepper)
{
	Serial.begin(9600);
	this->stepper = stepper;
	if (EEPROM.read(0) != 0xBE) {
		EEPROM.write(0, 0xBE);
		EEPROM.write(1, 0);
		EEPROM.write(2, 0);
	}
	if (EEPROM.read(3) != 0xBE) {
		EEPROM.write(3, 0xBE);
		EEPROM.write(4, (FULL_OPEN & 0xFF00) << 8);
		EEPROM.write(5, FULL_OPEN & 0xFF);
	}
	currentPosition = (EEPROM.read(1) << 8) + EEPROM.read(2);
	fullyOpened = (EEPROM.read(4) << 8) + EEPROM.read(5);
	Serial.print("Fully Open steps: ");
	Serial.println(fullyOpened, DEC);
}

void Blind::Move(int steps)
{
	if(currentPosition == 0xFFFF) {
	  return;
	}
	if (((int) (currentPosition + steps)) <= 0) {
		MoveTo(0); 
		return;
	} else if ((currentPosition + steps) >= fullyOpened) {
		MoveTo(100); 
		return;
	}
	stepper->step(steps);
	currentPosition += steps;
    EEPROM.write(1, (currentPosition & 0xFF00) >> 8);
    EEPROM.write(2, currentPosition & 0xFF);
}

void Blind::MoveTo(unsigned int value)
{
	if(currentPosition == 0xFFFF) {
	  return;
	}
	int absValue = PositionToSteps(value);
	unsigned int stepsToTake = absValue - currentPosition;
	stepper->step(stepsToTake);
	currentPosition = absValue;
    EEPROM.write(1, (currentPosition & 0xFF00) >> 8);
    EEPROM.write(2, currentPosition & 0xFF);
}

unsigned int Blind::GetCurrentPosition()
{
	return StepsToPosition(currentPosition);
}

void Blind::SetUpperLimit()
{
	fullyOpened = currentPosition;
	EEPROM.write(4, (fullyOpened & 0xFF00) >> 8);
	EEPROM.write(5, fullyOpened & 0xFF);
}

void Blind::ResetUpperLimit()
{
	fullyOpened = FULL_OPEN;
	EEPROM.write(4, (fullyOpened & 0xFF00) >> 8);
	EEPROM.write(5, fullyOpened & 0xFF);
}

unsigned int Blind::PositionToSteps(unsigned int position)
{
		return ((long) position * fullyOpened) / 100;
}
unsigned int Blind::StepsToPosition(unsigned int steps)
{
	return ((long) steps * 100) / fullyOpened;
}
