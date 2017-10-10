#include "Potentiometer.h"

Potentiometer::Potentiometer()
{
}

Potentiometer::Potentiometer(int pin)
{
  pin = pin;
}

Potentiometer::Potentiometer(int pin, int range, int averageOver)
{
  this->pin = pin;
  this->range = range;
  this->averageOver = averageOver;
}

float Potentiometer::GetValue()
{
  int sensor = GetDebouncedValue();
  if (InBetween(sensor, oldSensor - range, oldSensor + range))
  {
    return ConvertToVoltage(oldSensor);
  }
  this->oldSensor = sensor;
  return ConvertToVoltage(sensor);
}

float Potentiometer::GetValue(float nearest)
{
	return RoundNearest(GetValue(), nearest);
}

int Potentiometer::GetDebouncedValue()
{
  //
  // read the input on analog pin 0:
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //
  int val[averageOver];
  for(int i = 0; i < averageOver; i++) {
    val[i] = analogRead(this->pin);
    delay(1);
  }
  //
  // Avarage the value (debounce)
  //
  return Average(val);
}

float Potentiometer::ConvertToVoltage(int value)
{
  return float(value) * 5.0 / (1023.0);
}

int Potentiometer::Average(int values[])
{
  int total = 0;
  for(int i = 0; i < averageOver; i++) {
    total += values[i];
  }
  return total / averageOver;
}

bool Potentiometer::InBetween(int value, int min, int max)
{
  if ((value >= min) && (value <= max))
   {
    return true;
   }
  return false; 
}

bool Potentiometer::InBetween(float value, float min, float max)
{
  if ((value >= min) && (value <= max))
   {
    return true;
   }
  return false; 
}

float Potentiometer::RoundNearest(float value, float toDecimal)
{
  return round(value * (1.0 / toDecimal)) / (1.0 / toDecimal);
}
