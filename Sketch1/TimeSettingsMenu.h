/*
 * timeMenu.h
 *
 * Created: 1/31/2018 6:37:14 PM
 *  Author: dell
 */ 


#ifndef TIMEMENU_H_
#define TIMEMENU_H_

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Watch_Menu.h>
#include "defs.h"

class TimeSettingsMenu
{
public:
	void showTimeStr();
	void showTimeStr(int16_t invert_start, int16_t invert_length);
	void show1224HrStr();
	void show1224HrStr(int16_t invert_start, int16_t invert_length);

	static void timeFunc();
	static void timeDownFunc();
	static void timeUpFunc();
	static void saveTimeFunc();
	static void timeBack();
	static void select1224hr();
	static void hr1224Draw();
	static void timeDraw();
	static void timeDataUp();
	static void timeDataDown();
	static void selectTime();
	
	static WatchMenu* _menu;
	static tmElements_t _timeDataSet;
	static eHR1224 _hr1224;
	static eAMPM _amPm;



};

#endif /* TIMEMENU_H_ */