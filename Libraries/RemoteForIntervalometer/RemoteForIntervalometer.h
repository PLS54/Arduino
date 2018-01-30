#include <Arduino.h>
#include <IRremote.h>
#include <Timer.h>
#include <DisplayForIntervalometer.h>

/*#define FAST_BACKWARD   0xCC108
#define PLAY            0x0200B
#define PAUSE           0x0400B
#define STOP            0x0000B
#define FAST_FORWARD    0x2C108
#define DOWN_ARROW      0x9E108
#define UP_ARROW        0x1E108
#define LEFT_ARROW      0x5E108
#define RIGHT_ARROW     0xDE108
#define RETURN          0xBE108
#define YELLOW          0xE010C
#define BLUE            0x2010C
#define ENTER           0x3E108
#define ONE				0x00108
#define TWO				0x80108
#define THREE			0x40108
#define FOUR			0xC0108
#define FIVE			0x20108
#define SIX				0xA0108
#define SEVEN			0x60108
#define EIGHT			0xE0108
#define NINE			0x10108
#define ZERO			0x90108
#define MUTING			0x0140A
*/
#define FAST_BACKWARD   0x36293A
#define PLAY            0x37990C
#define PAUSE           0x374117
#define STOP            0x365934
#define FAST_FORWARD    0x36293A
#define DOWN_ARROW      0x37A10B
#define UP_ARROW        0x36812F
#define LEFT_ARROW      0x37810F
#define RIGHT_ARROW     0x364137
#define ENTER          0x366133
#define YELLOW          0x37E902
#define BLUE            0x36193C
#define RETURN           0x36E123
#define ONE				0x36113D
#define TWO				0x37111D
#define THREE			0x36912D
#define FOUR			0x37910D
#define FIVE			0x365135
#define SIX				0x375115
#define SEVEN			0x36D125
#define EIGHT			0x37D105
#define NINE			0x363139
#define ZERO			0x373119
#define MUTING			0x36F920 // MENU


class RemoteForIntervalometer
{
public:
	enum mode {initial, input, instant, running, paused};

	private:
	unsigned long lastCommand = 0;
	bool IsCommand(unsigned long command, unsigned long irValue, bool withRepeat = false);

	IRrecv* irDetect;
	decode_results irIn;


	Timer& intervalTimer;
	Timer& displayTimeout;
	DisplayForIntervalometer display;

	enum remoteActions {none, one, two, three, four, five, six, seven, eight, nine, zero, 
						start, startFast, pause, stop, brightDown, brightUp, toggleDiplay, 
						deleteLastChar, resetDisplay, takePicture, endMarker};

	mode currentMode = input;
	bool inputError = false;
	unsigned int previousDisplay = 0;

	unsigned long numToDisplay = 0;  //Variable to interate
	unsigned long interval = 0;

public:
	RemoteForIntervalometer(uint8_t irPin, Timer& intervalometerTimer, Timer& displayTimeout, DisplayForIntervalometer& display);
	remoteActions GetAction(unsigned long irValue);
	void ProcessRemoteInput();
};