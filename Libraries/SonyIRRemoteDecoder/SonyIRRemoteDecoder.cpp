#include <SonyIRRemoteDecoder.h>

#define DEBUG 0

#if DEBUG
# define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
# define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
# define DBG_PRINT(...)
# define DBG_PRINTLN(...)
#endif


int  MATCH (int measured,  int desired)
{
  DBG_PRINT(F("Testing: "));
  DBG_PRINT(TICKS_LOW(desired), DEC);
  DBG_PRINT(F(" <= "));
  DBG_PRINT(measured, DEC);
  DBG_PRINT(F(" <= "));
  DBG_PRINT(TICKS_HIGH(desired), DEC);

  bool passed = ((measured >= TICKS_LOW(desired)) && (measured <= TICKS_HIGH(desired)));
  if (passed)
    DBG_PRINTLN(F("?; passed"));
  else
    DBG_PRINTLN(F("?; FAILED")); 
  return passed;
}

//+========================================================
// Due to sensor lag, when received, Marks tend to be 100us too long
//
int  MATCH_MARK (int measured_ticks,  int desired_us)
{
  DBG_PRINT(F("Testing mark (actual vs desired): "));
  DBG_PRINT(measured_ticks * USECPERTICK, DEC);
  DBG_PRINT(F("us vs "));
  DBG_PRINT(desired_us, DEC);
  DBG_PRINT("us"); 
  DBG_PRINT(": ");
  DBG_PRINT(TICKS_LOW(desired_us + MARK_EXCESS) * USECPERTICK, DEC);
  DBG_PRINT(F(" <= "));
  DBG_PRINT(measured_ticks * USECPERTICK, DEC);
  DBG_PRINT(F(" <= "));
  DBG_PRINT(TICKS_HIGH(desired_us + MARK_EXCESS) * USECPERTICK, DEC);

  bool passed = ((measured_ticks >= TICKS_LOW (desired_us + MARK_EXCESS))
                && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS)));
  if (passed)
    DBG_PRINTLN(F("?; passed"));
  else
    DBG_PRINTLN(F("?; FAILED")); 
  return passed;
}
//+========================================================
// Due to sensor lag, when received, Spaces tend to be 100us too short
//
int  MATCH_SPACE (int measured_ticks,  int desired_us)
{
  DBG_PRINT(F("Testing space (actual vs desired): "));
  DBG_PRINT(measured_ticks * USECPERTICK, DEC);
  DBG_PRINT(F("us vs "));
  DBG_PRINT(desired_us, DEC);
  DBG_PRINT("us"); 
  DBG_PRINT(": ");
  DBG_PRINT(TICKS_LOW(desired_us - MARK_EXCESS) * USECPERTICK, DEC);
  DBG_PRINT(F(" <= "));
  DBG_PRINT(measured_ticks * USECPERTICK, DEC);
  DBG_PRINT(F(" <= "));
  DBG_PRINT(TICKS_HIGH(desired_us - MARK_EXCESS) * USECPERTICK, DEC);

  bool passed = ((measured_ticks >= TICKS_LOW (desired_us - MARK_EXCESS))
                && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS)));
  if (passed)
    DBG_PRINTLN(F("?; passed"));
  else
    DBG_PRINTLN(F("?; FAILED")); 
  return passed;
}
SonyIRRemoteDecoder::SonyIRRemoteDecoder()
{
  Serial.println("SonyIRRemoteDecoder creator");
}
bool  SonyIRRemoteDecoder::DecodeRaw(unsigned int* rawData, int rawLength)
{
  DBG_PRINTLN("Starting Sony decode");
  
  rawbuf = rawData;
  rawlen = rawLength;
  DBG_PRINT("rawlen: ");
  DBG_PRINTLN(rawlen, DEC);
  DBG_PRINTLN(sizeof(rawData), DEC);
  long  data   = 0;
  int   offset = 0;  // Dont skip first space, check its size
  
  if (rawlen < (2 * SONY_BITS) + 2)  return false ;

  // Some Sony's deliver repeats fast after first
  // unfortunately can't spot difference from of repeat from two fast clicks
  if (rawbuf[offset] < SONY_DOUBLE_SPACE_USECS) {
    // Serial.print("IR Gap found: ");
    bits = 0;
    value = REPEAT;

# ifdef DECODE_SANYO
    decode_type = SANYO;
# else
    decode_type = UNKNOWN;
# endif

      return true;
  }
  DBG_PRINT("Offset: ");
  DBG_PRINT(offset, DEC);
  DBG_PRINT(" value: ");
  DBG_PRINTLN(rawbuf[offset], DEC);
  offset++;
  // Initial mark
  DBG_PRINT("Offset: ");
  DBG_PRINT(offset, DEC);
  DBG_PRINT(" value: ");
  DBG_PRINTLN(rawbuf[offset], DEC);
  if (!MATCH_MARK(rawbuf[offset++], SONY_HDR_MARK))  return false ;
  while (offset + 1 < rawlen) {
    DBG_PRINT("Offset: ");
    DBG_PRINT(offset, DEC);
    DBG_PRINT(" value: ");
    DBG_PRINTLN(rawbuf[offset], DEC);
   if (!MATCH_SPACE(rawbuf[offset++], SONY_HDR_SPACE))  break ;

    if      (MATCH_MARK(rawbuf[offset], SONY_ONE_MARK))   data = (data << 1) | 1 ;
    else if (MATCH_MARK(rawbuf[offset], SONY_ZERO_MARK))  data = (data << 1) | 0 ;
    else                                                           return false ;
    offset++;
  }

  // Success
  bits = (offset - 1) / 2;
  if (bits < 12) {
    bits = 0;
    return false;
  }
  value       = data;
  decode_type = SONY;
  return true;
}

unsigned int  SonyIRRemoteDecoder::GetValue()
{
	return value;
}
