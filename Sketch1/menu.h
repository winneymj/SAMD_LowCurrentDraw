#ifndef _MENU_H
#define _MENU_H

#include <stdint.h>
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define NUM_MENUS             3
#define MENU_MAIN_INDEX       0
#define MENU_DATETIME_INDEX   1
#define MENU_SETTINGS_INDEX   2

#define OPT_DATE_TIME_INDEX   0
#define OPT_SETTINGS_INDEX    1

#define OPT_DATE_INDEX        0
#define OPT_TIME_INDEX        1
#define OPT_DATE_TIME_EXIT_INDEX 2

#define OPT_EXIT_INDEX        2

// Settings menu options
#define OPT_INVERT_INDEX        0
#define OPT_TEMPERATURE_INDEX   1
#define OPT_CALENDAR_INDEX      2
#define OPT_SETTINGS_EXIT_INDEX 3

// Calendar setting menus
#define OPTION_CALENDAR_GRID_INDEX      0
#define OPTION_CALENDAR_STARTDAY_INDEX  1
#define OPTION_CALENDAR_SAVE_INDEX      2
#define OPTION_CALENDAR_EXIT_INDEX      3
#define SETTING_CALENDAR_NOW_NONE       0
#define SETTING_CALENDAR_GRID_ONOFF     1
#define SETTING_CALENDAR_STARTDAY       2


#define INACTIVITY 30000

extern Adafruit_SharpMem display;
extern const GFXfont courier_10x15FontInfo;
extern volatile boolean rtcRead;
extern WatchMenu *currentMenu;
extern volatile uint8_t pinValM;
extern volatile uint8_t pinValD;
extern volatile uint8_t pinValU;
extern pFunc drawFunc;
extern void displayTime();
extern bool invert;


// Forward Declarations
void timeFunc();
void dateFunc();
void invertFunc();
void calendarFunc();
void switchTemp();
void exitMenu();
void menuUpFunc();
void menuDownFunc();
void calendarDownFunc();
void calendarUpFunc();
void calendarBack();
void showCalendarGridOptions();
void showCalendarGridOptions(int16_t invert_start, int16_t invert_length);
void showCalendarStartDayOptions();
void showCalendarStartDayOptions(int16_t invert_start, int16_t invert_length);





#endif