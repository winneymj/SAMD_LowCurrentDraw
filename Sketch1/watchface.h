/*
 * watchface.h
 *
 * Created: 1/7/2018 4:28:13 PM
 *  Author: dell
 */ 

#ifndef WATCHFACE_H_
#define WATCHFACE_H_

class Adafruit_SharpMem;

class WatchFace
{
public:	
	WatchFace(Adafruit_SharpMem& display, DS3232RTC& ds3232RTC);
	void displayTime();
	void invert(bool invert);

private:
	Adafruit_SharpMem& _display;
	DS3232RTC& _ds3232RTC;
	bool _invert;
};




#endif /* WATCHFACE_H_ */