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
//#include "menu.h"
//#include "datetime.h"
#include "cour6pt7b.h"
#include "courbd6pt7b.h"
#include "cour8pt7b.h"
#include "Calendar.h"

//extern void printCenterString(char *str, bool inverted);
//extern void printCenterString(char *str);
//
//extern Adafruit_SharpMem display;
//extern DS3232RTC MyDS3232;
//extern bool invert;
//extern bool calendarGrid = true; // Display grid by default.
//extern byte calendarDayOffWeek = 0; // Default to Monday

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

Calendar::Calendar(Adafruit_SharpMem& display) : _display(display), 
	_invert(false), 
	_displayGrid(true), 
	_calendarDayOffWeek(0) // Default to Monday
{
}

void Calendar::displayCalendar(tmElements_t currTime)
{
	// Clear down bottom area of the screen
	_display.fillRect(0, _display.height() / 2, _display.width(), _display.height(), _invert ? BLACK : WHITE);

	displayDOW();
	displayDates(currTime);
	if (_displayGrid) // Display the grid ?
	{
		displayCalendarGrid();
	}
}

void Calendar::invert(bool invert)
{
	_invert = invert;
}

void Calendar::displayGrid(bool grid)
{
	_displayGrid = grid;
}

void Calendar::displayDates(tmElements_t currTime)
{
	_display.setTextSize(1);
	_display.setFont(&cour6pt7b);
	_display.setTextColor(_invert ? WHITE: BLACK, _invert ? BLACK : WHITE);

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
				_display.fillRect(x1, y1, CAL_CELL_WIDTH, CAL_CELL_HEIGHT, _invert ? WHITE : BLACK);
				_display.setTextColor(_invert ? BLACK: WHITE, _invert ? WHITE : BLACK);
				// Output the date
				stringUtils::printCenterString(_display, !_invert, weekStr);
			}
			else
			{
				// Output the date
				stringUtils::printCenterString(_display, _invert, weekStr);
			}

			if (dom == startDate)
			{
				_display.setTextColor(_invert ? WHITE: BLACK, _invert ? BLACK : WHITE);
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
	_display.setFont(&courbd6pt7b);
	_display.setTextColor(_invert ? WHITE: BLACK, _invert ? BLACK : WHITE);

	int arrIndex = _calendarDayOffWeek;
	for(int loop = 0; loop < CAL_COLUMNS; loop++)
	{
		uint8_t cellCenter = (CAL_XPOS + 1) + ((CAL_CELL_WIDTH + 1 )/ 2.0) + (CAL_CELL_WIDTH * loop);
		_display.setCursor(cellCenter, CAL_YPOS + CAL_CELL_HEIGHT - 2);
		stringUtils::printCenterString(_display, _invert, (char *)dayOfWeek[arrIndex]);
		arrIndex++;
		arrIndex %= 7;  // Days in week
	}
}

void Calendar::displayCalendarGrid()
{
	// Rows
	for(int loop = 0; loop < CAL_ROWS + 1; loop++)
	{
		_display.writeLine(CAL_XPOS, CAL_YPOS + (CAL_CELL_HEIGHT * loop), CAL_XPOS + (CAL_CELL_WIDTH * CAL_COLUMNS), CAL_YPOS + (CAL_CELL_HEIGHT * loop), _invert ? WHITE : BLACK);
	}
	
	// Columns
	for(int loop = 0; loop < CAL_COLUMNS + 1; loop++)
	{
		_display.writeLine(CAL_XPOS + (CAL_CELL_WIDTH * loop), CAL_YPOS, CAL_XPOS + (CAL_CELL_WIDTH * loop), CAL_YPOS + (CAL_CELL_HEIGHT * CAL_ROWS) , _invert ? WHITE : BLACK);
	}
}

uint8_t Calendar::getCalendarStartDate(tmElements_t currTime)
{
	// Monday is 0, Tuesday is 1...
	uint8_t currDow = time_dow(currTime.Year + 2000, currTime.Month + 1, currTime.Day);
	uint8_t dom = currTime.Day; // Day in Month (1..28/29/30/31)

	// Calculate date on previous calendarDayOffWeek
	int8_t dayOffset = (currDow - _calendarDayOffWeek + 7) % 7;
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

//extern WatchMenu menu;
//extern s_menuNowSetting setting;
//extern void saveDateFunc();
//WatchMenu calendarMenu(display);

//------------------------------------------------------------
// Below is the code for calendar settings
//------------------------------------------------------------
//void calendarFunc()
//{
	//calendarMenu.initMenu(1);  // Create a menu system with ? menu rows
	//calendarMenu.setTextSize(1);
	//calendarMenu.setFont(&cour8pt7b);
	//calendarMenu.createMenu(MENU_MAIN_INDEX, 4, PSTR("<CALENDAR>"), MENU_TYPE_STR, calendarDownFunc, calendarUpFunc);
	//calendarMenu.createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_SAVE_INDEX, PSTR("Save"), NULL, saveDateFunc);
	//calendarMenu.createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_EXIT_INDEX, PSTR("Exit"), NULL, calendarBack);
	//calendarMenu.invertDisplay(invert);
//
	//showCalendarGridOptions();
	//showCalendarStartDayOptions();
//
	//// Default to date selected
	//calendarMenu.selectedOption(MENU_MAIN_INDEX, OPTION_CALENDAR_GRID_INDEX); // Set the default selection to the grid
	//
	//// Point to menu
	//currentMenu = &calendarMenu;
//
	//display.fillRect(0, 64, 128, 128, invert ? BLACK : WHITE); // Clear bottom of display
//}
//
//void calendarBack()
//{
	//// Point to top level menu
	//currentMenu = &menu;
//}
//
////--------------------------------------------------------------
////--------------------------------------------------------------
//void calendarGridDraw()
//{
	//calendarGrid = setting.val;
	//showCalendarGridOptions(6, calendarGrid ? 2 : 3); // Highlight 2(on), 3(off)
//}
//
////--------------------------------------------------------------
////--------------------------------------------------------------
//void calendarStartDayDraw()
//{
	//calendarDayOffWeek = setting.val;
	//showCalendarStartDayOptions(6, 3);
//}
//
////--------------------------------------------------------------
////--------------------------------------------------------------
//byte getMaxCalendarValForSetting()
//{
	//byte max;
	//switch(setting.now)
	//{
		//case SETTING_CALENDAR_GRID_ONOFF:
		//max = 1;
		//break;
		//case SETTING_CALENDAR_STARTDAY:
		//max = 6;
		//break;
		//default:
		//max = 1;
		//break;
	//}
	//return max;
//}
//
////------------------------------
//// Move the menu up on key press
////------------------------------
//void calendarDataUp()
//{
	//setting.val++;
	//if(setting.val > getMaxCalendarValForSetting())
	//setting.val = 0;
//}
//
////--------------------------------
//// Move the menu down on key press
////--------------------------------
//void calendarDataDown()
//{
	//setting.val--;
	//byte max = getMaxCalendarValForSetting();
	//if(setting.val > max)
	//setting.val = max;
//}
//
////----------------------------------------------------
////----------------------------------------------------
//void selectCalendarGrid()
//{
	//// Set the up and down buttons, and drawing routine to new functions
	//calendarMenu.setDownFunc(calendarDataUp);
	//calendarMenu.setUpFunc(calendarDataDown);
	//calendarMenu.setDrawFunc(calendarGridDraw);
//
	//switch(setting.now)
	//{
		//case SETTING_CALENDAR_NOW_NONE:
		//{
			//setting.val = calendarGrid;
			//setting.now = SETTING_CALENDAR_GRID_ONOFF;
			//break;
		//}
		//default:
		//calendarGrid = setting.val;
		//setting.now = SETTING_CALENDAR_NOW_NONE;
//
		//// Go back to menu after finishing the editing of the date.
		//// TODO - Find a nicer way to do this................
		//calendarMenu.setDownFunc(calendarDownFunc);
		//calendarMenu.setUpFunc(calendarUpFunc);
		//calendarMenu.setDrawFunc(NULL);
		//break;
	//}
//
	//// Display the new option
	//showCalendarGridOptions();
//}
//
////----------------------------------------------------
////----------------------------------------------------
//void selectCalendarStartDay()
//{
	//// Set the up and down buttons, and drawing routine to new functions
	//calendarMenu.setDownFunc(calendarDataUp);
	//calendarMenu.setUpFunc(calendarDataDown);
	//calendarMenu.setDrawFunc(calendarStartDayDraw);
//
	//switch(setting.now)
	//{
		//case SETTING_CALENDAR_NOW_NONE:
		//setting.val = calendarDayOffWeek;
		//setting.now = SETTING_CALENDAR_STARTDAY;
		//break;
		//default:
		//calendarDayOffWeek = setting.val;
		//setting.now = SETTING_CALENDAR_NOW_NONE;
		//
		//// Go back to menu after finishing the editing of the date.
		//// TODO - Find a nicer way to do this................
		//calendarMenu.setDownFunc(calendarDownFunc);
		//calendarMenu.setUpFunc(calendarUpFunc);
		//calendarMenu.setDrawFunc(NULL);
		//
		//break;
	//}
//
	//// Display the new option
	//showCalendarStartDayOptions();
//}
//
//void showCalendarGridOptions()
//{
	//showCalendarGridOptions(-1, 0); // No invert
//}
//
////--------------------------------------------------------------
////--------------------------------------------------------------
//void showCalendarGridOptions(int16_t invert_start, int16_t invert_length)
//{
	//char buff[21];
	//sprintf_P(buff, PSTR("Grid: %s"), calendarGrid ? PSTR("On") : PSTR("Off"));
	//calendarMenu.createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_GRID_INDEX, invert_start, invert_length, buff, NULL, selectCalendarGrid);
//}
//
//void showCalendarStartDayOptions()
//{
	//showCalendarStartDayOptions(-1, 0); // No invert
//}
//
////--------------------------------------------------------------
////--------------------------------------------------------------
//void showCalendarStartDayOptions(int16_t invert_start, int16_t invert_length)
//{
	//char buff[21];
	//sprintf_P(buff, PSTR("Day : %s"), dayOfWeekLong[calendarDayOffWeek]);
	//calendarMenu.createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_STARTDAY_INDEX, invert_start, invert_length, buff, NULL, selectCalendarStartDay);
//}
////----------------------------------------
////----------------------------------------
//void calendarDownFunc()
//{
	//calendarMenu.upOption();
//}
//
////----------------------------------------
////----------------------------------------
//void calendarUpFunc()
//{
	//calendarMenu.downOption();
//}
//
////----------------------------------------------------------------
////----------------------------------------------------------------
//void saveCalendarFunc()
//{
	//// Save data here
	//
	//// Change the option text to Saved
	//calendarMenu.createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_SAVE_INDEX, PSTR("Saved"), NULL, saveDateFunc);
//}
