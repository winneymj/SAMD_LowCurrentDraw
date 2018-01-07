#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <RTCx.h>         // https://github.com/stevemarple/RTCx

//#include "defs.h"
//#include "datetime.h"
//#include "cour8pt7b.h"
//
//extern Adafruit_SharpMem display;
//extern WatchMenu *currentMenu;
//extern DS3232RTC MyDS3232;
//extern WatchMenu menu;
//extern bool invert;
//
//s_menuNowSetting setting;
//WatchMenu dateMenu(display);
//tmElements_t dateDataSet = {0};
//
////------------------------------------------------------------
//// Method is called when the date/time menu option is selected.
//// It creates a new String type menu (as opposed to Icon).
////------------------------------------------------------------
//void dateFunc()
//{
	//tmElements_t currTime;
	//MyDS3232.read(currTime);
//
	//// Create copy of current time & date
	//memcpy(&dateDataSet, &currTime, sizeof(tmElements_t));
//
	//dateMenu.initMenu(1);  // Create a menu system with ? menu rows
	//dateMenu.setTextSize(1);
	//dateMenu.setFont(&cour8pt7b);
	//dateMenu.createMenu(MENU_MAIN_INDEX, 3, PSTR("<DATE>"), MENU_TYPE_STR, dateDownFunc, dateUpFunc);
	//dateMenu.createOption(MENU_MAIN_INDEX, OPTION_DATE_SAVE_INDEX, PSTR("Save"), NULL, saveDateFunc);
	//dateMenu.createOption(MENU_MAIN_INDEX, OPTION_DATE_EXIT_INDEX, PSTR("Exit"), NULL, dateBack);
	//dateMenu.invertDisplay(invert);
//
	//showDateStr();
//
	//// Default to date selected
	//dateMenu.selectedOption(MENU_MAIN_INDEX, OPTION_DATE_DATE_INDEX); // Set the default selection to the date
	//
	//// Point to date/time menu
	//currentMenu = &dateMenu;
//
	//display.fillRect(0, 64, 128, 128, invert ? BLACK : WHITE); // Clear bottom of display
//}
//
//void dateBack()
//{
	//// Point to top level menu
	//currentMenu = &menu;
//}
//
////--------------------------------------------------------------
//// Method draws the selection highlight when changing the date.
//// The character highlight is reversed so it can be seen
////--------------------------------------------------------------
//void dateDraw()
//{
	//#ifndef SLEEP_PROCESSOR
	////Serial.println("dateDraw(): Enter");
	//#endif
	//int16_t invert_start = -1;
	//int16_t invert_length = 0;
//
	//switch(setting.now)
	//{
		//case SETTING_NOW_DAY10:
		//invert_start = 1;
		//invert_length = 1;
		//dateDataSet.Day = (setting.val * 10) + (dateDataSet.Day % 10);
		//break;
		//case SETTING_NOW_DAY1:
		//invert_start = 2;
		//invert_length = 1;
		//dateDataSet.Day = ((dateDataSet.Day / 10) * 10) + setting.val;
		//break;
		//case SETTING_NOW_MONTH:
		//invert_start = 4;
		//invert_length = 3;
		//dateDataSet.Month = setting.val;
		//break;
		//case SETTING_NOW_YEAR10:
		//invert_start = 8;
		//invert_length = 1;
		//dateDataSet.Year = (setting.val * 10) + dateDataSet.Year % 10;
		//break;
		//case SETTING_NOW_YEAR1:
		//invert_start = 9;
		//invert_length = 1;
		//dateDataSet.Year = ((dateDataSet.Year / 10) * 10) + setting.val;
		//break;
		//default:
		//// Remove inversion
		//invert_start = -1;
		//invert_length = 0;
	//}
	//
	//showDateStr(invert_start, invert_length);
//}
//
//byte getMaxValForSetting()
//{
	//byte max;
	//switch(setting.now)
	//{
		//case SETTING_NOW_DAY10:
		//max = 3;
		//break;
		//case SETTING_NOW_DAY1:
		//max = 9;
		//break;
		//case SETTING_NOW_MONTH:
		//max = 11;
		//break;
		//case SETTING_NOW_YEAR10:
		//max = 9;
		//break;
		//case SETTING_NOW_YEAR1:
		//max = 9;
		//break;
		//case SETTING_NOW_12HR:
		//case SETTING_NOW_24HR:
		//max = 1;
		//case SETTING_NOW_AMHR:
		//case SETTING_NOW_PMHR:
		//max = 1;
		//break;
		//default:
		//max = 9;
		//break;
	//}
	//return max;
//}
//
////------------------------------
//// Move the menu up on key press
////------------------------------
//void dateDataUp()
//{
	//setting.val++;
	//if(setting.val > getMaxValForSetting())
	//setting.val = 0;
//}
//
////--------------------------------
//// Move the menu down on key press
////--------------------------------
//void dateDataDown()
//{
	//setting.val--;
	//byte max = getMaxValForSetting();
	//if(setting.val > max)
	//setting.val = max;
//}
//
////----------------------------------------------------
//// Method is called when the Middle button (select) is
//// pressed on the date menu.
////----------------------------------------------------
//void selectDate()
//{
	//// Set the up and down buttons, and drawing routine to new functions
	//dateMenu.setDownFunc(dateDataUp);
	//dateMenu.setUpFunc(dateDataDown);
	//dateMenu.setDrawFunc(dateDraw);
//
	//switch(setting.now)
	//{
		//case SETTING_NOW_NONE:
		//setting.now = SETTING_NOW_DAY10;
		//setting.val = dateDataSet.Day / 10;
		//break;
		//case SETTING_NOW_DAY10:
		//{
			//byte mod = dateDataSet.Day % 10;
			//dateDataSet.Day = (setting.val * 10) + mod;
			//setting.now = SETTING_NOW_DAY1;
			//setting.val = mod;
		//}
		//break;
		//case SETTING_NOW_DAY1:
		//dateDataSet.Day = ((dateDataSet.Day / 10) * 10) + setting.val;
		//if(dateDataSet.Day < 1)
		//dateDataSet.Day = 1;
		//else if(dateDataSet.Day > 31)
		//dateDataSet.Day = 31;
		//setting.now = SETTING_NOW_MONTH;
		//setting.val = dateDataSet.Month;
		//break;
		//case SETTING_NOW_MONTH:
		//{
			//dateDataSet.Month = setting.val;
			//byte maxDays = 31;
			//byte mnth = dateDataSet.Month;
			//if(mnth == 3 || mnth == 5 || mnth == 8 || mnth == 10)
			//maxDays = 30;
			//else if(mnth == 1)
			//maxDays = LEAP_YEAR(dateDataSet.Year) ? 29 : 28;
			//if(dateDataSet.Day > maxDays)
			//dateDataSet.Day = maxDays;
			//setting.now = SETTING_NOW_YEAR10;
			//setting.val = dateDataSet.Year / 10;
		//}
		//break;
		//case SETTING_NOW_YEAR10:
		//{
			//byte mod = dateDataSet.Year % 10;
			//dateDataSet.Year = (setting.val * 10) + mod;
			//setting.now = SETTING_NOW_YEAR1;
			//setting.val = mod;
		//}
		//break;
		//default:
		//dateDataSet.Year = ((dateDataSet.Year / 10) * 10) + setting.val;
		//dateDataSet.Wday = time_dow(dateDataSet.Year + 2000, dateDataSet.Month + 1, dateDataSet.Day);
		//setting.now = SETTING_NOW_NONE;
//
		//// Go back to menu after finishing the editing of the date.
		//// TODO - Find a nicer way to do this................
		//dateMenu.setDownFunc(dateDownFunc);
		//dateMenu.setUpFunc(dateUpFunc);
		//dateMenu.setDrawFunc(NULL);
		//
		//break;
	//}
//
	//// Display the new date
	//showDateStr();
//}
//
//
////--------------------------------------------------------------
//// Method creates the date string to me used for the menu string
////--------------------------------------------------------------
//void makeDateStr(char* buff)
//{
	//char month[4] = {0};
	//strcpy_P(month, shortMonths[dateDataSet.Month]);
	//sprintf_P(buff, PSTR("%1s%02u %s %02u"), "", dateDataSet.Day, month, dateDataSet.Year);
//}
//
//void showDateStr()
//{
	//showDateStr(-1, 0); // No invert
//}
//
////--------------------------------------------------------------
//// Method creates the date menu option using the date passed in.
////--------------------------------------------------------------
//void showDateStr(int16_t invert_start, int16_t invert_length)
//{
	//char buff[21];
	//makeDateStr(buff);
	//dateMenu.createOption(MENU_MAIN_INDEX, OPTION_DATE_DATE_INDEX, invert_start, invert_length, buff, NULL, selectDate);
//}
//
////----------------------------------------
//// Method handles down button pressed when
//// editing the date and time strings
////----------------------------------------
//void dateDownFunc()
//{
	//dateMenu.upOption();
//}
//
////----------------------------------------
//// Method handles up button pressed when
//// editing the date and time strings
////----------------------------------------
//void dateUpFunc()
//{
	//dateMenu.downOption();
//}

//----------------------------------------------------------------
// Method to save the time when the Save menu option is selected.
// Changes the menu option to "Saved".
//----------------------------------------------------------------
void saveDateFunc()
{
	//// Write the time.
	//MyDS3232.write(dateDataSet);
//
	//// Change the option text to Saved
	//dateMenu.createOption(MENU_MAIN_INDEX, OPTION_DATE_SAVE_INDEX, PSTR("Saved"), NULL, saveTimeFunc); // Position 3
}

