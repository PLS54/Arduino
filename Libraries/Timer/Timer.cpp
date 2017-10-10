#include "Timer.h"

Timer::Timer()
{
 lastTrigger = millis();
 watching = false;;
}

void Timer::Start()
{
  lastTrigger = millis();
  watching = true;
}

bool Timer::IsElapse(unsigned long period)
{
  unsigned long curMillis = millis();
  if (!watching) {
    return true;
  }
  if ((curMillis < lastTrigger) || ((lastTrigger + period) < curMillis)) {
    watching = false;
    return true;
  }
  return false;
}
