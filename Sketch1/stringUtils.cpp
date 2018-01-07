#include <stdint.h>
#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include "stringUtils.h"

void stringUtils::printCenterString(Adafruit_SharpMem& display, bool inverted, char *str)
{
	int16_t tempX;
	int16_t tempY;
	uint16_t w;
	uint16_t h;
	// Get the string width
	display.getTextBounds(str, 0, 0, &tempX, &tempY, &w, &h);

	int poX = display.getCursorX() - w / 2;
	int poY = display.getCursorY();
	display.setCursor(poX, poY);
	display.setTextColor(inverted ? WHITE: BLACK, inverted ? BLACK : WHITE);
	display.print(str);
}
