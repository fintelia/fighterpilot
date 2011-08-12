
#include "main.h"

bool GraphicsManager::drawObject(gID obj)
{
	if(objects.find(obj) !=objects.end())
	{
		objects[obj]->drawFlag = true;
		return true;
	}
	return false;
}
void GraphicsManager::render3D()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
		if(i->second->drawFlag && !i->second->transparent)	i->second->render();
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
		if(i->second->drawFlag && i->second->transparent)	i->second->render();
}

void GraphicsManager::reset()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		i->second->reset(); 
	}
}
void GraphicsManager::flashTaskBar(int times, int length)
{
	FLASHWINFO f;
	f.cbSize = sizeof(FLASHWINFO);
	f.hwnd = hWnd;
	f.dwFlags = FLASHW_TRAY;
	f.dwTimeout = length;
	f.uCount = times;
	FlashWindowEx(&f);
}
void GraphicsManager::minimizeWindow()
{
	ShowWindow(hWnd, SW_MINIMIZE);
}

Vec2f GraphicsManager::project(Vec3f p)
{
	p -= view.camera.eye;

	Vec3f f = (view.camera.center - view.camera.eye).normalize();
	Vec3f UP = (view.camera.up).normalize();

	Vec3f s = (f.cross(UP)).normalize();
	Vec3f u = (s.cross(f)).normalize();
	
	float F = 1.0/tan((view.projection.fovy*PI/180.0) / 2.0);

	Vec3f v = Vec3f(s.dot(p)*F/view.projection.aspect,   -u.dot(p)*F,   f.dot(p)*(view.projection.zNear+view.projection.zFar)-2.0*view.projection.zNear*view.projection.zFar) / (f.dot(p));

	return Vec2f( (v.x + 1.0) / 2.0, (v.y + 1.0) / 2.0 );
}
Vec3f GraphicsManager::unProject(Vec3f p)// from x=0 to 1 && y=0 to 1 && z from 0 to 1
{
	float alpha = view.projection.fovy * 0.5 * (p.y-0.5)*2 * PI/180;
	float beta = view.projection.fovy * 0.5 * (p.x-0.5)*2  * PI/180;

	Vec3f up = view.camera.up;
	Vec3f center = (view.camera.center - view.camera.eye).normalize();
	Vec3f right = up.cross(center);

	float d = (view.projection.zNear + (view.projection.zFar-view.projection.zNear)*p.z);

	Profiler.setOutput("p.x",(p.x*2-1.0));
	Profiler.setOutput("p.y",(p.y*2-1.0));

	Vec3f r = view.camera.eye + (center + up * tan(alpha) + right * tan(beta) * view.projection.aspect) * d;

	Vec2f pj = project(r);

	Profiler.setOutput("pj.x",((1.0-pj.x)*2-1.0));
	Profiler.setOutput("pj.y",((1.0-pj.y)*2-1.0));

	Profiler.setOutput("pj/p.x",((1.0-pj.x)*2-1.0)/(p.x*2-1.0));
	Profiler.setOutput("pj/p.y",((1.0-pj.y)*2-1.0)/(p.y*2-1.0));

	return r;


	//p = p-eye;
	//Angle alpha = acosA( (p.normalize()).dot(view.camera.center.normalize()) );
	//Angle beta = acosA( (p.normalize()).dot(view.camera.up.normalize()) );



	//matrix4x4<float> PM = view.modelViewMat * view.projectionMat;
	//view.projection.
}
void GraphicsManager::viewport(int x,int y,int width,int height)
{
	view.viewport.x = x;
	view.viewport.y = y;
	view.viewport.width = width;
	view.viewport.height = height;
}
void GraphicsManager::perspective(float fovy, float aspect, float zNear, float zFar)
{
	view.projection.type = View::Projection::PERSPECTIVE;
	view.projection.fovy = fovy;
	view.projection.aspect = aspect;
	view.projection.zNear = zNear;
	view.projection.zFar = zFar;

	float f = 1.0 / tan(fovy*PI/180 / 2.0);
	view.projectionMat.set(	f/aspect,	0.0,		0.0,						0.0,
							0.0,		f,			0.0,						0.0,
							0.0,		0.0,		(zFar+zNear)/(zNear-zFar),	2*zFar*zNear/(zNear-zFar),
							0.0,		0.0,		-1.0,						0.0);

}
void GraphicsManager::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	view.projection.type = View::Projection::ORTHOGRAPHIC;
	view.projection.left = left;
	view.projection.right = right;
	view.projection.bottom = bottom;
	view.projection.top = top;
	view.projection.zNear = zNear;
	view.projection.zFar = zFar;

	view.projectionMat.set(	2.0/(right-left),	0.0,				0.0,				(left+right)/(left-right),
							0.0,				2.0/(top-bottom),	0.0,				(bottom+top)/(bottom-top),
							0.0,				0.0,				2.0/(zNear-zFar),	(zNear+zFar)/(zNear-zFar),
							0.0,				0.0,				0.0,				1.0);

}
void GraphicsManager::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f f = (center-eye).normalize();
	Vec3f s = f.cross(up.normalize());
	Vec3f u = s.cross(f);

	view.modelViewMat.set(	s.x,	s.y,	s.z,	s.dot(-eye),
							u.x,	u.y,	u.z,	u.dot(-eye),
							-f.x,	-f.y,	-f.z,	(-f).dot(-eye),
							0.0,	0.0,	0.0,	1.0);

	view.camera.eye = eye;
	view.camera.center = center;
	view.camera.up = u;
}
