#include <Arduino.h>
#include <IRremote.h>

#define FAST_BACKWARD   0xCC108
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

#define MOVE_TO_0       FAST_BACKWARD
#define START      		PLAY
#define MOVE_TO_100     FAST_FORWARD
#define MOVE_DOWN_10    DOWN_ARROW
#define MOVE_UP_10      UP_ARROW
#define ADJUST_DOWN     LEFT_ARROW
#define ADJUST_UP       RIGHT_ARROW
#define REPORT_POSITION RETURN


class RemoteForIntervalometer
{
private:
	unsigned long lastCommand = 0;
	bool IsCommand(unsigned long command, unsigned long irValue, bool withRepeat = 0);

public:
	enum remoteActions {none, one, two, three, four, five, six, seven, eight, nine, zero, start, pause, stop, brightDown, brightUp, toggleDiplay, deleteLastChar, resetDisplay};
	
	RemoteForIntervalometer(int i);
	remoteActions GetAction(unsigned long irValue);
};