#ifndef _DEFS_H
#define _DEFS_H


#define DISPLAY_SCK 19
#define DISPLAY_MOSI 18
#define DISPLAY_SS 23
#define DISPLAY_EXTMODE 28

// Charging = 0; Charged = 1
// PA05 (Pin 6)
#define BATTERY_STATUS 5
#define POWER_SENSE 7
#define BATTERY_LEVEL 2

// Button pins
#define LED 6
#define MBUT 4
#define UBUT 14
#define DBUT 22
#define RTC_INT 9

#define BLACK 0
#define WHITE 1
#define INVERSE 2

//#define EVERY_SECOND
#define EVERY_MINUTE
#define SLEEP_PROCESSOR

#define DISPLAY_HR_MODE

#define MENU_MAIN_INDEX     0
#define MENU_SUB_INDEX      1
#define MENU_SUB_SUB_INDEX  2
#define MENU_CHANNELS_INDEX 3

// DATE/TIME menu settings
#define SETTING_NOW_NONE  0
#define SETTING_NOW_10HOUR  1
#define SETTING_NOW_1HOUR 2
#define SETTING_NOW_10MIN 3
#define SETTING_NOW_1MIN  4

#define SETTING_NOW_DAY10 5
#define SETTING_NOW_DAY1 6
#define SETTING_NOW_MONTH 7
#define SETTING_NOW_YEAR10 8
#define SETTING_NOW_YEAR1 9

#define SETTING_NOW_12HR  10
#define SETTING_NOW_24HR  11
#define SETTING_NOW_AMHR  12
#define SETTING_NOW_PMHR  13

//#define OPTION_TIME_INDEX 1
//#define OPTION_12HR_INDEX 2
//#define OPTION_SAVE_INDEX 3
//#define OPTION_EXIT_INDEX 4

#define OPTION_DATE_DATE_INDEX 0
#define OPTION_DATE_SAVE_INDEX 1
#define OPTION_DATE_EXIT_INDEX 2

#define OPTION_TIME_TIME_INDEX 0
#define OPTION_TIME_12HR_INDEX 1
#define OPTION_TIME_SAVE_INDEX 2
#define OPTION_TIME_EXIT_INDEX 3

#define AMPM  5 // Bit 5 of Hour byte

enum class eHR1224 { HR12, HR24 };
enum class eAMPM { HRAM, HRPM };

// leap year calculator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

//----------------------------

typedef struct{
	uint8_t now;
	uint8_t val;
}s_menuNowSetting;

const char months[12][10] PROGMEM = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
};

const char shortMonths[12][4] PROGMEM = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

const char dayOfWeek[7][2] PROGMEM = {
	"M",
	"T",
	"W",
	"T",
	"F",
	"S",
	"S"
};

const char dayOfWeekLong[7][4] PROGMEM = {
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};

const uint8_t dayInMonth[12] PROGMEM = {
	31, //Jan
	29, //Feb
	31, //Mar
	30, //Apr
	31, //May
	30, //June
	31, //July
	31, //Aug
	30, //Sept
	31, //Oct
	30, //Nov
	31, //Dec
};

enum eTempConversion { fahrenheit, centigrade };

inline uint8_t time_dow(int y, uint8_t m, uint8_t d)
{
	static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	y -= m < 3;
	uint8_t dow = (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;

	// 0 = Sunday, 1 = Monday, but Monday should be 0
	dow--;
	if(dow == 255) // Overflowed, must have been 0 which is Sunday
	dow = 6;
	return dow;
}

inline float CtoF(float temp)
{
	return (temp * 1.8) + 32;
}


#endif