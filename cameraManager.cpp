
#include "main.h"

CameraManager::view::view():eye(0,0,0), center(0,0,-1), up(0,1,0), fovy(80.0*PI/180), zNear(0.1), zFar(10000.0), origin(0.0,0.0), size(sw,sh)
{

}

CameraManager::CameraManager(): currentView(0)
{
	view v;
	views.push_back(v);
}

CameraManager::view& CameraManager::operator() (int v)
{
	if(views.size() > v)	
		return views[v];
	return views[0];
}
CameraManager::view& CameraManager::operator() ()
{
	return views[currentView];
}

unsigned int CameraManager::numViews()
{
	return views.size();
}
void CameraManager::setCurrentView(unsigned int nView)
{
	if(nView < views.size()) 
		currentView=nView;
}