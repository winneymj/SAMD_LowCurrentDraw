#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC

#include "defs.h"
#include "MainMenu.h"
#include "DateSettingsMenu.h"
#include "cour8pt7b.h"

extern Adafruit_SharpMem display;
extern DS3232RTC MyDS3232;

s_menuNowSetting setting;

DateSettingsMenu dateSettingMenu;
WatchMenu *DateSettingsMenu::_menu = new WatchMenu(display);
tmElements_t DateSettingsMenu::_dateDataSet = {0};

//------------------------------------------------------------
// Method is called when the date/time menu option is selected.
// It creates a new String type menu (as opposed to Icon).
//------------------------------------------------------------
void DateSettingsMenu::dateFunc()
{
	tmElements_t currTime;
	MyDS3232.read(currTime);

	// Create copy of current time & date
	memcpy(&_dateDataSet, &currTime, sizeof(tmElements_t));

	_menu->initMenu(1);  // Create a menu system with ? menu rows
	_menu->setTextSize(1);
	_menu->setFont(&cour8pt7b);
	_menu->createMenu(MENU_MAIN_INDEX, 3, PSTR("<DATE>"), MENU_TYPE_STR, DateSettingsMenu::dateDownFunc, DateSettingsMenu::dateUpFunc);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_DATE_SAVE_INDEX, PSTR("Save"), NULL, DateSettingsMenu::saveDateFunc);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_DATE_EXIT_INDEX, PSTR("Exit"), NULL, DateSettingsMenu::dateBack);
	_menu->invertDisplay(MainMenu::_inverted);

	dateSettingMenu.showDateStr();

	// Default to date selected
	_menu->selectedOption(MENU_MAIN_INDEX, OPTION_DATE_DATE_INDEX); // Set the default selection to the date
	
	// Point to date/time menu
	MainMenu::_currentMenu = _menu;

	display.fillRect(0, 64, 128, 128, MainMenu::_inverted ? BLACK : WHITE); // Clear bottom of display
}

void DateSettingsMenu::dateBack()
{
	// Point to top level menu
	MainMenu::_currentMenu = MainMenu::_menu;
}

//--------------------------------------------------------------
// Method draws the selection highlight when changing the date.
// The character highlight is reversed so it can be seen
//--------------------------------------------------------------
void DateSettingsMenu::dateDraw()
{
	int16_t invert_start = -1;
	int16_t invert_length = 0;

	switch(setting.now)
	{
		case SETTING_NOW_DAY10:
		invert_start = 1;
		invert_length = 1;
		DateSettingsMenu::_dateDataSet.Day = (setting.val * 10) + (DateSettingsMenu::_dateDataSet.Day % 10);
		break;
		case SETTING_NOW_DAY1:
		invert_start = 2;
		invert_length = 1;
		DateSettingsMenu::_dateDataSet.Day = ((DateSettingsMenu::_dateDataSet.Day / 10) * 10) + setting.val;
		break;
		case SETTING_NOW_MONTH:
		invert_start = 4;
		invert_length = 3;
		DateSettingsMenu::_dateDataSet.Month = setting.val;
		break;
		case SETTING_NOW_YEAR10:
		invert_start = 8;
		invert_length = 1;
		DateSettingsMenu::_dateDataSet.Year = (setting.val * 10) + DateSettingsMenu::_dateDataSet.Year % 10;
		break;
		case SETTING_NOW_YEAR1:
		invert_start = 9;
		invert_length = 1;
		DateSettingsMenu::_dateDataSet.Year = ((DateSettingsMenu::_dateDataSet.Year / 10) * 10) + setting.val;
		break;
		default:
		// Remove inversion
		invert_start = -1;
		invert_length = 0;
	}
	
	dateSettingMenu.showDateStr(invert_start, invert_length);
}

byte getMaxValForSetting()
{
	byte max;
	switch(setting.now)
	{
		case SETTING_NOW_DAY10:
		max = 3;
		break;
		case SETTING_NOW_DAY1:
		max = 9;
		break;
		case SETTING_NOW_MONTH:
		max = 11;
		break;
		case SETTING_NOW_YEAR10:
		max = 9;
		break;
		case SETTING_NOW_YEAR1:
		max = 9;
		break;
		case SETTING_NOW_12HR:
		case SETTING_NOW_24HR:
		max = 1;
		case SETTING_NOW_AMHR:
		case SETTING_NOW_PMHR:
		max = 1;
		break;
		default:
		max = 9;
		break;
	}
	return max;
}

//------------------------------
// Move the menu up on key press
//------------------------------
void dateDataUp()
{
	setting.val++;
	if(setting.val > getMaxValForSetting())
	setting.val = 0;
}

//--------------------------------
// Move the menu down on key press
//--------------------------------
void dateDataDown()
{
	setting.val--;
	byte max = getMaxValForSetting();
	if(setting.val > max)
	setting.val = max;
}

//----------------------------------------------------
// Method is called when the Middle button (select) is
// pressed on the date menu.
//----------------------------------------------------
void DateSettingsMenu::selectDate()
{
	// Set the up and down buttons, and drawing routine to new functions
	_menu->setDownFunc(dateDataUp);
	_menu->setUpFunc(dateDataDown);
	_menu->setDrawFunc(dateDraw);

	switch(setting.now)
	{
		case SETTING_NOW_NONE:
		setting.now = SETTING_NOW_DAY10;
		setting.val = DateSettingsMenu::_dateDataSet.Day / 10;
		break;
		case SETTING_NOW_DAY10:
		{
			byte mod = DateSettingsMenu::_dateDataSet.Day % 10;
			DateSettingsMenu::_dateDataSet.Day = (setting.val * 10) + mod;
			setting.now = SETTING_NOW_DAY1;
			setting.val = mod;
		}
		break;
		case SETTING_NOW_DAY1:
		DateSettingsMenu::_dateDataSet.Day = ((DateSettingsMenu::_dateDataSet.Day / 10) * 10) + setting.val;
		if(DateSettingsMenu::_dateDataSet.Day < 1)
		DateSettingsMenu::_dateDataSet.Day = 1;
		else if(DateSettingsMenu::_dateDataSet.Day > 31)
		DateSettingsMenu::_dateDataSet.Day = 31;
		setting.now = SETTING_NOW_MONTH;
		setting.val = DateSettingsMenu::_dateDataSet.Month;
		break;
		case SETTING_NOW_MONTH:
		{
			DateSettingsMenu::_dateDataSet.Month = setting.val;
			byte maxDays = 31;
			byte mnth = DateSettingsMenu::_dateDataSet.Month;
			if(mnth == 3 || mnth == 5 || mnth == 8 || mnth == 10)
			maxDays = 30;
			else if(mnth == 1)
			maxDays = LEAP_YEAR(DateSettingsMenu::_dateDataSet.Year) ? 29 : 28;
			if(DateSettingsMenu::_dateDataSet.Day > maxDays)
			DateSettingsMenu::_dateDataSet.Day = maxDays;
			setting.now = SETTING_NOW_YEAR10;
			setting.val = DateSettingsMenu::_dateDataSet.Year / 10;
		}
		break;
		case SETTING_NOW_YEAR10:
		{
			byte mod = DateSettingsMenu::_dateDataSet.Year % 10;
			DateSettingsMenu::_dateDataSet.Year = (setting.val * 10) + mod;
			setting.now = SETTING_NOW_YEAR1;
			setting.val = mod;
		}
		break;
		default:
		DateSettingsMenu::_dateDataSet.Year = ((DateSettingsMenu::_dateDataSet.Year / 10) * 10) + setting.val;
		DateSettingsMenu::_dateDataSet.Wday = time_dow(DateSettingsMenu::_dateDataSet.Year + 2000, DateSettingsMenu::_dateDataSet.Month + 1, DateSettingsMenu::_dateDataSet.Day);
		setting.now = SETTING_NOW_NONE;

		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_menu->setDownFunc(dateDownFunc);
		_menu->setUpFunc(dateUpFunc);
		_menu->setDrawFunc(NULL);
		
		break;
	}

	// Display the new date
	dateSettingMenu.showDateStr();
}


//--------------------------------------------------------------
// Method creates the date string to me used for the menu string
//--------------------------------------------------------------
void DateSettingsMenu::makeDateStr(char* buff)
{
	char month[4] = {0};
	strcpy_P(month, shortMonths[DateSettingsMenu::_dateDataSet.Month]);
	sprintf_P(buff, PSTR("%1s%02u %s %02u"), "", DateSettingsMenu::_dateDataSet.Day, month, DateSettingsMenu::_dateDataSet.Year);
}

void DateSettingsMenu::showDateStr()
{
	showDateStr(-1, 0); // No invert
}

//--------------------------------------------------------------
// Method creates the date menu option using the date passed in.
//--------------------------------------------------------------
void DateSettingsMenu::showDateStr(int16_t invert_start, int16_t invert_length)
{
	char buff[21];
	makeDateStr(buff);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_DATE_DATE_INDEX, invert_start, invert_length, buff, NULL, selectDate);
}

//----------------------------------------
// Method handles down button pressed when
// editing the date and time strings
//----------------------------------------
void DateSettingsMenu::dateDownFunc()
{
	_menu->upOption();
}

//----------------------------------------
// Method handles up button pressed when
// editing the date and time strings
//----------------------------------------
void DateSettingsMenu::dateUpFunc()
{
	_menu->downOption();
}

//----------------------------------------------------------------
// Method to save the time when the Save menu option is selected.
// Changes the menu option to "Saved".
//----------------------------------------------------------------
void DateSettingsMenu::saveDateFunc()
{
	// Write the time.
	MyDS3232.write(DateSettingsMenu::_dateDataSet);

	// Change the option text to Saved
	_menu->createOption(MENU_MAIN_INDEX, OPTION_DATE_SAVE_INDEX, PSTR("Saved"), NULL, DateSettingsMenu::saveDateFunc); // Position 3
}

