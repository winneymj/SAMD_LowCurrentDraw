#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <RTCx.h>         // https://github.com/stevemarple/RTCx

#include "defs.h"
#include "menu.h"
#include "timeMenu.h"
#include "cour8pt7b.h"

extern Adafruit_SharpMem display;
extern DS3232RTC ds3232RTC;
extern bool invert;

TimeMenu timewimeyMenu;
eAMPM amPm;

WatchMenu *TimeMenu::_timeMenu = new WatchMenu(display);
tmElements_t TimeMenu::_timeDataSet = {0};
eHR1224 TimeMenu::_hr1224;


//------------------------------------------------------------
// Method is called when the date/time menu option is selected.
// It creates a new String type menu (as opposed to Icon).
//------------------------------------------------------------
void TimeMenu::timeFunc()
{
	tmElements_t currTime;
	ds3232RTC.read(currTime);

	// Create copy of current time & date
	memcpy(&TimeMenu::_timeDataSet, &currTime, sizeof(tmElements_t));
	_hr1224 = (ds3232RTC.readRTC(RTC_HOURS) & _BV(HR1224)) ? HR12 : HR24; // Hr
	amPm = (ds3232RTC.readRTC(RTC_HOURS) & _BV(AMPM)) ? HRPM : HRAM; // Morning/Afternoon

	_timeMenu->initMenu(1);  // Create a menu system with ? menu rows
	_timeMenu->setTextSize(1);
	_timeMenu->setFont(&cour8pt7b);
	_timeMenu->createMenu(MENU_MAIN_INDEX, 4, PSTR("<DATE/TIME>"), MENU_TYPE_STR, TimeMenu::timeDownFunc, TimeMenu::timeUpFunc);
	_timeMenu->createOption(MENU_MAIN_INDEX, OPTION_TIME_SAVE_INDEX, PSTR("Save"), NULL, TimeMenu::saveTimeFunc);
	_timeMenu->createOption(MENU_MAIN_INDEX, OPTION_TIME_EXIT_INDEX, PSTR("Exit"), NULL, timeBack);
	_timeMenu->invertDisplay(invert);

	timewimeyMenu.showTimeStr();
	timewimeyMenu.show1224HrStr();

	// Default to date selected
	_timeMenu->selectedOption(MENU_MAIN_INDEX, OPTION_TIME_TIME_INDEX); // Set the default selection to the date
	
	// Point to date/time menu
	MainMenu::_currentMenu = _timeMenu;

	display.fillRect(0, 64, 128, 128, invert ? BLACK : WHITE); // Clear display
}

void TimeMenu::timeBack()
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
		max = (TimeMenu::_hr1224 ==  HR12) ? 1 : 2;
		break;
		case SETTING_NOW_1HOUR:
		max = (TimeMenu::_hr1224 ==  HR12) ? 2 : 9;
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
void TimeMenu::timeDataUp()
{
	MainMenu::_setting.val++;
	if(MainMenu::_setting.val > getMaxValForTimeSetting())
	MainMenu::_setting.val = 0;
}

//--------------------------------
// Move the menu down on key press
//--------------------------------
void TimeMenu::timeDataDown()
{
	MainMenu::_setting.val--;
	byte max = getMaxValForTimeSetting();
	if(MainMenu::_setting.val > max)
	MainMenu::_setting.val = max;
}

void TimeMenu::select1224hr()
{
	// Set the up and down buttons, and drawing routine to new functions
	_timeMenu->setDownFunc(TimeMenu::timeDataUp);
	_timeMenu->setUpFunc(TimeMenu::timeDataDown);
	_timeMenu->setDrawFunc(TimeMenu::hr1224Draw);

	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_NONE:
		{
			MainMenu::_setting.val = TimeMenu::_hr1224;
			MainMenu::_setting.now = SETTING_NOW_24HR;
			break;
		}
		default:
		TimeMenu::_hr1224 = (eHR1224)MainMenu::_setting.val;
		MainMenu::_setting.now = SETTING_NOW_NONE;

		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_timeMenu->setDownFunc(TimeMenu::timeDownFunc);
		_timeMenu->setUpFunc(TimeMenu::timeUpFunc);
		_timeMenu->setDrawFunc(NULL);
		break;
	}

	// Update the time mode
	timewimeyMenu.show1224HrStr();
}

void TimeMenu::hr1224Draw()
{
	TimeMenu::_hr1224 = (eHR1224)MainMenu::_setting.val;
	
	timewimeyMenu.show1224HrStr(1, 2);
}

//----------------------------------------------------
// Method is called when the Middle button (select) is
// pressed on the time menu.
//----------------------------------------------------
void TimeMenu::selectTime()
{
	// Set the up and down buttons, and drawing routine to new functions
	_timeMenu->setDownFunc(TimeMenu::timeDataUp);
	_timeMenu->setUpFunc(TimeMenu::timeDataDown);
	_timeMenu->setDrawFunc(TimeMenu::timeDraw);

	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_NONE:
		MainMenu::_setting.now = SETTING_NOW_10HOUR;
		MainMenu::_setting.val = TimeMenu::_timeDataSet.Hour / 10;
		break;
		case SETTING_NOW_10HOUR:
		{
			byte mod = TimeMenu::_timeDataSet.Hour % 10;
			TimeMenu::_timeDataSet.Hour = (MainMenu::_setting.val * 10) + mod;
			MainMenu::_setting.now = SETTING_NOW_1HOUR;
			MainMenu::_setting.val = mod;
		}
		break;
		case SETTING_NOW_1HOUR:
		TimeMenu::_timeDataSet.Hour = ((TimeMenu::_timeDataSet.Hour / 10) * 10) + MainMenu::_setting.val;
		if(TimeMenu::_timeDataSet.Hour > 23)
		TimeMenu::_timeDataSet.Hour = 23;
		MainMenu::_setting.now = SETTING_NOW_10MIN;
		MainMenu::_setting.val = TimeMenu::_timeDataSet.Minute / 10;
		break;
		case SETTING_NOW_10MIN:
		{
			byte mod = TimeMenu::_timeDataSet.Minute % 10;
			TimeMenu::_timeDataSet.Minute = (MainMenu::_setting.val * 10) + mod;
			MainMenu::_setting.now = SETTING_NOW_1MIN;
			MainMenu::_setting.val = mod;
		}
		break;
		case SETTING_NOW_1MIN:
		{
			TimeMenu::_timeDataSet.Minute = ((TimeMenu::_timeDataSet.Minute / 10) * 10) + MainMenu::_setting.val;
			if(TimeMenu::_timeDataSet.Minute > 59)
			{
				TimeMenu::_timeDataSet.Minute = 59;
			}

			// If 12 hr clock then move on to AM/PM selection else end selection.
			if (TimeMenu::_hr1224 == HR12)
			{
				MainMenu::_setting.now = SETTING_NOW_AMHR;
				MainMenu::_setting.val = amPm;
			}
			else
			{
				MainMenu::_setting.now = SETTING_NOW_NONE;
				// Go back to menu after finishing the editing of the date.
				// TODO - Find a nicer way to do this................
				_timeMenu->setDownFunc(TimeMenu::timeDownFunc);
				_timeMenu->setUpFunc(TimeMenu::timeUpFunc);
				_timeMenu->setDrawFunc(NULL);
			}
		}
		break;
		default:
		if (TimeMenu::_hr1224 == HR12)
		{
			amPm = (eAMPM)MainMenu::_setting.val;
		}
		MainMenu::_setting.now = SETTING_NOW_NONE;

		// Go back to menu after finishing the editing of the date.
		// TODO - Find a nicer way to do this................
		_timeMenu->setDownFunc(TimeMenu::timeDownFunc);
		_timeMenu->setUpFunc(TimeMenu::timeUpFunc);
		_timeMenu->setDrawFunc(NULL);
		break;
	}

	// Update the time display
	timewimeyMenu.showTimeStr();
}

//--------------------------------------------------------------
// Method draws the selection highlight when changing the time.
// The character highlight is reversed so it can be seen
//--------------------------------------------------------------
void TimeMenu::timeDraw()
{
	int16_t invert_start = -1;
	int16_t invert_length = 0;

	switch(MainMenu::_setting.now)
	{
		case SETTING_NOW_10HOUR:
		invert_start = 1;
		invert_length = 1;
		TimeMenu::_timeDataSet.Hour = (MainMenu::_setting.val * 10) + TimeMenu::_timeDataSet.Hour % 10;
		break;
		case SETTING_NOW_1HOUR:
		invert_start = 2;
		invert_length = 1;
		TimeMenu::_timeDataSet.Hour = ((TimeMenu::_timeDataSet.Hour / 10) * 10) + MainMenu::_setting.val;
		break;
		case SETTING_NOW_10MIN:
		invert_start = 4;
		invert_length = 1;
		TimeMenu::_timeDataSet.Minute = (MainMenu::_setting.val * 10) + TimeMenu::_timeDataSet.Minute % 10;
		break;
		case SETTING_NOW_1MIN:
		invert_start = 5;
		invert_length = 1;
		TimeMenu::_timeDataSet.Minute = ((TimeMenu::_timeDataSet.Minute / 10) * 10) + MainMenu::_setting.val;
		break;
		case SETTING_NOW_AMHR:
		invert_start = 7;
		invert_length = 2;
		amPm = (eAMPM)MainMenu::_setting.val;
		break;
	}

	timewimeyMenu.showTimeStr(invert_start, invert_length);
}

//--------------------------------------------------------------
// Method creates the time string to me used for the menu string
//--------------------------------------------------------------
void makeTimeStr(char* buff)
{
	switch(TimeMenu::_hr1224)
	{
		case HR12:
		sprintf_P(buff, PSTR("%1s%02u:%02u %2s"), "", TimeMenu::_timeDataSet.Hour, TimeMenu::_timeDataSet.Minute, (amPm == HRAM) ? "AM" : "PM");
		break;
		case HR24:
		sprintf_P(buff, PSTR("%1s%02u:%02u %2s"), "", TimeMenu::_timeDataSet.Hour, TimeMenu::_timeDataSet.Minute, "");
		break;
	}
}

void TimeMenu::showTimeStr()
{
	showTimeStr(-1, 0);
}

//--------------------------------------------------------------
// Method creates the time menu option using the date passed in.
//--------------------------------------------------------------
void TimeMenu::showTimeStr(int16_t invert_start, int16_t invert_length)
{
	char buff[12];
	makeTimeStr(buff);
	TimeMenu::_timeMenu->createOption(MENU_MAIN_INDEX, OPTION_TIME_TIME_INDEX, invert_start, invert_length, buff, NULL, selectTime);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void TimeMenu::show1224HrStr()
{
	show1224HrStr(-1, 0);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void TimeMenu::show1224HrStr(int16_t invert_start, int16_t invert_length)
{
	char buff[7] = {0};
	switch(TimeMenu::_hr1224)
	{
		case HR12:
		strcpy_P(buff, PSTR(" 12 hr"));
		break;
		case HR24:
		strcpy_P(buff, PSTR(" 24 hr"));
		break;
	}
	_timeMenu->createOption(MENU_MAIN_INDEX, OPTION_TIME_12HR_INDEX, invert_start, invert_length, buff, NULL, select1224hr);

	// Redraw the time string also, as we may have changed the 12/24 hr option.
	showTimeStr();
}

//----------------------------------------
// Method handles down button pressed when
// editing the date and time strings
//----------------------------------------
void TimeMenu::timeDownFunc()
{
	_timeMenu->upOption();
}

//----------------------------------------
// Method handles up button pressed when
// editing the date and time strings
//----------------------------------------
void TimeMenu::timeUpFunc()
{
	_timeMenu->downOption();
}

//----------------------------------------------------------------
// Method to save the time when the Save menu option is selected.
// Changes the menu option to "Saved".
//----------------------------------------------------------------
void TimeMenu::saveTimeFunc()
{
	// Write the time.
	ds3232RTC.write(TimeMenu::_timeDataSet);
	// Set the 12/24 hr option.  This is bit 6 of the Hr (0x02), 0=24hr, 1=12hr.
	byte hrRead = ds3232RTC.readRTC(RTC_HOURS); // Hr

	// Set 12 or 24 hr
	switch(TimeMenu::_hr1224)
	{
		case HR12:
		hrRead |= _BV(HR1224); // Set bit 6
		switch(amPm)
		{
			case HRPM:
			hrRead |= _BV(AMPM); // Set bit 5
			break;
			case HRAM:
			hrRead &= ~_BV(AMPM); // Clear bit 5
			break;
		}
		break;
		case HR24:
		hrRead &= ~_BV(HR1224); // Clear bit 6
		break;
	}

	ds3232RTC.writeRTC(RTC_HOURS, hrRead); // Set hrs with AM/PM/24Hr/12Hr

	// Change the option text to Saved
	TimeMenu::_timeMenu->createOption(MENU_MAIN_INDEX, OPTION_TIME_SAVE_INDEX, PSTR("Saved"), NULL, TimeMenu::saveTimeFunc); // Position 3
}

