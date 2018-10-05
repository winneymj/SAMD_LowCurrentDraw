/*
 * DateSettingsMenu.h
 *
 * Created: 2/4/2018 2:48:44 PM
 *  Author: dell
 */ 


#ifndef DATESETTINGSMENU_H_
#define DATESETTINGSMENU_H_

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Watch_Menu.h>
#include "defs.h"

#define SETTING_NOW_DAY10 5
#define SETTING_NOW_DAY1 6
#define SETTING_NOW_MONTH 7
#define SETTING_NOW_YEAR10 8
#define SETTING_NOW_YEAR1 9

class DateSettingsMenu
{
public:
	void showDateStr();
	void showDateStr(int16_t invert_start, int16_t invert_length);
	void makeDateStr(char* buff);


	static void dateFunc();
	static void dateDownFunc();
	static void dateUpFunc();
	static void saveDateFunc();
	static void dateBack();
	static void dateDraw();
	static void selectDate();


public:
	static WatchMenu *_menu;
	static tmElements_t _dateDataSet;
};

#endif /* DATESETTINGSMENU_H_ */