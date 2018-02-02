#include "Timer.h"

Timer::Timer()
{
	lastTrigger = millis();
	running = false;;
	autoRestart = false;
}

Timer::Timer(unsigned long period)
{
	StartAuto(period);
}

void Timer::Start(unsigned long period)
{
	lastTrigger = millis();
	running = true;
	autoRestart = false;
	this->period = period;
}

void Timer::StartAuto(unsigned long period)
{
	lastTrigger = millis();
	startTime = lastTrigger;
	autoRestart = true;
	this->period = period;
	running = true;
	count = 1;
}

void Timer::Stop()
{
	running = false;
}

void Timer::Restart()
{
	StartAuto(this->period);
}

bool:: Timer::Running()
{
	return running;
}

bool Timer::IsElapse()
{
	if (!running && !autoRestart) {
		return false;
	}
	unsigned long curMillis = millis();
	if ((curMillis < lastTrigger) || ((lastTrigger + period) < curMillis)) {
		if (autoRestart) {
			lastTrigger = startTime + (period * count++);
			return true;	
		}
		running = false;
		return true;
	}
	return false;
}
unsigned long Timer::GetTimeLeftBeforeTrigger()
{
	return period - (millis() - lastTrigger);
}
