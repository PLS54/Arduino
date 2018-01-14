#include <Arduino.h>

class Timer{
private:
  unsigned long lastTrigger;
  unsigned long startTime;
  bool watching;
  bool autoRestart = false;
  unsigned long period = 0;
  unsigned long count = 0;

public:
 Timer();
 Timer(unsigned long period);
 void Start(unsigned long period);
 bool IsElapse();
};

