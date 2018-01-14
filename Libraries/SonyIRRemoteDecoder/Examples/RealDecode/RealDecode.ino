#include <IRremote.h>
#include <SonyIRRemoteDecoder.h>

int recvPin = 12;
IRrecv irrecv(recvPin);
SonyIRRemoteDecoder decoder;

void setup() 
{
  irrecv.enableIRIn();  // Start the receiver
  Serial.begin(9600);   // Status message will be sent to PC at 9600 baud
}

void loop() 
{
  decode_results  results;        // Somewhere to store the results
  if (irrecv.decode(&results)) {
    if (decoder.DecodeRaw(results.rawbuf, results.rawlen)) {
      Serial.print("Result: 0x");
      Serial.println(decoder.GetValue(), HEX);
    }
    irrecv.resume();              // Prepare for the next value
  }
}

