
#include "main.h"

objectPath::objectPath(Vec3f sPos,Quat4f sRot, float tResolution): pointsValid(true), startPoint(sPos,sRot,world.time()), endPoint(sPos,sRot,world.time()),timeResolution(tResolution)
{

}
objectPath::objectPath(Vec3f sPos,Quat4f sRot, float time, float tResolution): pointsValid(true), startPoint(sPos,sRot,time), endPoint(sPos,sRot,time),timeResolution(tResolution)
{

}
void objectPath::addWaypoint(point p)
{
	if(!pointsValid)
	{
		startPoint=p;
		pointsValid=true;
	}
	else if(segments.empty())
	{
		segments.push_back(new linearPath(startPoint,p));
	}
	else
	{
		segments.push_back(new linearPath(segments.back()->getPoint(segments.back()->endTime()),p));
	}
	endPoint=p;
}
void objectPath::addWaypoint(Vec3f sPos,Quat4f sRot)
{
	addWaypoint(point(sPos,sRot,world.time()));
}
void objectPath::addWaypoint(Vec3f sPos,Quat4f sRot, float time)
{
	addWaypoint(point(sPos,sRot,time));
}
objectPath::point objectPath::operator() (float time)
{
	if(!pointsValid)
	{
		debugBreak();
		return point();
	}
	else if(segments.empty())
	{
		return startPoint;
	}
	else if(time >= segments.back()->endTime())
	{
		return point(endPoint.position,endPoint.rotation,time);//should interpolate with segments.back()
	}
	else if(time >= segments.back()->endTime())
	{
		return segments.back()->getPoint(time);
	}
	else if(time <= segments.front()->startTime() + 0.1)
	{
		return segments.front()->getPoint(time);
	}

	for(vector<pathSegment*>::iterator i = segments.begin(); i != segments.end(); i++)
	{
		if(time >= (*i)->startTime() && time <= (*i)->endTime())
			return (*i)->getPoint(time);
	}
		
	debugBreak();
	return point();
}
float objectPath::startTime()
{
	if(!pointsValid)
	{
		return 0;
	}
	else if(segments.empty())
	{
		return startPoint.time;
	}
	else
	{
		return segments.front()->startTime();
	}
}
float objectPath::endTime()
{
	if(!pointsValid)
	{
		return 0;
	}
	else
	{
		return endPoint.time;
	}
}
void objectPath::currentPoint(point p)
{
	if(!pointsValid)
	{
		startPoint=p;
		pointsValid=true;
	}
	else if(segments.empty() && p.time - startPoint.time > timeResolution)
	{
		segments.push_back(new linearPath(startPoint,p));
	}
	else if(!segments.empty() && p.time - segments.back()->endTime() > timeResolution)
	{
		segments.push_back(new linearPath(segments.back()->getPoint(segments.back()->endTime()),p));
	}
	endPoint=p;
}
void objectPath::currentPoint(Vec3f sPos,Quat4f sRot)
{
	currentPoint(point(sPos,sRot,world.time()));
}
void objectPath::currentPoint(Vec3f sPos,Quat4f sRot, float time)
{
	currentPoint(point(sPos,sRot,time));
}
objectPath& operator<<( objectPath& op, const objectPath::point& p ) 
{
	op.addWaypoint(p);
	return op;
}