#include <Arduino.h>

class Potentiometer
{

private:
int pin = A0;
int range = 0;
int averageOver = 1;
int oldSensor = -1;
float ConvertToVoltage(int value);
int Average(int values[]);
float RoundNearest(float value, float toDecimal);
int GetDebouncedValue();
static bool InBetween(int value, int min, int max);

public:
  Potentiometer();
  Potentiometer(int pin);
  Potentiometer(int pin, int range, int averageOver);
  float GetValue();
  float GetValue(float nearest);
  static bool InBetween(float value, float  min, float  max);
};

