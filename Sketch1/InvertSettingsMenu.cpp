
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

#include "MainMenu.h"
#include "InvertSettingsMenu.h"
#include "GlobalSettings.h"


InvertSettingsMenu invertSettingsMenu;

//------------------------------------------------------------
//------------------------------------------------------------
void InvertSettingsMenu::invertFunc()
{
	GlobalSettings::_inverted = !GlobalSettings::_inverted;
	MainMenu::_currentMenu->invertDisplay(GlobalSettings::_inverted);
}
