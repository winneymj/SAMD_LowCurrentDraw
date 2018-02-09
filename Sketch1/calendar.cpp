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
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <Watch_Menu.h>

#include "defs.h"
#include "stringUtils.h"
#include "Calendar.h"
#include "GlobalSettings.h"

Calendar::Calendar(Adafruit_SharpMem& display): 
	_display(display)
{
}

void Calendar::displayCalendar(tmElements_t currTime)
{
	// Clear down bottom area of the screen
	_display.fillRect(0, _display.height() / 2, _display.width(), _display.height(), GlobalSettings::_inverted ? BLACK : WHITE);

	displayDOW();
	displayDates(currTime);
	if (GlobalSettings::_calendarGrid) // Display the grid ?
	{
		displayCalendarGrid();
	}
}

void Calendar::setFont(const GFXfont* font)
{
	_font = font;
}

void Calendar::setDOWFont(const GFXfont* font)
{
	_dowfont = font;
}

void Calendar::displayDates(tmElements_t currTime)
{
	_display.setTextSize(1);
	_display.setFont(_font);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);

	// Get the date to start printing in the top left of calendar, Last StartDay.
	uint8_t startDate = getCalendarStartDate(currTime);

	uint8_t col_index = 0;
	uint8_t daysForMonth = 0;
	uint8_t dom = currTime.Day;
	// If dom is < 7 then possible to go back into previous month
	if (dom < 7)
	{
		daysForMonth = daysMonth(currTime, -1); // Previous month
	}
	else
	{
		daysForMonth = daysMonth(currTime, 0); // Current month
	}
	
	for(int row = 1; row < 5; row++)
	{
		for(int column = 1; column < 8; column++)
		{
			uint8_t cellCenter = CAL_XPOS + ((CAL_CELL_WIDTH + 1 )/ 2.0) + (CAL_CELL_WIDTH * (column - 1));
			uint8_t xpos = cellCenter - 1;
			uint8_t ypos = CAL_YPOS + TEXT_OFFSET_Y + (CAL_CELL_HEIGHT * (row + 1)) - 3;
			_display.setCursor(xpos, ypos);
			char weekStr[3] = {0};
			sprintf_P(weekStr, PSTR("%u"), startDate);

			// Invert color when today is current date
			if (dom == startDate)
			{
				int16_t x1 = CAL_XPOS + (CAL_CELL_WIDTH * (column - 1));
				int16_t y1 = CAL_YPOS + (CAL_CELL_HEIGHT * row);
				_display.fillRect(x1, y1, CAL_CELL_WIDTH, CAL_CELL_HEIGHT, GlobalSettings::_inverted ? WHITE : BLACK);
				_display.setTextColor(GlobalSettings::_inverted ? BLACK: WHITE, GlobalSettings::_inverted ? WHITE : BLACK);
				// Output the date
				stringUtils::printCenterString(_display, !GlobalSettings::_inverted, weekStr);
			}
			else
			{
				// Output the date
				stringUtils::printCenterString(_display, GlobalSettings::_inverted, weekStr);
			}

			if (dom == startDate)
			{
				_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);
			}
			startDate++;
			// Check if greater then end of month date and set to 1
			if (startDate > daysForMonth)
			{
				startDate = 1;
			}
		}
	}
}

uint8_t Calendar::daysMonth(tmElements_t currTime, uint8_t offset) // -1 previous month, 0 = current month
{
	// Goes back into previous month
	int8_t newMonth = currTime.Month + offset;
	
	int8_t prevMonth = newMonth < 0 ? 12 : newMonth > 12 ? 0 : newMonth;
	return dayInMonth[prevMonth];
}

void Calendar::displayDOW()
{
	_display.setTextSize(1);
	_display.setFont(_dowfont);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);

	int arrIndex = GlobalSettings::_calendarDayOffWeek;
	for(int loop = 0; loop < CAL_COLUMNS; loop++)
	{
		uint8_t cellCenter = (CAL_XPOS + 1) + ((CAL_CELL_WIDTH + 1 )/ 2.0) + (CAL_CELL_WIDTH * loop);
		_display.setCursor(cellCenter, CAL_YPOS + CAL_CELL_HEIGHT - 2);
		stringUtils::printCenterString(_display, GlobalSettings::_inverted, (char *)dayOfWeek[arrIndex]);
		arrIndex++;
		arrIndex %= 7;  // Days in week
	}
}

void Calendar::displayCalendarGrid()
{
	// Rows
	for(int loop = 0; loop < CAL_ROWS + 1; loop++)
	{
		_display.writeLine(CAL_XPOS, CAL_YPOS + (CAL_CELL_HEIGHT * loop), CAL_XPOS + (CAL_CELL_WIDTH * CAL_COLUMNS), CAL_YPOS + (CAL_CELL_HEIGHT * loop), GlobalSettings::_inverted ? WHITE : BLACK);
	}
	
	// Columns
	for(int loop = 0; loop < CAL_COLUMNS + 1; loop++)
	{
		_display.writeLine(CAL_XPOS + (CAL_CELL_WIDTH * loop), CAL_YPOS, CAL_XPOS + (CAL_CELL_WIDTH * loop), CAL_YPOS + (CAL_CELL_HEIGHT * CAL_ROWS) , GlobalSettings::_inverted ? WHITE : BLACK);
	}
}

uint8_t Calendar::getCalendarStartDate(tmElements_t currTime)
{
	// Monday is 0, Tuesday is 1...
	uint8_t currDow = time_dow(currTime.Year + 2000, currTime.Month + 1, currTime.Day);
	uint8_t dom = currTime.Day; // Day in Month (1..28/29/30/31)

	// Calculate date on previous calendarDayOffWeek
	int8_t dayOffset = (currDow - GlobalSettings::_calendarDayOffWeek + 7) % 7;
	int8_t lastDom = dom - dayOffset;
	uint8_t daysLastMonth = 0;
	
	if (lastDom <= 0)
	{
		daysLastMonth = daysMonth(currTime, -1);
	}

	if (daysLastMonth > 0)
	{
		lastDom = daysLastMonth + lastDom;
	}
	return lastDom;
}
