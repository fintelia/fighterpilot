
#include "main.h"

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

	return Vec2f( (v.x + 1.0) / 2.0 * sAspect, 1.0 - (v.y + 1.0) / 2.0);
}
Vec3f GraphicsManager::unProject(Vec3f p)// from x=0 to sAspect && y=0 to 1 && z from 0 to 1
{
	matrix4x4<double> A = view.projectionMat * view.modelViewMat;
	matrix4x4<double> inv;

	if(!A.inverse(inv))
		return Vec3f();//inverse could not be calculated

	Vec3d r = inv * Vec3d(p.x*2.0/sAspect - 1.0, p.y*2.0 - 1.0, p.z*2.0 - 1.0);

	return Vec3f(r.x,r.y,r.z);
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
