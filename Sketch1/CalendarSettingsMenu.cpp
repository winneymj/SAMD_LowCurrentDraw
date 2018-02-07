/*
 * CalendarSettingsMenu.cpp
 *
 * Created: 2/6/2018 5:16:28 PM
 *  Author: dell
 */ 

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC

#include "defs.h"
#include "MainMenu.h"
#include "CalendarSettingsMenu.h"
#include "cour8pt7b.h"

extern Adafruit_SharpMem display;
extern DS3232RTC ds3232RTC;

CalendarSettingsMenu calendarSettingMenu;
WatchMenu *CalendarSettingsMenu::_menu = new WatchMenu(display);
byte CalendarSettingsMenu::_calendarDayOffWeek = 0;
bool CalendarSettingsMenu::_calendarGrid = true; // Display grid by default.

void CalendarSettingsMenu::calendarFunc()
{
	_menu->initMenu(1);  // Create a menu system with ? menu rows
	_menu->setTextSize(1);
	_menu->setFont(&cour8pt7b);
	_menu->createMenu(MENU_MAIN_INDEX, 4, PSTR("<CALENDAR>"), MENU_TYPE_STR, CalendarSettingsMenu::calendarDownFunc, CalendarSettingsMenu::calendarUpFunc);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_SAVE_INDEX, PSTR("Save"), NULL, CalendarSettingsMenu::saveCalendarFunc);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_EXIT_INDEX, PSTR("Exit"), NULL, CalendarSettingsMenu::calendarBack);
	_menu->invertDisplay(MainMenu::_inverted);

	calendarSettingMenu.showCalendarGridOptions();
	calendarSettingMenu.showCalendarStartDayOptions();

	// Default to date selected
	_menu->selectedOption(MENU_MAIN_INDEX, OPTION_CALENDAR_GRID_INDEX); // Set the default selection to the grid
	
	// Point to menu
	MainMenu::_currentMenu = _menu;

	display.fillRect(0, 64, 128, 128, MainMenu::_inverted ? BLACK : WHITE); // Clear bottom of display
}

void CalendarSettingsMenu::calendarBack()
{
	// Point to top level menu
	MainMenu::_currentMenu = MainMenu::_menu;
}

void CalendarSettingsMenu::calendarGridDraw()
{
	_calendarGrid = MainMenu::_setting.val;
	calendarSettingMenu.showCalendarGridOptions(6, _calendarGrid ? 2 : 3); // Highlight 2(on), 3(off)
}

void CalendarSettingsMenu::calendarStartDayDraw()
{
	_calendarDayOffWeek = MainMenu::_setting.val;
	calendarSettingMenu.showCalendarStartDayOptions(6, 3);
}

byte CalendarSettingsMenu::getMaxCalendarValForSetting()
{
	byte max;
	switch(MainMenu::_setting.now)
	{
		case SETTING_CALENDAR_GRID_ONOFF:
		max = 1;
		break;
		case SETTING_CALENDAR_STARTDAY:
		max = 6;
		break;
		default:
		max = 1;
		break;
	}
	return max;
}

void CalendarSettingsMenu::calendarDataUp()
{
	MainMenu::_setting.val++;
	if(MainMenu::_setting.val > calendarSettingMenu.getMaxCalendarValForSetting())
	MainMenu::_setting.val = 0;
}

void CalendarSettingsMenu::calendarDataDown()
{
	MainMenu::_setting.val--;
	byte max = calendarSettingMenu.getMaxCalendarValForSetting();
	if(MainMenu::_setting.val > max)
	MainMenu::_setting.val = max;
}

void CalendarSettingsMenu::selectCalendarGrid()
{
	// Set the up and down buttons, and drawing routine to new functions
	_menu->setDownFunc(CalendarSettingsMenu::calendarDataUp);
	_menu->setUpFunc(CalendarSettingsMenu::calendarDataDown);
	_menu->setDrawFunc(CalendarSettingsMenu::calendarGridDraw);

	switch(MainMenu::_setting.now)
	{
		case SETTING_CALENDAR_NOW_NONE:
		{
			MainMenu::_setting.val = _calendarGrid;
			MainMenu::_setting.now = SETTING_CALENDAR_GRID_ONOFF;
			break;
		}
		default:
		_calendarGrid = MainMenu::_setting.val;
		MainMenu::_setting.now = SETTING_CALENDAR_NOW_NONE;

		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_menu->setDownFunc(CalendarSettingsMenu::calendarDownFunc);
		_menu->setUpFunc(CalendarSettingsMenu::calendarUpFunc);
		_menu->setDrawFunc(NULL);
		break;
	}

	// Display the new option
	calendarSettingMenu.showCalendarGridOptions();
}

void CalendarSettingsMenu::selectCalendarStartDay()
{
	// Set the up and down buttons, and drawing routine to new functions
	_menu->setDownFunc(CalendarSettingsMenu::calendarDataUp);
	_menu->setUpFunc(CalendarSettingsMenu::calendarDataDown);
	_menu->setDrawFunc(CalendarSettingsMenu::calendarStartDayDraw);

	switch(MainMenu::_setting.now)
	{
		case SETTING_CALENDAR_NOW_NONE:
		MainMenu::_setting.val = _calendarDayOffWeek;
		MainMenu::_setting.now = SETTING_CALENDAR_STARTDAY;
		break;
		default:
		_calendarDayOffWeek = MainMenu::_setting.val;
		MainMenu::_setting.now = SETTING_CALENDAR_NOW_NONE;
		
		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_menu->setDownFunc(CalendarSettingsMenu::calendarDownFunc);
		_menu->setUpFunc(CalendarSettingsMenu::calendarUpFunc);
		_menu->setDrawFunc(NULL);
		
		break;
	}

	// Display the new option
	calendarSettingMenu.showCalendarStartDayOptions();
}

void CalendarSettingsMenu::showCalendarGridOptions()
{
	showCalendarGridOptions(-1, 0); // No invert
}

void CalendarSettingsMenu::showCalendarGridOptions(int16_t invert_start, int16_t invert_length)
{
	char buff[21];
	sprintf_P(buff, PSTR("Grid: %s"), _calendarGrid ? PSTR("On") : PSTR("Off"));
	_menu->createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_GRID_INDEX, invert_start, invert_length, buff, NULL, CalendarSettingsMenu::selectCalendarGrid);
}

void CalendarSettingsMenu::showCalendarStartDayOptions()
{
	showCalendarStartDayOptions(-1, 0); // No invert
}

void CalendarSettingsMenu::showCalendarStartDayOptions(int16_t invert_start, int16_t invert_length)
{
	char buff[21];
	sprintf_P(buff, PSTR("Day : %s"), dayOfWeekLong[_calendarDayOffWeek]);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_STARTDAY_INDEX, invert_start, invert_length, buff, NULL, CalendarSettingsMenu::selectCalendarStartDay);
}

void CalendarSettingsMenu::calendarDownFunc()
{
	_menu->upOption();
}

void CalendarSettingsMenu::calendarUpFunc()
{
	_menu->downOption();
}

void CalendarSettingsMenu::saveCalendarFunc()
{
	// Save data here
	
	// Change the option text to Saved
	_menu->createOption(MENU_MAIN_INDEX, OPTION_CALENDAR_SAVE_INDEX, PSTR("Saved"), NULL, CalendarSettingsMenu::saveCalendarFunc);
}
