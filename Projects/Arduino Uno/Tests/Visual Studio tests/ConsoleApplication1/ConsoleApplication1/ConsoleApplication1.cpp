// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "ConsoleApplication1.h"
#include <math.h>

using namespace std;


void translateIR(unsigned long value) // takes action based on IR code received

				   // describing Remote IR codes 

{
	typedef struct {
		unsigned long int action;
		const char* message;
	} lookuptab;

	lookuptab tab[] = {
		{ 0xFF629D, "FORWARD" },
		{ 0xFF22DD, "LEFT" },
		{ 0xFF02FD, "-OK-" },
		{ 0xFFC23D, "RIGHT" },
		{ 0xFFA857, "REVERSE" },
		{ 0xFF6897, "1" },
		{ 0xFF9867, "2" },
		{ 0xFFB04F, "3" },
		{ 0xFF30CF, "4" },
		{ 0xFF18E7, "5" },
		{ 0xFF7A85, "6" },
		{ 0xFF10EF, "7" },
		{ 0xFF38C7, "8" },
		{ 0xFF5AA5, "9" },
		{ 0xFF42BD, "*" },
		{ 0xFF4AB5, "0" },
		{ 0xFF52AD, "#" },
		{ 0xFFFFFFFF, "Repeat" }
	};

	bool found = false;
	for (int i = 0; i < sizeof(tab) / sizeof(*tab); i++) {
		if (value == tab[i].action) {
			printf(tab[i].message);
			found = true;
			break;
		}
	}
	if (!found) {
		printf("Not a button");
	}
}
int main()
{
	char str[80];
	sprintf_s(str, "Value of Pi = %f - %f", 2.52*20,  round(2.57 * 20.0) / 20.0);
	printf(str);
	translateIR(0);
    return 0;
}

