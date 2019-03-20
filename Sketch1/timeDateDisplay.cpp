#include <stdint.h>
#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

#include "defs.h"
#include "stringUtils.h"
#include "timeDateDisplay.h"
#include "GlobalSettings.h"
#include "icons.h"

extern DS3232RTC ds3232RTC;
extern boolean battCharging;

TimeDateDisplay::TimeDateDisplay(Adafruit_SharpMem& display) : _display(display), _font(NULL)
{
}

void TimeDateDisplay::setFont(const GFXfont* font)
{
	_font = font;	
}

void TimeDateDisplay::setDateFont(const GFXfont* font)
{
	_dateFont = font;
}

void TimeDateDisplay::setTempFont(const GFXfont* font)
{
	_tempFont = font;
}

void TimeDateDisplay::setTempDegreeFont(const GFXfont* font)
{
	_tempDegreeFont = font;
}

void TimeDateDisplay::setTempType(eTempConversion type)
{
	_tempType = type;
}

void TimeDateDisplay::displayLongDate(tmElements_t currTime)
{
	_display.setTextSize(1);
	_display.setFont(_dateFont);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);

	char timeBuff[32] = {0};
	sprintf_P(timeBuff, PSTR("%s %u, %u"), months[currTime.Month], currTime.Day, currTime.Year + 2000);

	_display.setCursor(64, 60);
	stringUtils::printCenterString(_display, GlobalSettings::_inverted, timeBuff);
}

void TimeDateDisplay::displayTime(tmElements_t currTime)
{
	_display.setTextSize(1);
	_display.setFont(_font);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);
	
	char timeBuff[6] = {0};
#ifdef EVERY_SECOND
	sprintf_P(timeBuff, PSTR("%02u:%02u"), currTime.Hour, currTime.Second);
#endif
#ifdef EVERY_MINUTE
	sprintf_P(timeBuff, PSTR("%02u:%02u"), currTime.Hour, currTime.Minute);
#endif

	_display.setCursor(10, 49);
	_display.print(timeBuff);
}

void TimeDateDisplay::displayDateTime(tmElements_t currTime)
{
	// Clear down entire screen
	_display.fillRect(0, 0, _display.width(), _display.height() / 2, GlobalSettings::_inverted ? BLACK : WHITE);

	displayTime(currTime);
	displayLongDate(currTime);
}

void TimeDateDisplay::displayLocalTemp(float temp)
{
	_display.setTextSize(1);
	_display.setFont(_tempFont);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);

	_display.setCursor(75, 8);
	char tempBuff[8] = {0};
	sprintf_P(tempBuff, PSTR("%d.%02d"), (int)temp, (int)(temp * 100.0) % 100);
	_display.print(tempBuff);

	_display.setFont(_tempDegreeFont);
	_display.print((char)0xB0);

	_display.setFont(_tempFont);
	_display.print((_tempType == fahrenheit) ? 'F' : 'C');
}


void TimeDateDisplay::displayTemp(float temp)
{
//	displayLocalTemp((_tempType == fahrenheit) ? CtoF(temp) : temp);
}

boolean TimeDateDisplay::isUSBPluggedIn()
{
	int status = analogRead(POWER_SENSE);
	return (status > 100);
}


void TimeDateDisplay::displayBatteryLevel(float voltage)
{
	_display.setTextSize(1);
	_display.setFont(_tempFont);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);

	_display.setCursor(5, 8);
	char tempBuff[8] = {0};
	sprintf_P(tempBuff, PSTR("%d.%02d"), (int)voltage, (int)(voltage * 100.0) % 100);
	_display.print(tempBuff);

	const uint8_t *bitmap = battery_1qBitmaps;
	// See if we are charging and change the icon
	if (/*battCharging &&*/ isUSBPluggedIn())
	{
		bitmap = battery_chrgingBitmaps;
	}
	else
	{
		// Display Icon
		if (voltage >= 3.7)
			bitmap = battery_4qBitmaps;
		else if (voltage >= 3.5)
			bitmap = battery_3qBitmaps;
		else if (voltage >= 3.3)
			bitmap = battery_2qBitmaps;
	}
	
	_display.drawBitmap(35, 2, bitmap, battery_4qWidthPixels, battery_4qHeightPixels, GlobalSettings::_inverted ? WHITE : BLACK);

#ifdef DISPLAY_HR_MODE
	_display.setTextSize(1);
	_display.setFont(_tempFont);
	_display.setTextColor(GlobalSettings::_inverted ? WHITE: BLACK, GlobalSettings::_inverted ? BLACK : WHITE);

	float batStat = analogRead(BATTERY_STATUS);
	batStat *= 3.3;  // Multiply by 3.3V, our reference voltage
	batStat /= 1024; // convert to voltage

	int status = analogRead(POWER_SENSE);
	status *= 3.3;  // Multiply by 3.3V, our reference voltage
	status /= 1024; // convert to voltage

	_display.setCursor(50, 8);
	tempBuff[8] = {0};
//	eHR1224 hr1224 = (ds3232RTC.readRTC(RTC_HOURS) & _BV(HR1224)) ? eHR1224::HR12 : eHR1224::HR24; // Hr
	sprintf_P(tempBuff, PSTR("%d.%02d, %d"),  (int)batStat, (int)(batStat * 100.0) % 100, isUSBPluggedIn());
	_display.print(tempBuff);
#endif

	
}
