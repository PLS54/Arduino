#include "Blind.h"

Blind::Blind(Stepper* stepper, unsigned int startPos)
{
  currentPosition = startPos;
  this->stepper = stepper;
}

unsigned int Blind::GetCurrentPosition()
{
  return ((long) currentPosition * 100) / FULL_OPEN;
}

void Blind::Move(int steps)
{
  if (((int) (currentPosition + steps)) <= 0) {
    MoveTo(0); 
    return;
  } else if ((currentPosition + steps) >= FULL_OPEN) {
    Serial.println("going to 100: ");
    MoveTo(100); 
    return;
  }
  stepper->step(steps);
  currentPosition += steps;
}

void Blind::MoveTo(unsigned int value)
{
  int absValue = ((long) value * FULL_OPEN) / 100;
  unsigned int stepsToTake = absValue - currentPosition;
  stepper->step(stepsToTake);
  currentPosition = absValue;
}

