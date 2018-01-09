/*
 * Calendar.h
 *
 * Created: 1/8/2018 6:47:48 PM
 *  Author: dell
 */ 


#ifndef CALENDAR_H_
#define CALENDAR_H_

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <Adafruit_GFX.h>
//#include <Adafruit_SharpMem.h>
#include "defs.h"

class Adafruit_SharpMem;

class Calendar
{
public:
	Calendar(Adafruit_SharpMem& display);
	void displayCalendar(tmElements_t currTime);
	void invert(bool invert);
	void displayGrid(bool grid);

private:
	void displayDates(tmElements_t currTime);
	uint8_t daysMonth(tmElements_t currTime, uint8_t offset); // -1 previous month, 0 = current month
	void displayDOW();
	void displayCalendarGrid();
	uint8_t getCalendarStartDate(tmElements_t currTime);


private:
	bool _invert;
	Adafruit_SharpMem& _display;
	bool _displayGrid;
	uint8_t _calendarDayOffWeek;
};



#endif /* CALENDAR_H_ */