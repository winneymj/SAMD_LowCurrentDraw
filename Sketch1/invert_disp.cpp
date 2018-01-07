#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>

//extern WatchMenu *currentMenu;

bool invert = false;

//------------------------------------------------------------
//------------------------------------------------------------
void invertFunc()
{
	invert = !invert;
//	currentMenu->invertDisplay(invert);
}
