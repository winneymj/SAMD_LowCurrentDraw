/*
 * CalendarSettingsMenu.h
 *
 * Created: 2/6/2018 5:15:56 PM
 *  Author: dell
 */ 


#ifndef CALENDARSETTINGSMENU_H_
#define CALENDARSETTINGSMENU_H_

#include <Watch_Menu.h>

class CalendarSettingsMenu
{
public:
	static void calendarFunc();
	static void calendarBack();
	static void calendarDownFunc();
	static void calendarUpFunc();
	static void saveDateFunc();
	static void saveCalendarFunc();
	static void calendarStartDayDraw();
	static void selectCalendarStartDay();
	static void selectCalendarGrid();
	static void calendarDataDown();
	static void calendarDataUp();
	static void calendarGridDraw();


	void showCalendarStartDayOptions(int16_t invert_start, int16_t invert_length);
	void showCalendarStartDayOptions();
	void showCalendarGridOptions(int16_t invert_start, int16_t invert_length);
	void showCalendarGridOptions();
	byte getMaxCalendarValForSetting();
	
public:
	static WatchMenu* _menu;
	static byte _calendarDayOffWeek;
	static bool _calendarGrid; // Display grid by default.

};




#endif /* CALENDARSETTINGSMENU_H_ */