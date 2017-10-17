#include <Arduino.h>
#include <IRremote.h>

#define FAST_BACKWARD   0xCC108
#define PLAY            0x200B
#define FAST_FORWARD    0x2C108
#define DOWN_ARROW      0x9E108
#define UP_ARROW        0x1E108
#define LEFT_ARROW      0x5E108
#define RIGHT_ARROW     0xDE108
#define RETURN          0xBE108
#define YELLOW          0xE010C
#define BLUE            0x2010C
#define ENTER           0x3E108

#define MOVE_TO_0       FAST_BACKWARD
#define MOVE_TO_50      PLAY
#define MOVE_TO_100     FAST_FORWARD
#define MOVE_DOWN_10    DOWN_ARROW
#define MOVE_UP_10      UP_ARROW
#define ADJUST_DOWN     LEFT_ARROW
#define ADJUST_UP       RIGHT_ARROW
#define REPORT_POSITION RETURN


class RemoteForBlinds
{
private:
	IRrecv* irDetect;
	decode_results irIn;
	bool remoteActive = true;
	unsigned long lastCommand = 0;
	bool IsCommand(unsigned long command, unsigned long irValue);

public:
	enum blindActions {none, moveTo0, moveTo50, moveTo100, moveDown10, moveUp10, adjustDown, adjustUp, reportPosition, remoteActivated, remoteDeactivated};
	
	RemoteForBlinds(unsigned int irPin);
	blindActions GetAction();
	void Reactivate();
};