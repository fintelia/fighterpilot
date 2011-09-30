
#include "engine.h"
GraphicsManager::GraphicsManager(): currentId(0),hDC(NULL),hRC(NULL),hWnd(NULL), stereo(false), leftEye(true), interOcularDistance(0.0), currentView(0)
{
	resetViews(1);
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
Vec2f GraphicsManager::project(Vec3f p, unsigned int view)
{
	if(view >= views.size())
	{
		debugBreak();
		return Vec2f();
	}

	p -= views[view].camera.eye;

	Vec3f f = views[view].camera.fwd;
	Vec3f UP = views[view].camera.up;

	Vec3f s = (f.cross(UP)).normalize();
	Vec3f u = (s.cross(f)).normalize();
	
	float F = 1.0/tan((views[view].projection.fovy*PI/180.0) / 2.0);

	Vec3f v = Vec3f(s.dot(p)*F/views[view].projection.aspect,   -u.dot(p)*F,   f.dot(p)*(views[view].projection.zNear+views[view].projection.zFar)-2.0*views[view].projection.zNear*views[view].projection.zFar) / (f.dot(p));

	return Vec2f(views[view].viewport.x + views[view].viewport.width * (v.x + 1.0) / 2.0, views[view].viewport.y + views[view].viewport.height * (v.y + 1.0) / 2.0);
}
Vec3f GraphicsManager::unProject(Vec3f p, unsigned int view)// from x=0 to sAspect && y=0 to 1 && z from 0 to 1
{
	if(view >= views.size())
	{
		debugBreak();
		return Vec3f();
	}

	matrix4x4<double> A = views[view].projectionMat * views[view].modelViewMat;
	matrix4x4<double> inv;

	if(!A.inverse(inv))
		return Vec3f();//inverse could not be calculated

	Vec3d r = inv * Vec3d(2.0*(p.x - views[view].viewport.x)/views[view].viewport.width - 1.0, 2.0 * (p.y - views[view].viewport.y)/views[view].viewport.height - 1.0, p.z*2.0 - 1.0);

	return Vec3f(r.x,r.y,r.z);
}
void GraphicsManager::viewport(float x,float y,float width,float height, unsigned int view)
{
	if(view >= views.size())
	{
		debugBreak();
		return;
	}

	views[view].viewport.x = x;
	views[view].viewport.y = y;
	views[view].viewport.width = width;
	views[view].viewport.height = height;
}
void GraphicsManager::perspective(float fovy, float aspect, float zNear, float zFar, unsigned int view)
{
	if(view >= views.size())
	{
		debugBreak();
		return;
	}

	views[view].projection.type = View::Projection::PERSPECTIVE;
	views[view].projection.fovy = fovy;
	views[view].projection.aspect = aspect;
	views[view].projection.zNear = zNear;
	views[view].projection.zFar = zFar;

	float f = 1.0 / tan(fovy*PI/180 / 2.0);
	views[view].projectionMat.set(	f/aspect,	0.0,		0.0,						0.0,
									0.0,		f,			0.0,						0.0,
									0.0,		0.0,		(zFar+zNear)/(zNear-zFar),	2*zFar*zNear/(zNear-zFar),
									0.0,		0.0,		-1.0,						0.0);

}
void GraphicsManager::ortho(float left, float right, float bottom, float top, float zNear, float zFar, unsigned int view)
{
	if(view >= views.size())
	{
		debugBreak();
		return;
	}

	views[view].projection.type = View::Projection::ORTHOGRAPHIC;
	views[view].projection.left = left;
	views[view].projection.right = right;
	views[view].projection.bottom = bottom;
	views[view].projection.top = top;
	views[view].projection.zNear = zNear;
	views[view].projection.zFar = zFar;

	views[view].projectionMat.set(	2.0/(right-left),	0.0,				0.0,				(left+right)/(left-right),
									0.0,				2.0/(top-bottom),	0.0,				(bottom+top)/(bottom-top),
									0.0,				0.0,				2.0/(zNear-zFar),	(zNear+zFar)/(zNear-zFar),
									0.0,				0.0,				0.0,				1.0);
}
void GraphicsManager::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	up = up.normalize();

	Vec3f f = (center-eye).normalize();
	Vec3f s = (f.cross(up)).normalize();
	Vec3f u = s.cross(f);

	views[currentView].modelViewMat.set(	s.x,	s.y,	s.z,	s.dot(-eye),
											u.x,	u.y,	u.z,	u.dot(-eye),
											-f.x,	-f.y,	-f.z,	(-f).dot(-eye),
											0.0,	0.0,	0.0,	1.0);

	views[currentView].camera.eye = eye;

	views[currentView].camera.fwd = f;
	views[currentView].camera.up = u;
	views[currentView].camera.right = f.cross(u);

	matrix4x4<float> mvp = views[currentView].projectionMat * views[currentView].modelViewMat;

	views[currentView].clipPlanes[0] = Plane<float>( mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8], mvp[15]-mvp[12] );	// Right clipping plane
	views[currentView].clipPlanes[1] = Plane<float>( mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8], mvp[15]+mvp[12] );	// Left clipping plane
	views[currentView].clipPlanes[2] = Plane<float>( mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9], mvp[15]+mvp[13] );	// Bottom clipping plane
	views[currentView].clipPlanes[3] = Plane<float>( mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9], mvp[15]-mvp[13] );	// Top clipping plane
	views[currentView].clipPlanes[4] = Plane<float>( mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14] );	// Far clipping plane
	views[currentView].clipPlanes[5] = Plane<float>( mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14] );	// Near clipping plane
}
bool GraphicsManager::sphereInFrustum(Sphere<float> s)
{
	float distance;
	for(int i = 0; i < 6; i++)
	{
		distance = views[currentView].clipPlanes[i].distance(s.center);
		if(distance < -s.radius)
			return false;
		//if(distance < s.radius)		<--sphere intersects frustum
		//	return false;
	}
	return true;
}
void GraphicsManager::resetViews(unsigned int numViews)
{
	View v;
	views.clear();
	for(int i=0; i<numViews || i==0; i++)
	{
		views.push_back(v);
		viewport(0,0,sAspect,1.0, i);
		ortho(0,sAspect,0,1.0, i);
	}
}