#include "Blind.h"

Blind::Blind(Stepper* stepper)
{
  this->stepper = stepper;
}

void Blind::Move(int steps)
{
	if(currentPosition == 0xFFFF) {
	  return;
	}
	if (((int) (currentPosition + steps)) <= 0) {
		MoveTo(0); 
		return;
	} else if ((currentPosition + steps) >= FULL_OPEN) {
		MoveTo(100); 
		return;
	}
	stepper->step(steps);
	currentPosition += steps;
    EEPROM.write(1, GetCurrentPosition());
}

void Blind::MoveTo(unsigned int value)
{
	if(currentPosition == 0xFFFF) {
	  return;
	}
	int absValue = ((long) value * FULL_OPEN) / 100;
	unsigned int stepsToTake = absValue - currentPosition;
	stepper->step(stepsToTake);
	currentPosition = absValue;
	EEPROM.write(1, GetCurrentPosition());
}

void Blind::SetPosition(unsigned int position)
{
	currentPosition = ((long) position * FULL_OPEN) / 100;
    EEPROM.write(1, GetCurrentPosition());
}

unsigned int Blind::GetCurrentPosition()
{
	return ((long) currentPosition * 100) / FULL_OPEN;
}

