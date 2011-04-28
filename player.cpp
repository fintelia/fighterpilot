
#include "main.h"

int humanControl::TotalPlayers=1;

object* objectController::getObject()
{
	return world.objectList[mObjectNum];
}

void AIcontrol::update()
{
	if(getObject()->type & AA_GUN)
	{	
		contState.up=0.0;
		contState.down=0.0;
		contState.left=0.0;
		contState.right=0.0;
		contState.shoot1 = 1.0;

		nPlane* closestPlane = NULL;
		double cPlaneDist = 100000.0 * 100000.0;
		for(auto i = world.planes().begin(); i != world.planes().end();i++)
		{
			double iDist = getObject()->position.distanceSquared(i->second->position);
			if(iDist < cPlaneDist)
			{
				closestPlane = i->second;
				cPlaneDist = iDist;
				break;
			}
		}
		if(closestPlane != NULL)
		{
			target = closestPlane->id;

			Vec3f dir = (closestPlane->position - getObject()->position).normalize();	
			Vec2f dirXZ(dir.x,dir.z);
			dirXZ = dirXZ.normalize();
			Angle dirA = atan2A(dirXZ.y,dirXZ.x);

			Vec3f tar = Vec3f(cos(((aaGun*)getObject())->angle),sin(((aaGun*)getObject())->elevation),sin(((aaGun*)getObject())->angle)).normalize();
			Angle tarA = ((aaGun*)getObject())->angle;

			if(dir.y > tar.y)		contState.up = min(30.0*(dir.y - tar.y), 1.0);
			else if(dir.y < tar.y)	contState.down = min(30.0*(tar.y - dir.y), 1.0);

			//if(dir.y < 0.9999999)
			{
				if(dirA.inRange(tarA,tarA+PI))		contState.left = min(15.0*abs(dirA - tarA), 1.0);
				else								contState.right = min(15.0*abs(dirA - tarA), 1.0);
			}
			
			contState.shoot1 = 1.0;
		}
		else
		{
			target = 0;
		}
	}
}