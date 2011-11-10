
#include "engine.h"

objId object::currentId = 0;

void object::draw()
{
	if(dataManager.assetLoaded(objectTypeString(type)))
	{
		graphics->drawModel(type, position, rotation);
	}
}