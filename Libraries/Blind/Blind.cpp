#include "Blind.h"

Blind::Blind(Stepper* stepper, unsigned int startPos)
{
  currentPosition = startPos;
  this->stepper = stepper;
}

unsigned int Blind::GetCurrentPosition()
{
  Serial.print("currentPosition: ");
  Serial.println(currentPosition);
  return ((long) currentPosition * 100) / FULL_OPEN;
}

void Blind::Move(int steps)
{
  Serial.print("Moving: ");
  Serial.println(steps);
  
  Serial.print("Current: ");
  Serial.println(currentPosition);

  if (((int) (currentPosition + steps)) <= 0) {
    Serial.println("going to 0: ");

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
  Serial.print("Abs to: ");
  Serial.println(absValue);
  unsigned int stepsToTake = absValue - currentPosition;
  stepper->step(stepsToTake);
  Serial.print("Moving to: ");
  Serial.println(stepsToTake);
  currentPosition = absValue;
}

