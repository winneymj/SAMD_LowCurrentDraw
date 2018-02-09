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
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

#include "defs.h"
#include "stringUtils.h"
#include "TimeDateDisplay.h"
#include "Calendar.h"
#include "arialn26pt7b.h"
#include "courbd6pt7b.h"
#include "cour6pt7b.h"
#include "watchface.h"
#include "CalendarSettingsMenu.h"
#include "GlobalSettings.h"

WatchFace::WatchFace(Adafruit_SharpMem& display, DS3232RTC& ds3232RTC) : 
	_display(display),
	_ds3232RTC(ds3232RTC),
	_invert(false),
	_calendar(Calendar(display)),
	_timeDate(TimeDateDisplay(display))
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
	
	_calendar.setFont(&cour6pt7b);
	_calendar.setDOWFont(&courbd6pt7b);

	_calendar.displayCalendar(currTime);
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
	_timeDate.setFont(&arialn26pt7b);
	_timeDate.setDateFont(&courbd6pt7b);
	_timeDate.displayDateTime(currTime);
	
//	displayTime(currTime);
//	displayLongDate(currTime);

	// Display the temperature
	_timeDate.setTempFont(&courbd6pt7b);
	_timeDate.setTempDegreeFont(&cour6pt8bDegree);
	_timeDate.displayTemp(temp);
}
