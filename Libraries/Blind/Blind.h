#include <Arduino.h>
#include <Stepper.h>
#include <EEPROM.h>

class Blind
{
#define FULL_OPEN 2048
private:
  Stepper* stepper;
  unsigned int currentPosition = 0xFFFF;
public:
  Blind(Stepper* stepper);
  void Move(int steps);
  void MoveTo(unsigned int value);
  unsigned int GetCurrentPosition();
  void SetPosition(unsigned int position);
};
