#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC

#include "defs.h"
#include "MainMenu.h"
#include "TimeSettingsMenu.h"
#include "cour8pt7b.h"

extern Adafruit_SharpMem display;
extern DS3232RTC ds3232RTC;

TimeSettingsMenu timeSettingMenu;

WatchMenu *TimeSettingsMenu::_menu = new WatchMenu(display);
tmElements_t TimeSettingsMenu::_timeDataSet = {0};
eHR1224 TimeSettingsMenu::_hr1224;
eAMPM TimeSettingsMenu::_amPm;

//------------------------------------------------------------
// Method is called when the date/time menu option is selected.
// It creates a new String type menu (as opposed to Icon).
//------------------------------------------------------------
void TimeSettingsMenu::timeFunc()
{
	tmElements_t currTime;
	ds3232RTC.read(currTime);

	// Create copy of current time & date
	memcpy(&TimeSettingsMenu::_timeDataSet, &currTime, sizeof(tmElements_t));
	TimeSettingsMenu::_hr1224 = (ds3232RTC.readRTC(RTC_HOURS) & _BV(HR1224)) ? HR12 : HR24; // Hr
	TimeSettingsMenu::_amPm = (ds3232RTC.readRTC(RTC_HOURS) & _BV(AMPM)) ? HRPM : HRAM; // Morning/Afternoon

	_menu->initMenu(1);  // Create a menu system with ? menu rows
	_menu->setTextSize(1);
	_menu->setFont(&cour8pt7b);
	_menu->createMenu(MENU_MAIN_INDEX, 4, PSTR("<DATE/TIME>"), MENU_TYPE_STR, TimeSettingsMenu::timeDownFunc, TimeSettingsMenu::timeUpFunc);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_TIME_SAVE_INDEX, PSTR("Save"), NULL, TimeSettingsMenu::saveTimeFunc);
	_menu->createOption(MENU_MAIN_INDEX, OPTION_TIME_EXIT_INDEX, PSTR("Exit"), NULL, timeBack);
	_menu->invertDisplay(MainMenu::_inverted);

	timeSettingMenu.showTimeStr();
	timeSettingMenu.show1224HrStr();

	// Default to date selected
	_menu->selectedOption(MENU_MAIN_INDEX, OPTION_TIME_TIME_INDEX); // Set the default selection to the date
	
	// Point to date/time menu
	MainMenu::_currentMenu = _menu;

	display.fillRect(0, 64, 128, 128, MainMenu::_inverted ? BLACK : WHITE); // Clear display
}

void TimeSettingsMenu::timeBack()
{
	// Point to top level menu
	MainMenu::_currentMenu = MainMenu::_menu;
}

byte getMaxValForTimeSetting()
{
	byte max;
	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_10HOUR:
		max = (TimeSettingsMenu::_hr1224 ==  HR12) ? 1 : 2;
		break;
		case SETTING_NOW_1HOUR:
		max = (TimeSettingsMenu::_hr1224 ==  HR12) ? 2 : 9;
		break;
		case SETTING_NOW_10MIN:
		max = 5;
		break;
		case SETTING_NOW_1MIN:
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
void TimeSettingsMenu::timeDataUp()
{
	MainMenu::_setting.val++;
	if(MainMenu::_setting.val > getMaxValForTimeSetting())
	MainMenu::_setting.val = 0;
}

//--------------------------------
// Move the menu down on key press
//--------------------------------
void TimeSettingsMenu::timeDataDown()
{
	MainMenu::_setting.val--;
	byte max = getMaxValForTimeSetting();
	if(MainMenu::_setting.val > max)
	MainMenu::_setting.val = max;
}

void TimeSettingsMenu::select1224hr()
{
	// Set the up and down buttons, and drawing routine to new functions
	_menu->setDownFunc(TimeSettingsMenu::timeDataUp);
	_menu->setUpFunc(TimeSettingsMenu::timeDataDown);
	_menu->setDrawFunc(TimeSettingsMenu::hr1224Draw);

	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_NONE:
		{
			MainMenu::_setting.val = TimeSettingsMenu::_hr1224;
			MainMenu::_setting.now = SETTING_NOW_24HR;
			break;
		}
		default:
		TimeSettingsMenu::_hr1224 = (eHR1224)MainMenu::_setting.val;
		MainMenu::_setting.now = SETTING_NOW_NONE;

		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_menu->setDownFunc(TimeSettingsMenu::timeDownFunc);
		_menu->setUpFunc(TimeSettingsMenu::timeUpFunc);
		_menu->setDrawFunc(NULL);
		break;
	}

	// Update the time mode
	timeSettingMenu.show1224HrStr();
}

void TimeSettingsMenu::hr1224Draw()
{
	TimeSettingsMenu::_hr1224 = (eHR1224)MainMenu::_setting.val;
	
	timeSettingMenu.show1224HrStr(1, 2);
}

//----------------------------------------------------
// Method is called when the Middle button (select) is
// pressed on the time menu.
//----------------------------------------------------
void TimeSettingsMenu::selectTime()
{
	// Set the up and down buttons, and drawing routine to new functions
	_menu->setDownFunc(TimeSettingsMenu::timeDataUp);
	_menu->setUpFunc(TimeSettingsMenu::timeDataDown);
	_menu->setDrawFunc(TimeSettingsMenu::timeDraw);

	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_NONE:
		MainMenu::_setting.now = SETTING_NOW_10HOUR;
		MainMenu::_setting.val = TimeSettingsMenu::_timeDataSet.Hour / 10;
		break;
		case SETTING_NOW_10HOUR:
		{
			byte mod = TimeSettingsMenu::_timeDataSet.Hour % 10;
			TimeSettingsMenu::_timeDataSet.Hour = (MainMenu::_setting.val * 10) + mod;
			MainMenu::_setting.now = SETTING_NOW_1HOUR;
			MainMenu::_setting.val = mod;
		}
		break;
		case SETTING_NOW_1HOUR:
		TimeSettingsMenu::_timeDataSet.Hour = ((TimeSettingsMenu::_timeDataSet.Hour / 10) * 10) + MainMenu::_setting.val;
		if(TimeSettingsMenu::_timeDataSet.Hour > 23)
		TimeSettingsMenu::_timeDataSet.Hour = 23;
		MainMenu::_setting.now = SETTING_NOW_10MIN;
		MainMenu::_setting.val = TimeSettingsMenu::_timeDataSet.Minute / 10;
		break;
		case SETTING_NOW_10MIN:
		{
			byte mod = TimeSettingsMenu::_timeDataSet.Minute % 10;
			TimeSettingsMenu::_timeDataSet.Minute = (MainMenu::_setting.val * 10) + mod;
			MainMenu::_setting.now = SETTING_NOW_1MIN;
			MainMenu::_setting.val = mod;
		}
		break;
		case SETTING_NOW_1MIN:
		{
			TimeSettingsMenu::_timeDataSet.Minute = ((TimeSettingsMenu::_timeDataSet.Minute / 10) * 10) + MainMenu::_setting.val;
			if(TimeSettingsMenu::_timeDataSet.Minute > 59)
			{
				TimeSettingsMenu::_timeDataSet.Minute = 59;
			}

			// If 12 hr clock then move on to AM/PM selection else end selection.
			if (TimeSettingsMenu::_hr1224 == HR12)
			{
				MainMenu::_setting.now = SETTING_NOW_AMHR;
				MainMenu::_setting.val = TimeSettingsMenu::_amPm;
			}
			else
			{
				MainMenu::_setting.now = SETTING_NOW_NONE;
				// Go back to menu after finishing the editing of the date.
				// TODO - Find a nicer way to do this................
				_menu->setDownFunc(TimeSettingsMenu::timeDownFunc);
				_menu->setUpFunc(TimeSettingsMenu::timeUpFunc);
				_menu->setDrawFunc(NULL);
			}
		}
		break;
		default:
		if (TimeSettingsMenu::_hr1224 == HR12)
		{
			TimeSettingsMenu::_amPm = (eAMPM)MainMenu::_setting.val;
		}
		MainMenu::_setting.now = SETTING_NOW_NONE;

		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_menu->setDownFunc(TimeSettingsMenu::timeDownFunc);
		_menu->setUpFunc(TimeSettingsMenu::timeUpFunc);
		_menu->setDrawFunc(NULL);
		break;
	}

	// Update the time display
	timeSettingMenu.showTimeStr();
}

//--------------------------------------------------------------
// Method draws the selection highlight when changing the time.
// The character highlight is reversed so it can be seen
//--------------------------------------------------------------
void TimeSettingsMenu::timeDraw()
{
	int16_t invert_start = -1;
	int16_t invert_length = 0;

	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_10HOUR:
		invert_start = 1;
		invert_length = 1;
		TimeSettingsMenu::_timeDataSet.Hour = (MainMenu::_setting.val * 10) + TimeSettingsMenu::_timeDataSet.Hour % 10;
		break;
		case SETTING_NOW_1HOUR:
		invert_start = 2;
		invert_length = 1;
		TimeSettingsMenu::_timeDataSet.Hour = ((TimeSettingsMenu::_timeDataSet.Hour / 10) * 10) + MainMenu::_setting.val;
		break;
		case SETTING_NOW_10MIN:
		invert_start = 4;
		invert_length = 1;
		TimeSettingsMenu::_timeDataSet.Minute = (MainMenu::_setting.val * 10) + TimeSettingsMenu::_timeDataSet.Minute % 10;
		break;
		case SETTING_NOW_1MIN:
		invert_start = 5;
		invert_length = 1;
		TimeSettingsMenu::_timeDataSet.Minute = ((TimeSettingsMenu::_timeDataSet.Minute / 10) * 10) + MainMenu::_setting.val;
		break;
		case SETTING_NOW_AMHR:
		invert_start = 7;
		invert_length = 2;
		TimeSettingsMenu::_amPm = (eAMPM)MainMenu::_setting.val;
		break;
	}

	timeSettingMenu.showTimeStr(invert_start, invert_length);
}

//--------------------------------------------------------------
// Method creates the time string to me used for the menu string
//--------------------------------------------------------------
void makeTimeStr(char* buff)
{
	switch(TimeSettingsMenu::_hr1224)
	{
		case HR12:
		sprintf_P(buff, PSTR("%1s%02u:%02u %2s"), "", TimeSettingsMenu::_timeDataSet.Hour, TimeSettingsMenu::_timeDataSet.Minute, (TimeSettingsMenu::_amPm == HRAM) ? "AM" : "PM");
		break;
		case HR24:
		sprintf_P(buff, PSTR("%1s%02u:%02u %2s"), "", TimeSettingsMenu::_timeDataSet.Hour, TimeSettingsMenu::_timeDataSet.Minute, "");
		break;
	}
}

void TimeSettingsMenu::showTimeStr()
{
	showTimeStr(-1, 0);
}

//--------------------------------------------------------------
// Method creates the time menu option using the date passed in.
//--------------------------------------------------------------
void TimeSettingsMenu::showTimeStr(int16_t invert_start, int16_t invert_length)
{
	char buff[12];
	makeTimeStr(buff);
	TimeSettingsMenu::_menu->createOption(MENU_MAIN_INDEX, OPTION_TIME_TIME_INDEX, invert_start, invert_length, buff, NULL, selectTime);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void TimeSettingsMenu::show1224HrStr()
{
	show1224HrStr(-1, 0);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void TimeSettingsMenu::show1224HrStr(int16_t invert_start, int16_t invert_length)
{
	char buff[7] = {0};
	switch(TimeSettingsMenu::_hr1224)
	{
		case HR12:
		strcpy_P(buff, PSTR(" 12 hr"));
		break;
		case HR24:
		strcpy_P(buff, PSTR(" 24 hr"));
		break;
	}
	_menu->createOption(MENU_MAIN_INDEX, OPTION_TIME_12HR_INDEX, invert_start, invert_length, buff, NULL, select1224hr);

	// Redraw the time string also, as we may have changed the 12/24 hr option.
	showTimeStr();
}

//----------------------------------------
// Method handles down button pressed when
// editing the date and time strings
//----------------------------------------
void TimeSettingsMenu::timeDownFunc()
{
	_menu->upOption();
}

//----------------------------------------
// Method handles up button pressed when
// editing the date and time strings
//----------------------------------------
void TimeSettingsMenu::timeUpFunc()
{
	_menu->downOption();
}

//----------------------------------------------------------------
// Method to save the time when the Save menu option is selected.
// Changes the menu option to "Saved".
//----------------------------------------------------------------
void TimeSettingsMenu::saveTimeFunc()
{
	// zero out the seconds
	TimeSettingsMenu::_timeDataSet.Second = 0;
	
	// Write the time.
	ds3232RTC.write(TimeSettingsMenu::_timeDataSet);

	volatile uint8_t hrs = ds3232RTC.readRTC(RTC_HOURS);

	// Set the 12/24hr, AM/PM bits in the hour
	// Set 12 or 24 hr
	switch(TimeSettingsMenu::_hr1224)
	{
		case HR12:
		hrs |= _BV(HR1224); // Set bit 6
		switch(TimeSettingsMenu::_amPm)
		{
			case HRPM:
			hrs |= _BV(AMPM); // Set bit 5
			break;
			case HRAM:
			hrs &= ~_BV(AMPM); // Clear bit 5
			break;
		}
		break;
		case HR24:
		hrs &= ~_BV(HR1224); // Clear bit 6
		break;
	}

	ds3232RTC.writeRTC(RTC_HOURS, hrs);

	// Change the option text to Saved
	TimeSettingsMenu::_menu->createOption(MENU_MAIN_INDEX, OPTION_TIME_SAVE_INDEX, PSTR("Saved"), NULL, TimeSettingsMenu::saveTimeFunc); // Position 3
}

