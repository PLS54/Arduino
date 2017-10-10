#include <Arduino.h>

class Timer{
private:
  unsigned long lastTrigger;
  bool watching;
  bool autoRestart = false;
  unsigned long period = 0;

public:
 Timer();
 Timer(unsigned long period);
 void Start(unsigned long period);
 bool IsElapse();
};

