
#include "main.h"

int humanControl::TotalPlayers=1;

object* objectController::getObject()
{
	return world[mObjectNum].get();
}

void AIcontrol::update()
{
	if(getObject()->type & PLANE)
	{	
		// ...
	}
}