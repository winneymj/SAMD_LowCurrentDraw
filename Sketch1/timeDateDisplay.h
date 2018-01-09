/*
 * timeDateDisplay.h
 *
 * Created: 1/7/2018 9:03:29 AM
 *  Author: dell
 */ 
#ifndef TIMEDATEDISPLAY_H_
#define TIMEDATEDISPLAY_H_

#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include "defs.h"

class Adafruit_SharpMem;

class TimeDateDisplay
{
public:
	TimeDateDisplay(Adafruit_SharpMem& display);
	void displayDateTime(tmElements_t currTime);
	void displayTemp(float temp);
	void setTemp(float temp);
	void setFont(const GFXfont* font);
	void setDateFont(const GFXfont* font);
	void setTempFont(const GFXfont* font);
	void setTempDegreeFont(const GFXfont* font);
	void setTempType(eTempConversion type);
	void invert(bool invert);

private:
	void displayLongDate(tmElements_t currTime);
	void displayTime(tmElements_t currTime);
	void displayLocalTemp(float temp);

	
private:
	bool _invert;
	Adafruit_SharpMem& _display;
	const GFXfont* _font;
	const GFXfont* _dateFont;
	const GFXfont* _tempFont;
	const GFXfont* _tempDegreeFont;
	eTempConversion _tempType;
};

#endif /* TIMEDATEDISPLAY_H_ */