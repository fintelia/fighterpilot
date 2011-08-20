
#include "main.h"

void ModeManager::destroyMode(modeScreen* Mode)
{
	if(Mode != NULL)
	{
		delete Mode;
		Mode = NULL;
	}
}
bool ModeManager::init()
{
	modeScreen *mode = NULL;

	// register all the menus, each new menu
	// must call register before it can be used
	//registerMode("menuLevelEditor", &CreateObject<menuLevelEditor>);
	//registerMode("menuInGame", &CreateObject<menuInGame>);
	//registerMode("menuChooseMode", &CreateObject<menuChooseMode>);

	return true;
}
void ModeManager::render2D()
{
	if(mode != NULL)
		mode->draw2D();
}
void ModeManager::render3D()
{
	if(mode != NULL)
		mode->draw3D();
}
void ModeManager::renderParticles()
{
	if(mode != NULL)
		mode->drawParticles();
}
int ModeManager::update()
{
	if(mode != NULL)
		return mode->update();
	return 0;
}
void ModeManager::shutdown()
{
	if(mode != NULL)
		delete mode;
}
bool ModeManager::setMode(modeScreen* newMode)
{
	if(mode != NULL) delete mode;
	mode = NULL;

	if(newMode==NULL)	
		return true;

	mode = newMode;

	// initialize the menu
	if (!mode->init())
	{
		// shutdown the menu system
		shutdown();
		return false;
	}
	return true;
}