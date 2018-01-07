#ifndef _DEFS_H
#define _DEFS_H


#define SCK 19
#define MOSI 18
#define SS 23
#define EXTMODE 28


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

#define MENU_MAIN_INDEX     0
#define MENU_SUB_INDEX      1
#define MENU_SUB_SUB_INDEX  2
#define MENU_CHANNELS_INDEX 3

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