#include <Arduino.h>

class Timer{
private:
 unsigned long lastTrigger;
  bool watching;

public:
 Timer();
 void Start();
 bool IsElapse(unsigned long period);
};

