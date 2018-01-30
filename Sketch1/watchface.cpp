#include <stdint.h>
#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time

#include "defs.h"
#include "stringUtils.h"
#include "TimeDateDisplay.h"
#include "Calendar.h"
#include "arialn26pt7b.h"
#include "courbd6pt7b.h"
#include "cour6pt7b.h"
#include "watchface.h"

WatchFace::WatchFace(Adafruit_SharpMem& display, DS3232RTC& ds3232RTC) : _display(display), _ds3232RTC(ds3232RTC), _invert(false)
{
}

void WatchFace::invert(bool invert)
{
	_invert = invert;
}

void WatchFace::displayCalendar()
{
	// Get the time from the RTC
	tmElements_t currTime;
	_ds3232RTC.read(currTime);
	
	Calendar calendar(_display);
	calendar.setFont(&cour6pt7b);
	calendar.setDOWFont(&courbd6pt7b);
	calendar.invert(_invert);

	calendar.displayCalendar(currTime);
}


void WatchFace::displayTime()
{
	// Get the time from the RTC
	tmElements_t currTime;
	_ds3232RTC.read(currTime);

	int initTemp = _ds3232RTC.temperature();
	float temp = initTemp / 4.0;

	// Clear down entire screen
	_display.fillRect(0, 0, _display.width(), _display.height() / 2, _invert ? BLACK : WHITE);

	// Update the display
	TimeDateDisplay disp(_display);
	disp.setFont(&arialn26pt7b);
	disp.setDateFont(&courbd6pt7b);
	disp.invert(_invert);
	disp.displayDateTime(currTime);
	
//	displayTime(currTime);
//	displayLongDate(currTime);

	// Display the temperature
	disp.setTempFont(&courbd6pt7b);
	disp.setTempDegreeFont(&cour6pt8bDegree);
	disp.displayTemp(temp);
}
