/*
 * stringUtils.h
 *
 * Created: 1/7/2018 9:35:23 AM
 *  Author: dell
 */ 


#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include "defs.h"

class Adafruit_SharpMem;

class stringUtils
{
public:
	static void printCenterString(Adafruit_SharpMem& display, bool inverted, char *str);

private:
	
	
};



#endif /* STRINGUTILS_H_ */