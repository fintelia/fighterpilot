

#include "engine.h"

const unsigned char LEFT		= 0x01; //patch to the left is one level above this patch
const unsigned char RIGHT		= 0x02; //patch to the right is one level above this patch
const unsigned char TOP			= 0x04; //patch to the top is one level above this patch
const unsigned char BOTTOM		= 0x08; //patch to the bottom is one level above this patch

const unsigned char SUBDIVIDED	= 0x10;	//patch is divided for rendering
const unsigned char LEVEL_USED	= 0x20; //patch is rendered at this level
const unsigned char COMBINED	= 0x40; //patch is not subdivided this far

TerrainPatch::TerrainPatch(): parent(nullptr), flags(0)
{
	children[0] = nullptr;
	children[1] = nullptr;
	children[2] = nullptr;
	children[3] = nullptr;
	neighbors[0] = nullptr;
	neighbors[1] = nullptr;
	neighbors[2] = nullptr;
	neighbors[3] = nullptr;
}
void TerrainPatch::init(TerrainPatch* trunk, unsigned int levelsDeep, unsigned int totalLevels)
{
	level = levelsDeep;
	if(levelsDeep < totalLevels)
	{
		//flags = flags | CAN_SUBDIVIDE;

		unsigned int cLevelOffset = levelOffset(levelsDeep);
		unsigned int childLevelOffset = levelOffset(levelsDeep+1);

		unsigned int row = (this - trunk - cLevelOffset) >> levelsDeep;
		unsigned int col = (this - trunk - cLevelOffset) % (1 << levelsDeep);

		children[0] = trunk + childLevelOffset + col * 2 + row * 2 * (1 << (levelsDeep+1));
		children[1] = children[0] + 1;

		children[2] = children[0] + (1 << (levelsDeep+1));
		children[3] = children[2] + 1;



		for(int i=0; i<4;i++)
		{
			children[i]->parent = this;
			children[i]->init(trunk, levelsDeep + 1, totalLevels);
		}
	}
	else
	{
		//flags = flags & (~CAN_SUBDIVIDE);
	}
}
TerrainPage::TerrainPage(unsigned short* Heights, unsigned int LevelsDeep, unsigned short patchResolution, Vec3f position, Vec3f scale):minXYZ(position), maxXYZ(position+scale), height((1 << LevelsDeep) * patchResolution + 1), width((1 << LevelsDeep) * patchResolution + 1), levelsDeep(LevelsDeep), heights(Heights), trunk(nullptr)
{
//////////////////////////////////////////////////////VBO and texture//////////////////////////////////////////////////////
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&indexBuffer.id);

	glGenTextures(1,&texture);

	GLfloat* heightMap = new float[width*height*3];
	for(int x = 0; x < width; x++)
	{
		for(int y=0; y < height; y++)
		{
			heightMap[(x+y*width)*3 + 0] = minXYZ.x + (maxXYZ.x - minXYZ.x) * x / width;
			heightMap[(x+y*width)*3 + 1] = minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[x+y*width] / (float)USHRT_MAX;
			heightMap[(x+y*width)*3 + 2] = minXYZ.z + (maxXYZ.z - minXYZ.z) * y / height;
		}
	}

	indexBuffer.numVertices = patchResolution*patchResolution * 6;
	GLuint* indices = new unsigned int[indexBuffer.numVertices];
	int i=0;
	for(int x = 0; x < patchResolution; x++)
	{
		for(int y=0; y < patchResolution; y++)
		{
			indices[i++] = x +			y*width;
			indices[i++] = x +		(y+1)*width;
			indices[i++] = (x+1) +		y*width;

			indices[i++] = (x+1) +		y*width;
			indices[i++] = x +		(y+1)*width;
			indices[i++] = (x+1) +	(y+1)*width;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*width*height*3, heightMap, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indexBuffer.numVertices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//////////////////
	Vec3f n;
	unsigned char* groundValues = new unsigned char[width*height*4];
	int xPOT = uPowerOfTwo(width);
	int zPOT = uPowerOfTwo(height);
	for(unsigned int x=0; x < xPOT; x++)
	{
		for(unsigned int z=0; z < zPOT; z++)
		{
			n = interpolatedNormal(Vec2f(float(x*width)/xPOT,float(z*height)/zPOT));
			if(n.magnitudeSquared() < 0.001)
				n = Vec3f(0.0,1.0,0.0);

			groundValues[(x + z * xPOT)*4 + 0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x + z * xPOT)*4 + 1] = (unsigned char)(n.y*255.0);
			groundValues[(x + z * xPOT)*4 + 2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x + z * xPOT)*4 + 3] = (unsigned char)(255.0*(interpolatedHeight(Vec2f(float(x*width)/xPOT,float(z*height)/zPOT))-minXYZ.y)/(maxXYZ.y-minXYZ.y));
		}
	}
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, xPOT, zPOT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xPOT, zPOT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] groundValues;
	/////////////////

	delete[] heightMap;
	delete[] indices;
///////////////////////////////////////////////////QUAD TREE///////////////////////////////////////////////////
	unsigned int nPatches = 0;
	for(int i=0; i<=LevelsDeep; i++)
	{
		nPatches += 1 << (2*i);
	}

	trunk = new TerrainPatch[nPatches];
	trunk->parent = nullptr;
	trunk->init(trunk, 0, LevelsDeep);

	TerrainPatch* p;
	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (1<<l); x++)
		{
			for(unsigned int y=0; y < (1<<l); y++)
			{
				p = getPatch(l,x,y);

				if(x > 0)			p->neighbors[0] = getPatch(l,x-1,y);
				if(x < (1<<l)-1)	p->neighbors[1] = getPatch(l,x+1,y);
				if(y > 0)			p->neighbors[2] = getPatch(l,x,y-1);
				if(y < (1<<l)-1)	p->neighbors[3] = getPatch(l,x,y+1);

			}
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
TerrainPatch* TerrainPage::getPatch(unsigned int level,unsigned int x, unsigned int y) const
{
	return trunk + trunk->levelOffset(level) + x + y*(1<<level);
}
Vec3f TerrainPage::rasterNormal(Vec2u loc) const
{
	float Cy = (loc.y < height-1)	? ((float)heights[(loc.x)	+ (loc.y+1)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Ay = (loc.x < width-1)	? ((float)heights[(loc.x+1) + (loc.y)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Dy = (  loc.y > 0	)		? ((float)heights[(loc.x)	+ (loc.y-1)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float By = (  loc.x > 0	)		? ((float)heights[(loc.x-1)	+ (loc.y)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;

	return Vec3f(Cy - Ay, 30.0, Dy - By).normalize();
}
float TerrainPage::rasterHeight(Vec2u loc) const
{
	return minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[min(loc.x,width) + min(loc.y,height) * width] / USHRT_MAX;
}
Vec3f TerrainPage::interpolatedNormal(Vec2f loc) const
{
	if(loc.x-floor(loc.x)+loc.y-floor(loc.y)<1.0)
	{
		Vec3f A = rasterNormal(Vec2u(floor(loc.x),floor(loc.y)));
		Vec3f B = rasterNormal(Vec2u(floor(loc.x),floor(loc.y+1)));
		Vec3f D = rasterNormal(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(A,B,loc.y-floor(loc.y)),D,loc.x-floor(loc.x));
	}
	else
	{
		Vec3f B = rasterNormal(Vec2u(floor(loc.x),floor(loc.y+1)));
		Vec3f C = rasterNormal(Vec2u(floor(loc.x+1),floor(loc.y+1)));
		Vec3f D = rasterNormal(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(B,C,loc.x-floor(loc.x)),D,1.0-(loc.y-floor(loc.y)));
	}
}
float TerrainPage::interpolatedHeight(Vec2f loc) const
{
	if(loc.x-floor(loc.x)+loc.y-floor(loc.y)<1.0)
	{
		float A = rasterHeight(Vec2u(floor(loc.x),floor(loc.y)));
		float B = rasterHeight(Vec2u(floor(loc.x),floor(loc.y+1)));
		float D = rasterHeight(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(A,B,loc.y-floor(loc.y)),D,loc.x-floor(loc.x));
	}
	else
	{
		float B = rasterHeight(Vec2u(floor(loc.x),floor(loc.y+1)));
		float C = rasterHeight(Vec2u(floor(loc.x+1),floor(loc.y+1)));
		float D = rasterHeight(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(B,C,loc.x-floor(loc.x)),D,1.0-(loc.y-floor(loc.y)));
	}
}
Vec3f TerrainPage::getNormal(Vec2f loc) const
{
	loc -= Vec2f(minXYZ.x, minXYZ.z);
	loc.x *= (maxXYZ.x - minXYZ.x) / width;
	loc.y *= (maxXYZ.z - minXYZ.z) / height;

	if(loc.x-floor(loc.x)+loc.y-floor(loc.y)<1.0)
	{
		Vec3f A = rasterNormal(Vec2u(floor(loc.x),floor(loc.y)));
		Vec3f B = rasterNormal(Vec2u(floor(loc.x),floor(loc.y+1)));
		Vec3f D = rasterNormal(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(A,B,loc.y-floor(loc.y)),D,loc.x-floor(loc.x));
	}
	else
	{
		Vec3f B = rasterNormal(Vec2u(floor(loc.x),floor(loc.y+1)));
		Vec3f C = rasterNormal(Vec2u(floor(loc.x+1),floor(loc.y+1)));
		Vec3f D = rasterNormal(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(B,C,loc.x-floor(loc.x)),D,1.0-(loc.y-floor(loc.y)));
	}
}
float TerrainPage::getHeight(Vec2f loc) const
{
	loc = loc - Vec2f(minXYZ.x, minXYZ.z);
	loc.x *= width / (maxXYZ.x - minXYZ.x);
	loc.y *= height / (maxXYZ.z - minXYZ.z);

	if(loc.x-floor(loc.x)+loc.y-floor(loc.y)<1.0)
	{
		float A = rasterHeight(Vec2u(floor(loc.x),floor(loc.y)));
		float B = rasterHeight(Vec2u(floor(loc.x),floor(loc.y+1)));
		float D = rasterHeight(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(A,B,loc.y-floor(loc.y)),D,loc.x-floor(loc.x));
	}
	else
	{
		float B = rasterHeight(Vec2u(floor(loc.x),floor(loc.y+1)));
		float C = rasterHeight(Vec2u(floor(loc.x+1),floor(loc.y+1)));
		float D = rasterHeight(Vec2u(floor(loc.x+1),floor(loc.y)));
		return lerp(lerp(B,C,loc.x-floor(loc.x)),D,1.0-(loc.y-floor(loc.y)));
	}
}
void TerrainPage::render(Vec3f eye) const
{
	renderQueue.clear();

	TerrainPatch* p;
	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (1<<l); x++)
		{
			for(unsigned int y=0; y < (1<<l); y++)
			{
				getPatch(l,x,y)->flags = COMBINED;
			}
		}
	}

	//for(int l=0; l<=levelsDeep; l++)
	//{
	//	for(unsigned int x=0; x < (1<<l); x++)
	//	{
	//		for(unsigned int y=0; y < (1<<l); y++)
	//		{
	//			p = getPatch(l,x,y);
	//			if(l != 0 && p->parent->flags == SUBDIVIDED)
	//			{
	//				//todo: write code to decide whether to subdivide patch or just render it
	//				//if patch is to be rendered then it should be pushed back in renderQueue
	//			}
	//		}
	//	}
	//}

	for(unsigned int x=0; x < (1<<levelsDeep); x++) //for now just render the highest level of detail
	{
		for(unsigned int y=0; y < (1<<levelsDeep); y++)
		{
			p = getPatch(levelsDeep,x,y);
			renderQueue.push_back(p);
		}
	}
	dataManager.bind("island new terrain");
	dataManager.bind("sand",0);
	dataManager.bind("grass",1);
	dataManager.bind("rock",2);
	dataManager.bind("LCnoise",3);
	dataManager.bindTex(texture,4);

	dataManager.setUniform1f("maxHeight",	maxXYZ.y);//shader should just accept minXYZ and maxXYZ vectors
	dataManager.setUniform1f("minHeight",	minXYZ.y);
	dataManager.setUniform1f("XZscale",		maxXYZ.z-minXYZ.z);
	dataManager.setUniform1f("time",		world.time());

	dataManager.setUniform1i("sand",		0);
	dataManager.setUniform1i("grass",		1);
	dataManager.setUniform1i("rock",		2);
	dataManager.setUniform1i("LCnoise",		3);
	dataManager.setUniform1i("groundTex",	4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableClientState(GL_VERTEX_ARRAY);

	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
		glDrawElements(GL_TRIANGLES,indexBuffer.numVertices, GL_UNSIGNED_INT,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	dataManager.unbindShader();
	dataManager.unbindTextures();
}
void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, bool water)
{
	waterPlane = water;
	std::shared_ptr<TerrainPage> p(new TerrainPage(Heights,0,patchResolution,position,scale));
	terrainPages.push_back(p);
	mBounds = Circle<float>(Vec2f(position.x + scale.x * 0.5, position.z + scale.z * 0.5), max(scale.x, scale.z));
	glError();
}
void Terrain::renderTerrain(Vec3f eye) const
{
	Vec3d center(eye.x,0,eye.z);
	double radius = (eye.y)*tan(asin(6000000/(6000000+eye.y)));
	if(waterPlane)
	{
		glDepthMask(false);
		
		dataManager.bind("horizon2");
		dataManager.bind("hardNoise",0);
		dataManager.bindTex(0,1);
		dataManager.bind("sand",2);

		dataManager.setUniform1i("bumpMap",	0);
		dataManager.setUniform1i("ground",	1);
		dataManager.setUniform1i("tex",		2);
		dataManager.setUniform1f("time",	world.time());
		dataManager.setUniform1f("seaLevel",0);
		dataManager.setUniform2f("center",	center.x,center.z);
		dataManager.setUniform3f("eyePos", eye.x, eye.y, eye.z);
		dataManager.setUniform1f("scale", radius);
		glEnable(GL_BLEND);
		glPushMatrix();
		glTranslatef(center.x, center.y, center.z);
		glScalef(radius,1,radius);
		graphics->drawModelCustomShader("disk",center,Quat4f(),Vec3f(radius,1,radius));
		glPopMatrix();

		dataManager.unbindTextures();
		dataManager.unbindShader();

		glDepthMask(true);
	}
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		(*i)->render(eye);
	}
	if(waterPlane)
	{
		glColorMask(false,false,false,false);//this code interferes with stereo rendering...
		glPushMatrix();
		glTranslatef(center.x, center.y, center.z);
		glScalef(radius,1,radius);
		graphics->drawModel("disk",center,Quat4f(),Vec3f(radius,1,radius));
		glPopMatrix();
		glColorMask(true,true,true,true);
	}
}
std::shared_ptr<TerrainPage> Terrain::getPage(Vec2f position) const
{
	for(auto i=terrainPages.begin(); i!=terrainPages.end(); i++)
	{
		if((*i)->minXYZ.x <= position.x && (*i)->maxXYZ.x >= position.x && (*i)->minXYZ.z <= position.y && (*i)->maxXYZ.z >= position.y)
		{
			return *i;
		}
	}
	return std::shared_ptr<TerrainPage>();
}
std::shared_ptr<TerrainPage> Terrain::getPage(Vec3f position) const // position.y is ignored
{
	for(auto i=terrainPages.begin(); i!=terrainPages.end(); i++)
	{
		if((*i)->minXYZ.x <= position.x && (*i)->maxXYZ.x >= position.x && (*i)->minXYZ.z <= position.z && (*i)->maxXYZ.z >= position.z)
		{
			return *i;
		}
	}
	return std::shared_ptr<TerrainPage>();
}
float Terrain::elevation(Vec2f v) const
{
	auto ptr = getPage(v);
	if(ptr)
	{
		return waterPlane ? max(0.0f,ptr->getHeight(v)) : ptr->getHeight(v);
	}
	return 0.0f;
}
float Terrain::elevation(float x, float z) const
{
	return elevation(Vec2f(x,z));
}
float Terrain::altitude(Vec3f v) const
{
	auto ptr = getPage(v);
	if(ptr)
	{
		return waterPlane ? v.y-max(0.0f,ptr->getHeight(Vec2f(v.x,v.z))) : v.y-ptr->getHeight(Vec2f(v.x,v.z));
	}
	return v.y;
}
float Terrain::altitude(float x, float y, float z) const
{
	return altitude(Vec3f(x,y,z));
}
bool Terrain::isLand(Vec2f v) const
{
	auto ptr = getPage(v);
	return ptr && ptr->getHeight(v) > 0.0f;
}
bool Terrain::isLand(float x, float z) const
{
	return isLand(Vec2f(x,z));
}