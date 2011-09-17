
#include "engine.h"

objId object::currentId = 0;

void object::draw()
{
	if(dataManager.assetLoaded(objectTypeString(type)))
	{
		glPushMatrix();
			glTranslatef(position.x,position.y,position.z);
			Angle ang = acosA(rotation.w);
			glRotatef((ang*2.0).degrees(), rotation.x/sin(ang),rotation.y/sin(ang),rotation.z/sin(ang));
		
			graphics->drawModel(type, position, rotation);
		glPopMatrix();
	}
}