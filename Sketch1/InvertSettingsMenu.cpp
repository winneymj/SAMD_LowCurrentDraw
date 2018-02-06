
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

#include "MainMenu.h"
#include "InvertSettingsMenu.h"


InvertSettingsMenu invertSettingsMenu;

//------------------------------------------------------------
//------------------------------------------------------------
void InvertSettingsMenu::invertFunc()
{
	MainMenu::_inverted = !MainMenu::_inverted;
	MainMenu::_currentMenu->invertDisplay(MainMenu::_inverted);
}
