#include <SonyIRRemoteDecoder.h>


void setup() 
{
  Serial.begin(9600);   // Status message will be sent to PC at 9600 baud
  //unsigned int  rawData[32] = {44681, 47,12, 24,11, 23,13, 23,12, 12,12, 12,11, 12,12, 12,11, 11,13, 11,13, 12,11, 11,13, 24,11, 12,12, 24,11, 23};  // SONY 700B
//  unsigned int  rawData[32] = {51373, 47,12, 11,13, 22,13, 11,13, 11,12, 11,13, 11,12, 11,13, 11,13, 11,12, 11,13, 11,13, 22,13, 11,13, 22,13, 23};  // SONY 200B
//unsigned int  rawData[26] = {10802, 47,12, 11,13, 11,13, 23,12, 23,12, 24,12, 23,12, 23,13, 10,13, 11,13, 11,12, 11,13, 23};
unsigned int  rawData[42] = {18281, 47,12, 11,12, 11,13, 23,12, 23,13, 23,12, 23,13, 23,12, 11,13, 10,13, 11,13, 11,12, 24,12, 11,12, 11,13, 11,13, 10,13, 23,13, 11,12, 11,13, 11};  // SONY 3E108

  SonyIRRemoteDecoder decoder;
  if (decoder.DecodeRaw(rawData, sizeof(rawData) / sizeof(rawData[0]))) {
    Serial.print("Result: 0x");
    Serial.println(decoder.GetValue(), HEX);
  }
}

void loop() 
{
}





