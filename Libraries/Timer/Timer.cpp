#include "Timer.h"

Timer::Timer()
{
	lastTrigger = millis();
	watching = false;;
	autoRestart = false;
}

Timer::Timer(unsigned long period)
{
	StartAuto(period);
}

void Timer::Start(unsigned long period)
{
	lastTrigger = millis();
	watching = true;
	autoRestart = false;
	this->period = period;
}
void Timer::StartAuto(unsigned long period)
{
	lastTrigger = millis();
	startTime = lastTrigger;
	autoRestart = true;
	this->period = period;
	count = 1;
}

bool Timer::IsElapse()
{
	if (!watching && !autoRestart) {
		return true;
	}
	unsigned long curMillis = millis();
	if ((curMillis < lastTrigger) || ((lastTrigger + period) < curMillis)) {
		if (autoRestart) {
			lastTrigger = startTime + (period * count++);
			return true;	
		}
		watching = false;
		return true;
	}
	return false;
}
unsigned long Timer::GetTimeLeftBeforeTrigger()
{
	return period - (millis() - lastTrigger);
}
