#include <Arduino.h>
#include <IRremote.h>
#include <Timer.h>
#include <DisplayForIntervalometer.h>

#define SONY_ON			0x540A
#define VIDEOTRON_ON	0x37C107

#define DISPLAY_TIMEOUT   60000

class RemoteForIntervalometer
{
public:
	enum mode {initial, input, instant, running, paused};

private:
	unsigned long lastCommand = 0;
	
	Timer* intervalTimer;
	Timer displayTimeout;
	Timer* flashTimer;
	DisplayForIntervalometer* display;
	void (*callTakePicture)();
	void FlashDisplay();
	IRrecv* irDetect;
	decode_results irIn;

	
	enum remoteActions {none, one, two, three, four, five, six, seven, eight, nine, zero, 
						start, startFast, pause, stop, brightDown, brightUp, toggleDiplay, 
						disableDisplayToggle, enableDisplayToggle, deleteLastChar, resetDisplay, 
						takePicture, endMarker};

	typedef struct {
		unsigned long int sonyCode;
		unsigned long int videotronCode;
		remoteActions action;
		const char* message;
	} lookuptab;

						
	mode currentMode = input;
	bool inputError = false;
	unsigned int previousTime = 0;
	unsigned long numToDisplay = 0;  //Variable to interate
	unsigned long interval = 0;
	bool displayToogleStatus = true;
	bool sonyRemoteOn = true;
	bool videotronRemoteOn = true;
	
	remoteActions GetAction(unsigned long irValue);

public:
	RemoteForIntervalometer(uint8_t irPin, Timer* pIntervalometerTimer, Timer* pFlashTimer, DisplayForIntervalometer* pDisplay, void (*caLLtakePicture)());
	bool ProcessRemoteInput();
	RemoteForIntervalometer::mode GetCurrentMode();
	void SetNewMode(RemoteForIntervalometer::mode newMode);
	bool GetInputError();
	void ResetToPreviousTime();
};