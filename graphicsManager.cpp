
#include "engine.h"

GraphicsManager::shader* GraphicsManager::shader::boundShader = nullptr;

GraphicsManager::GraphicsManager(): stereoMode(STEREO_NONE), leftEye(true), interOcularDistance(0.0), currentView(0), currentGamma(1.0), highResScreenshot(false)
{
	viewConstraint = Rect::XYXY(0,0,1,1);
}
GraphicsManager::View::View(): mViewConstraint(0,0,1,1)
{

}
Vec2f GraphicsManager::View::project(Vec3f p)
{
	//p -= mCamera.eye;
	//
	//Vec3f f = mCamera.fwd;
	//Vec3f UP = mCamera.up;
	//
	//Vec3f s = (f.cross(UP)).normalize();
	//Vec3f u = (s.cross(f)).normalize();
	//
	//float F = 1.0/tan((mProjection.fovy*PI/180.0) / 2.0);
	//
	//Vec3f v = Vec3f(s.dot(p)*F/mProjection.aspect,   -u.dot(p)*F,   f.dot(p)*(mProjection.zNear+mProjection.zFar)-2.0*mProjection.zNear*mProjection.zFar) / (f.dot(p));
	//
	//return Vec2f(mViewport.x + mViewport.width * (v.x + 1.0) / 2.0, mViewport.y + mViewport.height * (v.y + 1.0) / 2.0);
	Vec3f projected = mProjectionMat * mModelViewMat * p;
	Vec2f r =  Vec2f(mViewport.x + mViewport.width*(projected.x*0.5+0.5), mViewport.y + mViewport.height*(0.5-projected.y*0.5));

	return r;
}
Vec3f GraphicsManager::View::project3(Vec3f p) //z=0: far plane, z=1: near plane, z>1: behind camera
{
	Vec3f projected = mProjectionMat * mModelViewMat * p;
	return Vec3f(mViewport.x + mViewport.width*(projected.x*0.5+0.5), mViewport.y + mViewport.height*(0.5-projected.y*0.5), (projected.z+1.0)/2.0);
}
Vec3f GraphicsManager::View::unProject(Vec3f p)// from x=0 to sAspect && y=0 to 1 && z from 0 to 1
{
	Mat4f A = mProjectionMat * mModelViewMat;
	Mat4f inv;

	if(!A.inverse(inv))
		return Vec3f();//inverse could not be calculated

	return inv * Vec3f(2.0*(p.x - mViewport.x)/mViewport.width - 1.0, 2.0 * (1.0 - mViewport.y - p.y/mViewport.height) - 1.0, p.z*2.0 - 1.0);
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
	mProjectionMat.set(	f/aspect,	0.0,	0.0,						0.0,
						0.0,		f,		0.0,						0.0,
						0.0,		0.0,	(zFar+zNear)/(zNear-zFar),	2.0*zFar*zNear/(zNear-zFar),
						0.0,		0.0,	-1.0,						0.0);

	completeProjectionMat = mProjectionMat;
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

	completeProjectionMat = mProjectionMat;
}
void GraphicsManager::View::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
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
	Mat4f combinedMat = mProjectionMat * mModelViewMat;

	Vec3f points[8] =  {combinedMat * Vec3f(b.minXYZ.x, b.minXYZ.y, b.minXYZ.z),
						combinedMat * Vec3f(b.minXYZ.x, b.minXYZ.y, b.maxXYZ.z),
						combinedMat * Vec3f(b.minXYZ.x, b.maxXYZ.y, b.minXYZ.z),
						combinedMat * Vec3f(b.minXYZ.x, b.maxXYZ.y, b.maxXYZ.z),

						combinedMat * Vec3f(b.maxXYZ.x, b.minXYZ.y, b.minXYZ.z),
						combinedMat * Vec3f(b.maxXYZ.x, b.minXYZ.y, b.maxXYZ.z),
						combinedMat * Vec3f(b.maxXYZ.x, b.maxXYZ.y, b.minXYZ.z),
						combinedMat * Vec3f(b.maxXYZ.x, b.maxXYZ.y, b.maxXYZ.z)};

	bool result = !((points[0].x < -1.0 && points[1].x < -1.0 && points[2].x < -1.0 && points[3].x < -1.0 && points[4].x < -1.0 && points[5].x < -1.0 && points[6].x < -1.0 && points[7].x < -1.0) ||
					(points[0].y < -1.0 && points[1].y < -1.0 && points[2].y < -1.0 && points[3].y < -1.0 && points[4].y < -1.0 && points[5].y < -1.0 && points[6].y < -1.0 && points[7].y < -1.0) ||
					(points[0].z < -1.0 && points[1].z < -1.0 && points[2].z < -1.0 && points[3].z < -1.0 && points[4].z < -1.0 && points[5].z < -1.0 && points[6].z < -1.0 && points[7].z < -1.0) ||
		
					(points[0].x > 1.0 && points[1].x > 1.0 && points[2].x > 1.0 && points[3].x > 1.0 && points[4].x > 1.0 && points[5].x > 1.0 && points[6].x > 1.0 && points[7].x > 1.0) ||
					(points[0].y > 1.0 && points[1].y > 1.0 && points[2].y > 1.0 && points[3].y > 1.0 && points[4].y > 1.0 && points[5].y > 1.0 && points[6].y > 1.0 && points[7].y > 1.0) ||
					(points[0].z > 1.0 && points[1].z > 1.0 && points[2].z > 1.0 && points[3].z > 1.0 && points[4].z > 1.0 && points[5].z > 1.0 && points[6].z > 1.0 && points[7].z > 1.0));


	bool result2 = true;
	for(int p = 0; p < 6; ++p)
	{
		if(	(mClipPlanes[p].normal.x * b.minXYZ.x + mClipPlanes[p].normal.y * b.minXYZ.y + mClipPlanes[p].normal.z * b.minXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.minXYZ.x + mClipPlanes[p].normal.y * b.minXYZ.y + mClipPlanes[p].normal.z * b.maxXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.minXYZ.x + mClipPlanes[p].normal.y * b.maxXYZ.y + mClipPlanes[p].normal.z * b.minXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.minXYZ.x + mClipPlanes[p].normal.y * b.maxXYZ.y + mClipPlanes[p].normal.z * b.maxXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.maxXYZ.x + mClipPlanes[p].normal.y * b.minXYZ.y + mClipPlanes[p].normal.z * b.minXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.maxXYZ.x + mClipPlanes[p].normal.y * b.minXYZ.y + mClipPlanes[p].normal.z * b.maxXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.maxXYZ.x + mClipPlanes[p].normal.y * b.maxXYZ.y + mClipPlanes[p].normal.z * b.minXYZ.z < -mClipPlanes[p].d) &&
			(mClipPlanes[p].normal.x * b.maxXYZ.x + mClipPlanes[p].normal.y * b.maxXYZ.y + mClipPlanes[p].normal.z * b.maxXYZ.z < -mClipPlanes[p].d))
		{
			result2 = false;
			break;
		}
	}

	return result || result2; //since neither method always works, return whether either believes that AABB to in inside the frustum
}
/**
 * Returns the (approximate) projected area of the bounding box. The units are (screen heights)^2. 
 * This means that on a 16x9 (wide screen) display, a return value of 1.0 indicates 
 * that the box takes up 9/16 of the entire screen.
 */
float GraphicsManager::View::boundingBoxProjectedArea(BoundingBox<float> b)
{
	Vec2f p[8] = { project(Vec3f(b.minXYZ.x, b.minXYZ.y, b.minXYZ.z)),
		project(Vec3f(b.minXYZ.x, b.minXYZ.y, b.maxXYZ.z)),
		project(Vec3f(b.minXYZ.x, b.maxXYZ.y, b.minXYZ.z)),
		project(Vec3f(b.minXYZ.x, b.maxXYZ.y, b.maxXYZ.z)),

		project(Vec3f(b.maxXYZ.x, b.minXYZ.y, b.minXYZ.z)),
		project(Vec3f(b.maxXYZ.x, b.minXYZ.y, b.maxXYZ.z)),
		project(Vec3f(b.maxXYZ.x, b.maxXYZ.y, b.minXYZ.z)),
		project(Vec3f(b.maxXYZ.x, b.maxXYZ.y, b.maxXYZ.z)) };

	Vec2f minXY = p[0], maxXY = p[0];
	for (int i = 1; i < 8; i++){
		minXY.x = min(minXY.x, p[i].x);
		minXY.y = min(minXY.y, p[i].y);
		maxXY.x = max(maxXY.x, p[i].x);
		maxXY.y = max(maxXY.y, p[i].y);
	}

	minXY.x = max(minXY.x, 0.0f);
	minXY.y = max(minXY.y, 0.0f);
	maxXY.x = min(maxXY.x, sAspect);
	maxXY.y = min(maxXY.y, 1.0f);

	return max((maxXY.x - minXY.x) * (maxXY.y - minXY.y), 0.0f);
}
void GraphicsManager::View::shiftCamera(Vec3f shift)
{
	Vec3f diff = Quat4f(mCamera.fwd) * (shift - cameraShift);
	cameraShift = shift;
	lookAt(mCamera.eye + diff, mCamera.eye + diff + mCamera.fwd, mCamera.up);
}
void GraphicsManager::View::constrainView(Rect4f bounds)
{
	mViewConstraint = bounds;
	if(mProjection.type == Projection::PERSPECTIVE)
	{
		float zNear		= mProjection.zNear;
		float zFar		= mProjection.zFar;

		float top		= zNear * tan((mProjection.fovy*PI/180) / 2.0);
		float bottom	= -top;
		float left		= mProjection.aspect * bottom;
		float right		= mProjection.aspect * top;

		float height = top - bottom;
		float width = right - left;

		top		= bottom + height * (bounds.y + bounds.h);
		bottom	= bottom + height * bounds.y;

		right	= left + width * (bounds.x + bounds.w);
		left	= left + width * bounds.x;

		//float A = (right + left) / (right - left);
		//float B = (top + bottom) / (top - bottom);
		//float C = (zNear + zFar) / (zNear - zFar);
		//float D = 2.0 * zNear * zFar / (zNear - zFar);

		mProjectionMat.set(	2.0*zNear/(right-left),	0.0,						(right + left) / (right - left),		0.0,
							0.0,					2.0*zNear/(top-bottom),		(top + bottom) / (top - bottom),		0.0,
							0.0,					0.0,						(zNear + zFar) / (zNear - zFar),		2.0 * zNear * zFar / (zNear - zFar),
							0.0,					0.0,						-1.0,									0.0);

	}
	else
	{
		debugBreak(); // code not written!
	}
}
shared_ptr<GraphicsManager::View> GraphicsManager::genView()
{
	shared_ptr<View> v(new View);
	
	views.push_back(weak_ptr<View>(v));

	return v;
}
void GraphicsManager::vertexBuffer::addVertexAttribute(VertexAttribute attrib, unsigned int offset)
{
	vertexAttributeData attribData;
	attribData.offset = offset;
	vertexAttributes[attrib] = attribData;
}
GraphicsManager::shader* GraphicsManager::getBoundShader()
{
	return shader::boundShader;
}
float GraphicsManager::getGamma()const
{
	return currentGamma;
}
Vec3f GraphicsManager::getLightPosition()const
{
	return lightPosition;
}
bool GraphicsManager::getVSync()const
{
	return vSync;
}
Rect GraphicsManager::getViewContraint()const
{
	return viewConstraint;
}
bool GraphicsManager::isHighResScreenshot()const
{
	return highResScreenshot;
}
void GraphicsManager::setColor(float r, float g, float b, float a)
{
	setColor(Color4(r,g,b,a));
}
void GraphicsManager::setColor(float r, float g, float b)
{
	setColor(Color4(r,g,b,1.0));
}
void GraphicsManager::setLightPosition(Vec3f position)
{
	lightPosition = position;
}
void GraphicsManager::setStereoMode(StereoMode s)
{
	stereoMode = s;
}
void GraphicsManager::setInterOcularDistance(float d)
{
	interOcularDistance = d;
}
bool GraphicsManager::drawOverlay(Rect4f r, string tex)
{
	return drawOverlay(r, tex != "" ? dataManager.getTexture(tex) : nullptr);
}
bool GraphicsManager::drawRotatedOverlay(Rect4f r, Angle rotation, string tex)
{
	return drawRotatedOverlay(r, rotation, tex != "" ? dataManager.getTexture(tex) : nullptr);
}
bool GraphicsManager::drawPartialOverlay(Rect4f r, Rect4f t, string tex)
{
	return drawPartialOverlay(r, t,  tex != "" ? dataManager.getTexture(tex) : nullptr);
}
