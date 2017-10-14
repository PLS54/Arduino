#include <Arduino.h>
#include <Stepper.h>

class Blind
{
#define FULL_OPEN 2048
private:
  Stepper* stepper;
  unsigned int currentPosition = -1;
public:
  Blind(Stepper* stepper, unsigned int startPos);
  void Move(int steps);
  void MoveTo(unsigned int value);
  unsigned int GetCurrentPosition();
};
