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
#include "defs.h"

#define CAL_XPOS          6
#define CAL_YPOS          64
#define CAL_WIDTH         126
#define CAL_HEIGHT        64
#define CAL_CELL_WIDTH    16
#define CAL_CELL_HEIGHT   12
#define CAL_ROWS          5
#define CAL_COLUMNS       7
#define TEXT_OFFSET_X     3
#define TEXT_OFFSET_Y     1

class Adafruit_SharpMem;

class Calendar
{
public:
	Calendar(Adafruit_SharpMem& display);
	void displayCalendar(tmElements_t currTime);
	void invert(bool invert);
	void setFont(const GFXfont* font);
	void setDOWFont(const GFXfont* font);
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
	const GFXfont* _font;
	const GFXfont* _dowfont;
	bool _displayGrid;
	uint8_t _calendarDayOffWeek;
};



#endif /* CALENDAR_H_ */