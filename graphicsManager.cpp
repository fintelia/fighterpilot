
#include "engine.h"


GraphicsManager::GraphicsManager(): currentId(0), currentView(0), currentGamma(1.0), stereo(false), leftEye(true), interOcularDistance(0.0)
{

}
GraphicsManager::View::View(): mRenderParticles(true)
{

}
Vec2f GraphicsManager::View::project(Vec3f p)
{
	p -= mCamera.eye;

	Vec3f f = mCamera.fwd;
	Vec3f UP = mCamera.up;

	Vec3f s = (f.cross(UP)).normalize();
	Vec3f u = (s.cross(f)).normalize();

	float F = 1.0/tan((mProjection.fovy*PI/180.0) / 2.0);

	Vec3f v = Vec3f(s.dot(p)*F/mProjection.aspect,   -u.dot(p)*F,   f.dot(p)*(mProjection.zNear+mProjection.zFar)-2.0*mProjection.zNear*mProjection.zFar) / (f.dot(p));

	return Vec2f(mViewport.x + mViewport.width * (v.x + 1.0) / 2.0, mViewport.y + mViewport.height * (v.y + 1.0) / 2.0);
}
Vec3f GraphicsManager::View::project3(Vec3f p)
{
	p -= mCamera.eye;

	Vec3f f = mCamera.fwd;
	Vec3f UP = mCamera.up;

	Vec3f s = (f.cross(UP)).normalize();
	Vec3f u = (s.cross(f)).normalize();

	float F = 1.0/tan((mProjection.fovy*PI/180.0) / 2.0);

	Vec3f v = Vec3f(s.dot(p)*F/mProjection.aspect,   -u.dot(p)*F,   f.dot(p)*(mProjection.zNear+mProjection.zFar)-2.0*mProjection.zNear*mProjection.zFar) / (f.dot(p));

	return Vec3f(mViewport.x + mViewport.width * (v.x + 1.0) / 2.0, mViewport.y + mViewport.height * (v.y + 1.0) / 2.0, (v.z + 1.0) / 2.0);
}
Vec3f GraphicsManager::View::unProject(Vec3f p)// from x=0 to sAspect && y=0 to 1 && z from 0 to 1
{
	p.y = 1.0-p.y; //we have to cheat since the 2D and 3D coordinate systems used are reversed

	Mat4f A = mProjectionMat * mModelViewMat;
	Mat4f inv;

	if(!A.inverse(inv))
		return Vec3f();//inverse could not be calculated

	return inv * Vec3f(2.0*(p.x - mViewport.x)/mViewport.width - 1.0, 2.0 * (p.y - mViewport.y)/mViewport.height - 1.0, p.z*2.0 - 1.0);
}
void GraphicsManager::View::viewport(float x,float y,float width,float height)
{
	mViewport.x = x;
	mViewport.y = y;
	mViewport.width = width;
	mViewport.height = height;
}
void GraphicsManager::View::perspective(float fovy, float aspect, float zNear, float zFar)
{
	mProjection.type = View::Projection::PERSPECTIVE;
	mProjection.fovy = fovy;
	mProjection.aspect = aspect;
	mProjection.zNear = zNear;
	mProjection.zFar = zFar;

	float f = 1.0 / tan(fovy*PI/180 / 2.0);
	mProjectionMat.set(	f/aspect,	0.0,		0.0,						0.0,
						0.0,		f,			0.0,						0.0,
						0.0,		0.0,		(zFar+zNear)/(zNear-zFar),	2.0*zFar*zNear/(zNear-zFar),
						0.0,		0.0,		-1.0,						0.0);

}
void GraphicsManager::View::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mProjection.type = View::Projection::ORTHOGRAPHIC;
	mProjection.left = left;
	mProjection.right = right;
	mProjection.bottom = bottom;
	mProjection.top = top;
	mProjection.zNear = zNear;
	mProjection.zFar = zFar;

	mProjectionMat.set(	2.0/(right-left),	0.0,				0.0,				(left+right)/(left-right),
						0.0,				2.0/(top-bottom),	0.0,				(bottom+top)/(bottom-top),
						0.0,				0.0,				2.0/(zNear-zFar),	(zNear+zFar)/(zNear-zFar),
						0.0,				0.0,				0.0,				1.0);
}
void GraphicsManager::View::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	//if(stereo)			//stereo disabled for now
	//{
	//	Vec3f f = (center - eye).normalize();
	//	Vec3f r = (up.normalize()).cross(f);
	//	if(leftEye)
	//	{
	//		eye += r * interOcularDistance * 0.5;
	//		center += r * interOcularDistance * 0.5;
	//	}
	//	else
	//	{
	//		eye -= r * interOcularDistance * 0.5;
	//		center -= r * interOcularDistance * 0.5;
	//	}
	//}


	up = up.normalize();

	Vec3f f = (center-eye).normalize();
	Vec3f s = (f.cross(up)).normalize();
	Vec3f u = s.cross(f);

	mModelViewMat.set(	s.x,	s.y,	s.z,	s.dot(-eye),
						u.x,	u.y,	u.z,	u.dot(-eye),
						-f.x,	-f.y,	-f.z,	(-f).dot(-eye),
						0.0,	0.0,	0.0,	1.0);

	mCamera.eye = eye;

	mCamera.fwd = f;
	mCamera.up = u;
	mCamera.right = f.cross(u);

	Mat4f mvp = mProjectionMat * mModelViewMat;

	mClipPlanes[0] = Plane<float>( mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8], mvp[15]+mvp[12] );	// Left clipping plane
	mClipPlanes[1] = Plane<float>( mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8], mvp[15]-mvp[12] );	// Right clipping plane
	mClipPlanes[2] = Plane<float>( mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9], mvp[15]+mvp[13] );	// Bottom clipping plane
	mClipPlanes[3] = Plane<float>( mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9], mvp[15]-mvp[13] );	// Top clipping plane
	mClipPlanes[4] = Plane<float>( mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14] );// Near clipping plane
	mClipPlanes[5] = Plane<float>( mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14] );// Far clipping plane
 }
bool GraphicsManager::View::sphereInFrustum(Sphere<float> s)
{
	float distance;
	for(int i = 0; i < 6; i++)
	{
		distance = mClipPlanes[i].distance(s.center);
		if(distance < -s.radius)
			return false;
		//if(distance < s.radius)		<--sphere intersects frustum
		//	return false;
	}
	return true;
}

bool GraphicsManager::View::boundingBoxInFrustum(BoundingBox<float> b)
{
	for(int p = 0; p < 6; ++p) 
	{
		if(mClipPlanes[p].normal.x * b.minXYZ.x    +    mClipPlanes[p].normal.y * b.minXYZ.y    +    mClipPlanes[p].normal.z * b.minXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.minXYZ.x    +    mClipPlanes[p].normal.y * b.minXYZ.y    +    mClipPlanes[p].normal.z * b.maxXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.minXYZ.x    +    mClipPlanes[p].normal.y * b.maxXYZ.y    +    mClipPlanes[p].normal.z * b.minXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.minXYZ.x    +    mClipPlanes[p].normal.y * b.maxXYZ.y    +    mClipPlanes[p].normal.z * b.maxXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.maxXYZ.x    +    mClipPlanes[p].normal.y * b.minXYZ.y    +    mClipPlanes[p].normal.z * b.minXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.maxXYZ.x    +    mClipPlanes[p].normal.y * b.minXYZ.y    +    mClipPlanes[p].normal.z * b.maxXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.maxXYZ.x    +    mClipPlanes[p].normal.y * b.maxXYZ.y    +    mClipPlanes[p].normal.z * b.minXYZ.z > -mClipPlanes[p].d) continue;
		if(mClipPlanes[p].normal.x * b.maxXYZ.x    +    mClipPlanes[p].normal.y * b.maxXYZ.y    +    mClipPlanes[p].normal.z * b.maxXYZ.z > -mClipPlanes[p].d) continue;

		return false;
	}
	return true;
}
//
//Vec2f GraphicsManager::project(Vec3f p, unsigned int view)
//{
//	if(view >= views.size())
//	{
//		debugBreak();
//		return Vec2f();
//	}
//
//	p -= views[view].camera.eye;
//
//	Vec3f f = views[view].camera.fwd;
//	Vec3f UP = views[view].camera.up;
//
//	Vec3f s = (f.cross(UP)).normalize();
//	Vec3f u = (s.cross(f)).normalize();
//
//	float F = 1.0/tan((views[view].projection.fovy*PI/180.0) / 2.0);
//
//	Vec3f v = Vec3f(s.dot(p)*F/views[view].projection.aspect,   -u.dot(p)*F,   f.dot(p)*(views[view].projection.zNear+views[view].projection.zFar)-2.0*views[view].projection.zNear*views[view].projection.zFar) / (f.dot(p));
//
//	return Vec2f(views[view].viewport.x + views[view].viewport.width * (v.x + 1.0) / 2.0, views[view].viewport.y + views[view].viewport.height * (v.y + 1.0) / 2.0);
//}
//Vec3f GraphicsManager::unProject(Vec3f p, unsigned int view)// from x=0 to sAspect && y=0 to 1 && z from 0 to 1
//{
//	if(view >= views.size())
//	{
//		debugBreak();
//		return Vec3f();
//	}
//
//	p.y = 1.0-p.y; //we have to cheat since the 2D and 3D coordinate systems used are reversed
//
//	Mat4d A = views[view].projectionMat * views[view].modelViewMat;
//	Mat4d inv;
//
//	if(!A.inverse(inv))
//		return Vec3f();//inverse could not be calculated
//
//	Vec3d r = inv * Vec3d(2.0*(p.x - views[view].viewport.x)/views[view].viewport.width - 1.0, 2.0 * (p.y - views[view].viewport.y)/views[view].viewport.height - 1.0, p.z*2.0 - 1.0);
//
//	return Vec3f(r.x,r.y,r.z);
//}
//void GraphicsManager::viewport(float x,float y,float width,float height, unsigned int view)
//{
//	if(view >= views.size())
//	{
//		debugBreak();
//		return;
//	}
//
//	views[view].viewport.x = x;
//	views[view].viewport.y = y;
//	views[view].viewport.width = width;
//	views[view].viewport.height = height;
//}
//void GraphicsManager::perspective(float fovy, float aspect, float zNear, float zFar, unsigned int view)
//{
//	if(view >= views.size())
//	{
//		debugBreak();
//		return;
//	}
//
//	views[view].projection.type = View::Projection::PERSPECTIVE;
//	views[view].projection.fovy = fovy;
//	views[view].projection.aspect = aspect;
//	views[view].projection.zNear = zNear;
//	views[view].projection.zFar = zFar;
//
//	float f = 1.0 / tan(fovy*PI/180 / 2.0);
//	views[view].projectionMat.set(	f/aspect,	0.0,		0.0,						0.0,
//									0.0,		f,			0.0,						0.0,
//									0.0,		0.0,		(zFar+zNear)/(zNear-zFar),	2*zFar*zNear/(zNear-zFar),
//									0.0,		0.0,		-1.0,						0.0);
//
//}
//void GraphicsManager::ortho(float left, float right, float bottom, float top, float zNear, float zFar, unsigned int view)
//{
//	if(view >= views.size())
//	{
//		debugBreak();
//		return;
//	}
//
//	views[view].projection.type = View::Projection::ORTHOGRAPHIC;
//	views[view].projection.left = left;
//	views[view].projection.right = right;
//	views[view].projection.bottom = bottom;
//	views[view].projection.top = top;
//	views[view].projection.zNear = zNear;
//	views[view].projection.zFar = zFar;
//
//	views[view].projectionMat.set(	2.0/(right-left),	0.0,				0.0,				(left+right)/(left-right),
//									0.0,				2.0/(top-bottom),	0.0,				(bottom+top)/(bottom-top),
//									0.0,				0.0,				2.0/(zNear-zFar),	(zNear+zFar)/(zNear-zFar),
//									0.0,				0.0,				0.0,				1.0);
//}
//void GraphicsManager::lookAt(Vec3f eye, Vec3f center, Vec3f up, unsigned int view)
//{
//	//if(stereo)			//stereo disabled for now
//	//{
//	//	Vec3f f = (center - eye).normalize();
//	//	Vec3f r = (up.normalize()).cross(f);
//	//	if(leftEye)
//	//	{
//	//		eye += r * interOcularDistance * 0.5;
//	//		center += r * interOcularDistance * 0.5;
//	//	}
//	//	else
//	//	{
//	//		eye -= r * interOcularDistance * 0.5;
//	//		center -= r * interOcularDistance * 0.5;
//	//	}
//	//}
//
//
//	up = up.normalize();
//
//	Vec3f f = (center-eye).normalize();
//	Vec3f s = (f.cross(up)).normalize();
//	Vec3f u = s.cross(f);
//
//	views[view].modelViewMat.set(	s.x,	s.y,	s.z,	s.dot(-eye),
//											u.x,	u.y,	u.z,	u.dot(-eye),
//											-f.x,	-f.y,	-f.z,	(-f).dot(-eye),
//											0.0,	0.0,	0.0,	1.0);
//
//	views[view].camera.eye = eye;
//
//	views[view].camera.fwd = f;
//	views[view].camera.up = u;
//	views[view].camera.right = f.cross(u);
//
//	Mat4f mvp = views[view].projectionMat * views[view].modelViewMat;
//
//	views[view].clipPlanes[0] = Plane<float>( mvp[3]-mvp[0], mvp[7]-mvp[4], mvp[11]-mvp[8], mvp[15]-mvp[12] );	// Right clipping plane
//	views[view].clipPlanes[1] = Plane<float>( mvp[3]+mvp[0], mvp[7]+mvp[4], mvp[11]+mvp[8], mvp[15]+mvp[12] );	// Left clipping plane
//	views[view].clipPlanes[2] = Plane<float>( mvp[3]+mvp[1], mvp[7]+mvp[5], mvp[11]+mvp[9], mvp[15]+mvp[13] );	// Bottom clipping plane
//	views[view].clipPlanes[3] = Plane<float>( mvp[3]-mvp[1], mvp[7]-mvp[5], mvp[11]-mvp[9], mvp[15]-mvp[13] );	// Top clipping plane
//	views[view].clipPlanes[4] = Plane<float>( mvp[3]-mvp[2], mvp[7]-mvp[6], mvp[11]-mvp[10], mvp[15]-mvp[14] );	// Far clipping plane
//	views[view].clipPlanes[5] = Plane<float>( mvp[3]+mvp[2], mvp[7]+mvp[6], mvp[11]+mvp[10], mvp[15]+mvp[14] );	// Near clipping plane
//}
//bool GraphicsManager::sphereInFrustum(Sphere<float> s)
//{
//	float distance;
//	for(int i = 0; i < 6; i++)
//	{
//		distance = views[currentView].clipPlanes[i].distance(s.center);
//		if(distance < -s.radius)
//			return false;
//		//if(distance < s.radius)		<--sphere intersects frustum
//		//	return false;
//	}
//	return true;
//}
shared_ptr<GraphicsManager::View> GraphicsManager::genView()
{
	shared_ptr<View> v(new View);
	
	views_new.push_back(weak_ptr<View>(v));

	return v;
}
//void GraphicsManager::resetViews(unsigned int numViews)
//{
//	View v;
//	views.clear();
//	for(int i=0; i<numViews || i==0; i++)
//	{
//		views.push_back(v);
//		viewport(0,0,sAspect,1.0, i);
//		ortho(0,sAspect,0,1.0, i);
//	}
//}
