

#include "engine.h"

const unsigned char LEFT		= 0x01; //patch to the left is one level above this patch
const unsigned char RIGHT		= 0x02; //patch to the right is one level above this patch
const unsigned char TOP			= 0x04; //patch to the top is one level above this patch
const unsigned char BOTTOM		= 0x08; //patch to the bottom is one level above this patch
//
//const unsigned char SUBDIVIDED	= 0x10;	//patch is divided for rendering
//const unsigned char LEVEL_USED	= 0x20; //patch is rendered at this level
//const unsigned char COMBINED	= 0x40; //patch is not subdivided this far

TerrainPatch::TerrainPatch(): parent(nullptr)
{
	divisionLevel = COMBINED;
	edgeFlags = 0;

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

		row = (this - trunk - cLevelOffset) >> levelsDeep;
		col = (this - trunk - cLevelOffset) % (1 << levelsDeep);


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

		unsigned int cLevelOffset = levelOffset(levelsDeep);
		row = (this - trunk - cLevelOffset) >> levelsDeep;
		col = (this - trunk - cLevelOffset) % (1 << levelsDeep);
	}
}
void TerrainPatch::subdivide(const Vec3f& eye) const
{
	float d = (eye.x-center.x)*(eye.x-center.x) + (eye.z-center.z)*(eye.z-center.z);//eye.distanceSquared(center);
	if(children[0] && children[1] && children[2] && children[3] && d < minDistanceSquared)// || (bounds.minXYZ.x < eye.x && bounds.minXYZ.z < eye.z && bounds.maxXYZ.x > eye.x && bounds.maxXYZ.z > eye.z)
	{
		divisionLevel = SUBDIVIDED;

		children[0]->subdivide(eye);
		children[1]->subdivide(eye);
		children[2]->subdivide(eye);
		children[3]->subdivide(eye);
	}
	else
	{
		divisionLevel = LEVEL_USED;
	}
}
void TerrainPatch::patchEdges() const
{
	if(divisionLevel == SUBDIVIDED)
	{
		children[0]->patchEdges();
		children[1]->patchEdges();
		children[2]->patchEdges();
		children[3]->patchEdges();
	}
	else
	{
		//write code to find out whether we need neighbors are further subdivided
	}
}
TerrainPage::TerrainPage(unsigned short* Heights, unsigned int patchResolution, Vec3f position, Vec3f scale, unsigned int lod):minXYZ(position), maxXYZ(position+scale),LOD(lod), heights(Heights), trunk(nullptr)
{
	double t = GetTime();

	unsigned short* nHeights = nullptr;
	if(!isPowerOfTwo(patchResolution-1))
	{
		unsigned int nPatchResolution = uPowerOfTwo(patchResolution-1) + 1;
		nHeights = new unsigned short[nPatchResolution*nPatchResolution];

		scale.x *= static_cast<float>(nPatchResolution) / patchResolution;
		scale.z *= static_cast<float>(nPatchResolution) / patchResolution;
		maxXYZ = position+scale;

		for(unsigned int x = 0; x < nPatchResolution; x++)
		{
			for(unsigned int y = 0; y < nPatchResolution; y++)
			{
				if(x < patchResolution && y < patchResolution)
				{
					nHeights[x + y*nPatchResolution] = heights[x + y*patchResolution];
				}
				else
				{
					nHeights[x + y*nPatchResolution] = 0;
				}
			}
		}
		swap(nHeights, heights);
		patchResolution = nPatchResolution;
	}

	height = patchResolution;
	width = patchResolution;

	unsigned int v = ((patchResolution-1)/LOD) >> 4;
	levelsDeep = 0;
	while(v >>= 1) ++levelsDeep;

//////////////////////////////////////////////////////VBO and texture//////////////////////////////////////////////////////

	texture = graphics->genTexture2D();

	int numHeights=0;
	float* heightMap = new float[((width-1)/LOD+1)*((height-1)/LOD+1)*3];
	for(int x = 0; x < width; x += LOD)
	{
		for(int y=0; y < height; y += LOD)
		{
			heightMap[numHeights*3 + 0] = minXYZ.x + (maxXYZ.x - minXYZ.x) * x / width;
			heightMap[numHeights*3 + 1] = minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[x+y*width] / (float)USHRT_MAX;
			heightMap[numHeights*3 + 2] = minXYZ.z + (maxXYZ.z - minXYZ.z) * y / height;
			numHeights++;
		}
	}

	vertexBuffer = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
	vertexBuffer->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0*sizeof(float));
	vertexBuffer->setTotalVertexSize(sizeof(float)*3);
	vertexBuffer->setVertexData(sizeof(float)*((width-1)/LOD+1)*((height-1)/LOD+1)*3, heightMap);

	const unsigned int numIndices = 16*16 * 6;
	unsigned int* indices = new unsigned int[numIndices];
	for(unsigned int d = 0; d <= levelsDeep; d++)
	{
		int i=0;
		unsigned int spacing = 1 << (levelsDeep-d);	
		for(int y=0; y < 16; y++)
		{
			for(int x = 0; x < 16; x++)
			{

				indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
				indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
				indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);

				indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
				indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
				indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
			}
		}

		auto iBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
		iBuffer->setData(indices, numIndices);
		indexBuffers.push_back(iBuffer);
	}
	delete[] indices;

	//////////////////
	Vec3f n;
	//int xPOT = uPowerOfTwo(width);
	//int zPOT = uPowerOfTwo(height);

	/////////////////////////////////////
	unsigned short* groundValues = new unsigned short[(width)*(height)*4];
	for(unsigned int x=0; x < width; x++)
	{
		for(unsigned int z=0; z < height; z++)
		{
			n = rasterNormal(Vec2u(x,z));
			//n = interpolatedNormal(Vec2f(x,z));
			if(n.magnitudeSquared() < 0.001)
				n = Vec3f(0.0,1.0,0.0);
	
			groundValues[(x + z * width)*4 + 0] = (unsigned short)(32767.0+n.x*32768.0);
			groundValues[(x + z * width)*4 + 1] = (unsigned short)(n.y*65536.0);
			groundValues[(x + z * width)*4 + 2] = (unsigned short)(32767.0+n.z*32768.0);
			groundValues[(x + z * width)*4 + 3] = heights[x+z*width];
		}
	}
	texture->setData(width, height, GraphicsManager::texture::RGBA16, (unsigned char*)groundValues);
	/////////////////////////////////////
	//float sx = static_cast<float>(width-1)/width;
	//float sz = static_cast<float>(height-1)/height;
	//unsigned short* groundValues = new unsigned short[(width-1)*(height-1)*4];
	//for(unsigned int x=0; x < width-1; x++)
	//{
	//	for(unsigned int z=0; z < height-1; z++)
	//	{
	//		n = interpolatedNormal(Vec2u(sx*x,sz*z));
	//		//n = interpolatedNormal(Vec2f(x,z));
	//		if(n.magnitudeSquared() < 0.001)
	//			n = Vec3f(0.0,1.0,0.0);
	//
	//		groundValues[(x + z * (width-1))*4 + 0] = (unsigned short)(32767.0+n.x*32768.0);
	//		groundValues[(x + z * (width-1))*4 + 1] = (unsigned short)(n.y*65536.0);
	//		groundValues[(x + z * (width-1))*4 + 2] = (unsigned short)(32767.0+n.z*32768.0);
	//		groundValues[(x + z * (width-1))*4 + 3] = (unsigned short)(interpolatedHeight(Vec2f(sx*x,sz*z)) - minXYZ.y)/(maxXYZ.y-minXYZ.y)*USHRT_MAX;
	//	}
	//}
	//texture->setData(width-1, height-1, GraphicsManager::texture::RGBA16, (unsigned char*)groundValues);
	/////////////////////////////////////
	//unsigned char* groundValues = new unsigned char[(width)*(height)*4];
	//for(unsigned int x=0; x < width; x++)
	//{
	//	for(unsigned int z=0; z < height; z++)
	//	{
	//		n = rasterNormal(Vec2u(x,z));
	//		if(n.magnitudeSquared() < 0.001)
	//			n = Vec3f(0.0,1.0,0.0);
	//
	//		groundValues[(x + z * width)*4 + 0] = (unsigned char)(127.0+n.x*128.0);
	//		groundValues[(x + z * width)*4 + 1] = (unsigned char)(n.y*255.0);
	//		groundValues[(x + z * width)*4 + 2] = (unsigned char)(127.0+n.z*128.0);
	//		groundValues[(x + z * width)*4 + 3] = (unsigned char)(heights[x+z*width]>>8);
	//	}
	//}
	//texture->setData(width, height, GraphicsManager::texture::RGBA, (unsigned char*)groundValues);


	delete[] groundValues;
	/////////////////

	delete[] heightMap;
	delete[] nHeights;

///////////////////////////////////////////////////QUAD TREE///////////////////////////////////////////////////
	unsigned int nPatches = 0;
	for(int i=0; i<=levelsDeep; i++)
	{
		nPatches += 1 << (2*i);
	}

	trunk = new TerrainPatch[nPatches];
	trunk->parent = nullptr;
	trunk->init(trunk, 0, levelsDeep);

	TerrainPatch* p;
	float e;
	float r = sqrt(scale.x*scale.x + scale.z*scale.z) / 2;
	unsigned int spacing;
	float errorScale = (maxXYZ.y - minXYZ.y) / USHRT_MAX / 2.0;

	float C = 900.0;//(1.0 / 512.0) / (2.0 * 4.0 / 1024); // A = (nearClipPlane / abs(topClipPlane)) / (2*maxErrorInPixels / verticalResolution)
	for(int l=levelsDeep; l>=0; l--)
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

				p->bounds.minXYZ = minXYZ + Vec3f(scale.x * x / (1<<l), 0, scale.z * y / (1<<l));
				p->bounds.maxXYZ = minXYZ + Vec3f(scale.x * (x+1) / (1<<l), scale.y, scale.z * (y+1) / (1<<l));
				p->center = (p->bounds.minXYZ + p->bounds.maxXYZ) * 0.5;

				p->maxError = 0.0f;
				p->minDistanceSquared = 0.0f;

				if(l < levelsDeep)
				{
					spacing = 1 << (levelsDeep-l);
					for(unsigned int h = x*16*spacing; h < (x*16+15)*spacing; h += spacing)
					{
						for(unsigned int k = y*16*spacing; k < (y*16+15)*spacing; k += spacing)
						{
							e = errorScale * abs(heights[h*LOD + k*LOD*width] + heights[(h+spacing)*LOD + k*LOD*width] - 2*heights[(h+spacing/2)*LOD + k*LOD*width]);
							if(e > p->maxError) p->maxError = e;

							e = errorScale * abs(heights[h*LOD + k*LOD*width] + heights[h*LOD + (k+spacing)*LOD*width] - 2*heights[h*LOD + (k+spacing/2)*LOD*width]);
							if(e > p->maxError) p->maxError = e;
						}
					}
					//p->maxError += max(max(p->children[0]->maxError, p->children[1]->maxError),max(p->children[2]->maxError, p->children[3]->maxError));
					//p->minDistanceSquared = (p->maxError * C) * (p->maxError * C);
					p->minDistanceSquared = (p->maxError * C + r/(1<<l)) * (p->maxError * C + r/(1<<l));
				}
			}
		}
	}
}
TerrainPatch* TerrainPage::getPatch(unsigned int level,unsigned int x, unsigned int y) const
{
	return trunk + trunk->levelOffset(level) + x + y*(1<<level);
}
Vec3f TerrainPage::rasterNormal(Vec2u loc) const
{
	loc.x = clamp(loc.x, 0, width-1);
	loc.y = clamp(loc.y, 0, height-1);

	float By = (loc.y < height-1)	? ((float)heights[(loc.x)	+ (loc.y+1)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Ay = (loc.x < width-1)	? ((float)heights[(loc.x+1) + (loc.y)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Dy = (  loc.y > 0	)		? ((float)heights[(loc.x)	+ (loc.y-1)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Cy = (  loc.x > 0	)		? ((float)heights[(loc.x-1)	+ (loc.y)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;

	return Vec3f(Cy - Ay, 2.0 * (maxXYZ.x-minXYZ.x) / width, Dy - By).normalize();
}
float TerrainPage::rasterHeight(Vec2u loc) const
{
	return minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[min(loc.x,width-1) + min(loc.y,height-1) * width] / USHRT_MAX;
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
	loc = loc - Vec2f(minXYZ.x, minXYZ.z);
	loc.x *= width / (maxXYZ.x - minXYZ.x);
	loc.y *= height / (maxXYZ.z - minXYZ.z);

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
void TerrainPage::render(shared_ptr<GraphicsManager::View> view) const
{
	renderQueue.clear();
	TerrainPatch* p;

	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (0x1<<l); x++)
		{
			for(unsigned int y=0; y < (0x1<<l); y++)
			{
				getPatch(l,x,y)->divisionLevel = TerrainPatch::COMBINED;
			}
		}
	}

	trunk->subdivide(view->camera().eye);
	trunk->patchEdges();
	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (1<<l); x++)
		{
			for(unsigned int y=0; y < (1<<l); y++)
			{
				p = getPatch(l,x,y);
				if(p->divisionLevel == TerrainPatch::LEVEL_USED && view->boundingBoxInFrustum(p->bounds))
				{
					renderQueue.push_back(p);
				}
			}
		}
	}

	//for(unsigned int x=0; x < (1<<(levelsDeep)); x++) //for now just render the highest level of detail
	//{
	//	for(unsigned int y=0; y < (1<<(levelsDeep)); y++)
	//	{
	//		p = getPatch(levelsDeep,x,y);
	//		if(view->boundingBoxInFrustum(p->bounds))
	//			renderQueue.push_back(p);
	//	}
	//}

	//view->lookAt(Vec3f(12900/2,15000,12900/2),Vec3f(12900/2,0,12900/2), Vec3f(0,0,1));
	texture->bind(1);
	//graphics->setWireFrame(true);

	Vec3f eye;

	unsigned int bufferOffset;
	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		//float d = (eye.x-(*i)->center.x)*(eye.x-(*i)->center.x) + (eye.z-(*i)->center.z)*(eye.z-(*i)->center.z);
		//dataManager.setUniform1f("dist", (*i)->minDistanceSquared / d);
		bufferOffset = sizeof(float)*3 * ((*i)->row + ((*i)->col*((width-1)/LOD+1))) * (16<<(levelsDeep-(*i)->level));
		indexBuffers[(*i)->level]->drawBuffer(GraphicsManager::TRIANGLES, vertexBuffer, bufferOffset);
		//graphics->drawLine(Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z), Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z));
		//graphics->drawLine(Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z), Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z));
		//graphics->drawLine(Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z), Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z));
		//graphics->drawLine(Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z), Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z));
	}

	//graphics->setWireFrame(false);
}
TerrainPage::~TerrainPage()
{
	delete[] heights;
	delete[] trunk;
}
void Terrain::generateSky(Angle theta, Angle phi, float L)//see "Rendering Physically-Based Skyboxes" - Game Engine Gems 1 and http://www.cs.utah.edu/~shirley/papers/sunsky/sunsky.pdf
{//theta = angle from sun to zenith, phi = angle from south axis (positive is towards east)
	double t = GetTime();

	unsigned int l = 64; //length of sides of square for each face in cube map
	float m = 0.5 * l - 0.5;
	float* cubeMap = new float[l*l * 6 * 3];
	unsigned char* cubeMapTex = new unsigned char[l*l * 6 * 3];

	Vec3f sunDirection(cos(phi)*sin(theta), cos(theta), sin(phi)*sin(theta));
	sunDirection = sunDirection.normalize();//is this necessary?


	//xyY color space stored as:		x						  Y						  y			
	float constants[5][3] =	{{-0.0193*L - 0.2592,		 0.1787*L - 1.4630,		-0.0167*L - 0.2608},
							 {-0.0665*L + 0.0008,		-0.3554*L + 0.4275,		-0.0950*L + 0.0092},
							 {-0.0004*L + 0.2125,		-0.0227*L + 5.3251,		-0.0079*L + 0.2102},
							 {-0.0641*L - 0.8989,		 0.1206*L - 2.5771,		-0.0441*L - 1.6537},
							 {-0.0033*L + 0.0452,		-0.0670*L + 0.3703,		-0.0109*L + 0.0529}};



	float theta2 = theta*theta;
	float theta3 = theta*theta*theta;
	float chi = (4.0/9.0 - L/120.0)*(PI - 2*theta);
	float zenithLuminance = abs((4.0453*L - 4.9710) * tan(chi) - 0.2155*L + 0.24192);// * 1000;
	float zenith_x = L*L*(0.00166*theta3 - 0.00375*theta2 + 0.00209*theta) + L*(-0.02903*theta3 + 0.06377*theta2 - 0.03202*theta + 0.00394) + (0.11693*theta3 - 0.21196*theta2 + 0.06052*theta + 0.25886);
	float zenith_y = L*L*(0.00275*theta3 - 0.00610*theta2 + 0.00317*theta) + L*(-0.04214*theta3 + 0.08970*theta2 - 0.04153*theta + 0.00516) + (0.15346*theta3 - 0.26756*theta2 + 0.06670*theta + 0.26688);
	float zenithOverSun[3] = {0.85*zenith_x /		((1.0 + constants[0][0] * exp(constants[1][0])) * (1.0 + constants[2][0]*exp(constants[3][0]*(float)theta) + constants[4][0]*cos(theta)*cos(theta))),
							  0.20*zenithLuminance / ((1.0 + constants[0][1] * exp(constants[1][1])) * (1.0 + constants[2][1]*exp(constants[3][1]*(float)theta) + constants[4][1]*cos(theta)*cos(theta))),
							  0.85*zenith_y /		((1.0 + constants[0][2] * exp(constants[1][2])) * (1.0 + constants[2][2]*exp(constants[3][2]*(float)theta) + constants[4][2]*cos(theta)*cos(theta)))};

	//float zenithOverSun[3] = {1.0/0.186220,
	//						  1.0/0.13,
	//						  1.0/0.130598};

	Vec3f direction;
	unsigned int i = 0;
	for(int face=0; face<6; face++)
	{
		for(int x=0;x<l;x++)
		{
			for(int y=0;y<l;y++)
			{
				if(face == 0)		direction.set(m, m - x, m - y); //right
				else if(face == 1)	direction.set(-m, m - x, -m + y); //left
				else if(face == 2)	direction.set(-m + y, m, -m + x); //top
				else if(face == 3)	direction.set(-m + y, -m, m - x); //bottom
				else if(face == 4)	direction.set(-m + y, m - x, m); //back
				else if(face == 5)	direction.set(m - y, m - x, -m); //front

				direction = direction.normalize();

				float cos_sunAngle = direction.dot(sunDirection);
				float sunAngle = acos(cos_sunAngle);
				float reciprocal_cos_upAngle = 1.0 / max(direction.y,0.01);//cos_upAng should really be just direction.y but this prevents the horizon from having artifacts

			//	if(sunAngle > PI/2) sunAngle = PI/4 + sunAngle * 0.5;
				
																												  //pow(constants[2][i], constants[3][i] * sunAngle) is the only term that dramatically effects the color accros the sky
				cubeMap[i + 0] = (1.0 + constants[0][0] * exp(constants[1][0]*reciprocal_cos_upAngle)) * (1.0 + constants[2][0]*exp(constants[3][0] * sunAngle) + constants[4][0]*cos_sunAngle*cos_sunAngle) * zenithOverSun[0];
				cubeMap[i + 1] = (1.0 + constants[0][1] * exp(constants[1][1]*reciprocal_cos_upAngle)) * (1.0 + constants[2][1]*exp(constants[3][1] * sunAngle) + constants[4][1]*cos_sunAngle*cos_sunAngle) * zenithOverSun[1];
				cubeMap[i + 2] = (1.0 + constants[0][2] * exp(constants[1][2]*reciprocal_cos_upAngle)) * (1.0 + constants[2][2]*exp(constants[3][2] * sunAngle) + constants[4][2]*cos_sunAngle*cos_sunAngle) * zenithOverSun[2];
			
				i +=3;

			}
		}
	}
	
	t = GetTime() - t;
	t = GetTime();

	for(i=0; i < l*l*6*3; i+=3)// xyY -> XYZ (actually xYy -> XYZ)
	{
		float x = cubeMap[i + 0];
		float y = cubeMap[i + 2];


		cubeMap[i + 1] = 0.75 * cubeMap[i + 1] / (1.0 + cubeMap[i + 1]);	//tone mapping (optional?)
		//cubeMap[i + 1] = pow((double)cubeMap[i + 1], invGamma);

		cubeMap[i + 0] = cubeMap[i + 1] * x / y;
		cubeMap[i + 2] = cubeMap[i + 1] * (1.0 - x - y) / y;
	}

	t = GetTime() - t;
	t = GetTime();

	const float invGamma = 1.0/1.8;
	for(i=0; i < l*l*6*3; i+=3)// XYZ -> rgb
	{
	//	float brightness = pow(clamp(cubeMap[i + 0], 0.0, 1.0),invGamma);
		cubeMapTex[i + 0] = clamp((cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.498530),0.0,1.0) * 255.0;
		cubeMapTex[i + 1] = clamp((cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556),0.0,1.0) * 255.0;
		cubeMapTex[i + 2] = clamp((cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311),0.0,1.0) * 255.0;
	}

	t = GetTime() - t;
	t = GetTime();

	skyTexture = graphics->genTextureCube();
	skyTexture->setData(l, l, GraphicsManager::texture::RGB, cubeMapTex);

	delete[] cubeMap;
	delete[] cubeMapTex;

	t = GetTime() - t;
	t = GetTime();
}
float cosineInterpolation(float v1, float v2, float interp)
{
	return  lerp(v1, v2, (1.0 - cos(interp * PI)) * 0.5);
}
void Terrain::generateOceanTexture()
{
	double t = GetTime();

	unsigned int X = random<unsigned int>(UINT_MAX); // for random numbers;

	const unsigned int resolution = 128;
	const unsigned int depth = 4;
	const unsigned int numOctaves = 5;

	float* heights = new float[resolution * resolution * depth];
	float* layer = new float[resolution * resolution];
	unsigned char* textureData = new unsigned char[resolution * resolution * depth * 4];

	for(unsigned int i=0; i<resolution * resolution * depth; i++)
		heights[i] = 0.0f;


	float h11, h21, h22, h12, x1, x2;
	for(unsigned int octave = 0; (octave < numOctaves) && (resolution >> (octave + 3) > 0); octave++)
	{
		unsigned int octaveStep = resolution >> (octave + 3);

		for(unsigned int z = 0; z < depth; z += 1)
		{
			for(unsigned int x = 0; x < resolution; x += octaveStep)
			{
				for(unsigned int y = 0; y < resolution; y += octaveStep)
				{
					X = (1664525 * X + 1013904223)/* % 0x100000000*/;

					layer[x + y*resolution] = float(X) / UINT_MAX * pow(0.5f,(int)octave+1); //random<float>(pow(0.5f,(int)octave+1));
				}
			}
			for(unsigned int x = 0; x < resolution; x ++)
			{
				for(unsigned int y = 0; y < resolution; y ++)
				{
					if(x % octaveStep != 0 || y % octaveStep != 0)
					{
						h11 = layer[(x - (x%octaveStep))							+ (y - (y%octaveStep)) * resolution];
						h21 = layer[(x - (x%octaveStep) + octaveStep)%resolution	+ (y - (y%octaveStep)) * resolution];
						h22 = layer[(x - (x%octaveStep) + octaveStep)%resolution	+ ((y - (y%octaveStep) + octaveStep)%resolution) * resolution];
						h12 = layer[(x - (x%octaveStep))							+ ((y - (y%octaveStep) + octaveStep)%resolution) * resolution];

						x1 = cosineInterpolation(h11, h12, static_cast<float>(y%octaveStep)/octaveStep);
						x2 = cosineInterpolation(h21, h22, static_cast<float>(y%octaveStep)/octaveStep);

						heights[x + y*resolution + z * resolution * resolution] += cosineInterpolation(x1, x2, static_cast<float>(x%octaveStep)/octaveStep);
					}
					else
					{
						heights[x + y*resolution + z * resolution * resolution] += layer[x + y*resolution];
					}
				}
			}
		}
	}

	t = GetTime() - t;
	t = GetTime();

	Vec3f n;
	float Ay, By, Cy, Dy;
	for(unsigned int z = 0; z < depth; z++)
	{
		for(unsigned int x = 0; x < resolution; x++)
		{
			for(unsigned int y = 0; y < resolution; y++)
			{
				By = heights[x				+ ((y+1)%resolution)*resolution + z * resolution * resolution] - heights[x + y*resolution + z * resolution * resolution];
				Ay = heights[(x+1)%resolution	+ y*resolution					+ z * resolution * resolution] - heights[x + y*resolution + z * resolution * resolution];
				Dy = heights[x				+ ((y-1)%resolution)*resolution + z * resolution * resolution] - heights[x + y*resolution + z * resolution * resolution];
				Cy = heights[(x-1)%resolution	+ y*resolution					+ z * resolution * resolution] - heights[x + y*resolution + z * resolution * resolution];

				n = Vec3f(Cy - Ay, 0.1, Dy - By).normalize();

				textureData[(x + y*resolution + z * resolution * resolution)*4 + 0] = (n.x + 1.0) * 0.5 * 255.0;
				textureData[(x + y*resolution + z * resolution * resolution)*4 + 1] = n.y * 255.0;
				textureData[(x + y*resolution + z * resolution * resolution)*4 + 2] = (n.z + 1.0) * 0.5 * 255.0;
				textureData[(x + y*resolution + z * resolution * resolution)*4 + 3] = heights[x + y*resolution + z * resolution * resolution] * 255.0;
			}
		}
	}
	t = GetTime() - t;
	t = GetTime();

	oceanTexture = graphics->genTexture3D();
	oceanTexture->setData(resolution, resolution, depth, GraphicsManager::texture::RGBA, textureData, true);

	delete[] heights;
	delete[] layer;
	delete[] textureData;
}
void Terrain::generateFoliage(int count)
{
	vector<texturedVertex3D> vertices;
	texturedVertex3D tmpVerts[16];
	//plant tmpFoliage;
	Vec3f position;

	tmpVerts[0].UV = Vec2f(0.0,1);
	tmpVerts[1].UV = Vec2f(0.5,1);
	tmpVerts[2].UV = Vec2f(0.5,0);
	tmpVerts[3].UV = Vec2f(0.0,0);

	tmpVerts[4].UV = Vec2f(0.5,1);
	tmpVerts[5].UV = Vec2f(1.0,1);
	tmpVerts[6].UV = Vec2f(1.0,0);
	tmpVerts[7].UV = Vec2f(0.5,0);

	tmpVerts[8].UV = Vec2f(0.5,1);
	tmpVerts[9].UV = Vec2f(0.0,1);
	tmpVerts[10].UV = Vec2f(0.0,0);
	tmpVerts[11].UV = Vec2f(0.5,0);

	tmpVerts[12].UV = Vec2f(1.0,1);
	tmpVerts[13].UV = Vec2f(0.5,1);
	tmpVerts[14].UV = Vec2f(0.5,0);
	tmpVerts[15].UV = Vec2f(1.0,0);

	Vec2f dir;
	Vec3f right, fwd;
	unsigned int nQuads=0;
	unsigned int n = 0;

	for(int i=0; i<1000000 && nQuads<count*2; i++)
	{
		position.x = terrainPosition.x + random<float>(terrainScale.x);
		position.z = terrainPosition.z + random<float>(terrainScale.z);
		position.y = elevation(position.x, position.z);

		if(position.y > 40.0)
		{
			n = random<float>(1.0) < 0.5 ? 0 : 8;

			dir = random2<float>();
			float s = 4.0 + random<float>(4.0) + random<float>(4.0);
			tmpVerts[n+0].position = position + Vec3f(-2.0*dir.x, 0.0, -2.0*dir.y)*s;
			tmpVerts[n+1].position = position + Vec3f( 2.0*dir.x, 0.0,  2.0*dir.y)*s;
			tmpVerts[n+2].position = position + Vec3f( 2.0*dir.x, 5.0,  2.0*dir.y)*s;
			tmpVerts[n+3].position = position + Vec3f(-2.0*dir.x, 5.0, -2.0*dir.y)*s;

			swap(dir.x, dir.y);
			dir.x = - dir.x;
			tmpVerts[n+4].position = position + Vec3f(-2.0*dir.x, 0.0, -2.0*dir.y)*s;
			tmpVerts[n+5].position = position + Vec3f( 2.0*dir.x, 0.0,  2.0*dir.y)*s;
			tmpVerts[n+6].position = position + Vec3f( 2.0*dir.x, 5.0,  2.0*dir.y)*s;
			tmpVerts[n+7].position = position + Vec3f(-2.0*dir.x, 5.0, -2.0*dir.y)*s;

			//tmpVerts[n+0].padding[0] = s*0.4;
			//tmpVerts[n+1].padding[0] = s*0.4;
			//tmpVerts[n+2].padding[0] = s*0.4;
			//tmpVerts[n+3].padding[0] = s*0.4;



			//tmpVerts[4].position = position + Vec3f(-2.0*dir.x, 0.0, -2.0*dir.y)*s;
			//tmpVerts[5].position = position + Vec3f( 2.0*dir.x, 0.0,  2.0*dir.y)*s;
			//tmpVerts[6].position = position + Vec3f( 2.0*dir.x, 5.0,  2.0*dir.y)*s;
			//tmpVerts[7].position = position + Vec3f(-2.0*dir.x, 5.0, -2.0*dir.y)*s;

			vertices.push_back(tmpVerts[n+0]);
			vertices.push_back(tmpVerts[n+1]);
			vertices.push_back(tmpVerts[n+2]);
			vertices.push_back(tmpVerts[n+3]);
			vertices.push_back(tmpVerts[n+4]);
			vertices.push_back(tmpVerts[n+5]);
			vertices.push_back(tmpVerts[n+6]);
			vertices.push_back(tmpVerts[n+7]);

			//if(random<float>(1.0) < 0.5)
			//{
			//	foliageVertices.push_back(tmpVerts[0]);
			//	foliageVertices.push_back(tmpVerts[1]);
			//	foliageVertices.push_back(tmpVerts[2]);
			//	foliageVertices.push_back(tmpVerts[3]);
			//}
			//else
			//{
			//	foliageVertices.push_back(tmpVerts[4]);
			//	foliageVertices.push_back(tmpVerts[5]);
			//	foliageVertices.push_back(tmpVerts[6]);
			//	foliageVertices.push_back(tmpVerts[7]);
			//}

			//tmpFoliage.height = 5.0*s;
			//tmpFoliage.location = position;
			//foliage.push_back(tmpFoliage);

			nQuads += 2;
		}
	}

	foliageIBO = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
	if(nQuads > 0 && nQuads*6 > 65536)
	{
		unsigned int* indices = new unsigned int[nQuads*6];
		for(unsigned int i=0; i<nQuads; i++)
		{
			indices[i*6+0] = i*4 + 0;
			indices[i*6+1] = i*4 + 1;
			indices[i*6+2] = i*4 + 2;
			indices[i*6+3] = i*4 + 0;
			indices[i*6+4] = i*4 + 2;
			indices[i*6+5] = i*4 + 3;
		}
		foliageIBO->setData(indices, nQuads*6);
	}
	else if(nQuads > 0)
	{
		unsigned short* indices = new unsigned short[nQuads*6];
		for(unsigned int i=0; i<nQuads; i++)
		{
			indices[i*6+0] = i*4 + 0;
			indices[i*6+1] = i*4 + 1;
			indices[i*6+2] = i*4 + 2;
			indices[i*6+3] = i*4 + 0;
			indices[i*6+4] = i*4 + 2;
			indices[i*6+5] = i*4 + 3;
		}
		foliageIBO->setData(indices, nQuads*6);
	}

	foliageVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM);
	foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
	foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		3*sizeof(float));
	//foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::NORMAL,		3*sizeof(float));
	foliageVBO->setTotalVertexSize(sizeof(texturedVertex3D));
	foliageVBO->setVertexData(sizeof(texturedVertex3D)*vertices.size(), !vertices.empty() ? &vertices[0] : nullptr);
}
void Terrain::resetTerrain()
{
	skyTexture.reset();
	oceanTexture.reset();
	terrainPages.clear();
}
Terrain::~Terrain()
{
	resetTerrain();
}
void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, TerrainType shader, int foliageAmount, unsigned int LOD)
{
	shaderType = shader;
	waterPlane = (shader==TERRAIN_ISLAND);
	std::shared_ptr<TerrainPage> p(new TerrainPage(Heights,patchResolution,position,scale,LOD));
	terrainPages.push_back(p);
	terrainPosition = position;
	terrainScale = scale;
	mBounds = Circle<float>(Vec2f(position.x + scale.x * 0.5, position.z + scale.z * 0.5), max(scale.x, scale.z)/2.0);

	Vec3f sun = (graphics->getLightPosition()).normalize();

	generateSky(acos(sun.y), atan2(sun.z,sun.x), 1.8); //should actually make the sun be at the position of the light source...
	generateOceanTexture();

	generateFoliage(foliageAmount);

	graphics->checkErrors();
}
void Terrain::renderTerrain(shared_ptr<GraphicsManager::View> view) const
{
	Vec3f eye = view->camera().eye;
	Vec3f center = Vec3f(eye.x,0,eye.z);
	float h = eye.y;	//height off the ground
	float r = 6367500;	//radius of the earth

	double radius = max(h*tan(asin(r/(r+h))), 2.0*max(terrainScale.x,terrainScale.z));



	//float h2 = sqrt(2.0*r*r+2.0*r*h+h*h) / r;

	graphics->setDepthMask(false);
	graphics->setDepthTest(false);
	dataManager.bind("sky shader");

	dataManager.setUniform1i("tex", 0);
	//dataManager.setUniform1i("clouds", 0);
	skyTexture->bind();
	//dataManager.bind("skybox");
	graphics->drawModelCustomShader("sky dome",Vec3f(eye.x,0,eye.z),Quat4f(),Vec3f(radius,radius,radius));


	// based on GPU GEMS 2: Chapter 16 Accurate Atmospheric Scattering
	/*double terrainRadius = max(terrainScale.x,terrainScale.z);
	double planetRadius = 6367500.0;//*0.003;
	Vec3f planetCenter(eye.x,-sqrt(planetRadius*planetRadius-terrainRadius*terrainRadius),eye.z);
	float m_fWavelength4[3] = {0.17850623,0.10556000,0.050906640};
	float m_fInnerRadius = planetRadius;//*0.003;
	float m_fOuterRadius = planetRadius*1.025;
	float m_ESun = 20.0;
	float m_Kr = 0.0025;
	float m_Km = 0.001;
	float m_fRayleighScaleDepth = 0.25;
	float m_g = -0.99;

	dataManager.bind("sky2 shader");
	dataManager.setUniform3f("v3CameraPos", eye);
	dataManager.setUniform3f("v3LightPos", graphics->getLightPosition().normalize());
	dataManager.setUniform3f("v3InvWavelength", 1/m_fWavelength4[0], 1/m_fWavelength4[1], 1/m_fWavelength4[2]);
	dataManager.setUniform3f("planetCenter", planetCenter);
	dataManager.setUniform1f("fCameraHeight", eye.y);
	dataManager.setUniform1f("fInnerRadius", m_fInnerRadius);
	dataManager.setUniform1f("fKrESun", m_Kr*m_ESun);
	dataManager.setUniform1f("fKmESun", m_Km*m_ESun);
	dataManager.setUniform1f("fKr4PI", m_Kr * 4.0*PI);
	dataManager.setUniform1f("fKm4PI", m_Km * 4.0*PI);
	dataManager.setUniform1f("fScale", 1.0f / (m_fOuterRadius - m_fInnerRadius));
	dataManager.setUniform1f("fScaleDepth", m_fRayleighScaleDepth);
	dataManager.setUniform1f("fScaleOverScaleDepth", (1.0f / (m_fOuterRadius - m_fInnerRadius)) / m_fRayleighScaleDepth);
	dataManager.setUniform1f("g", m_g);
	dataManager.setUniform1f("g2", m_g*m_g);
	
	graphics->drawModelCustomShader("sky dome",planetCenter,Quat4f(),Vec3f(planetRadius,planetRadius,planetRadius)*1.025);*/





	if(!waterPlane)
	{
	//	graphics->drawModelCustomShader("sky dome",Vec3f(eye.x,0,eye.z),Quat4f(),Vec3f(radius,-radius,radius));
	}

	dataManager.bind("ortho");
	Vec3f sunPos = view->project3((graphics->getLightPosition()+eye).normalize() * 3000000.0);
	if(sunPos.z > 0)
	{
		Angle rotateAng = acosA(view->camera().up.dot(Vec3f(0,1,0)));
	//	graphics->drawRotatedOverlay(Rect::CWH(sunPos.x,sunPos.y,0.45,0.45), rotateAng, "sun"); //doesn't work right in 2 player
	}

//	if(waterPlane)
	{
		dataManager.bind("ocean");

		dataManager.bind("ocean normals", 1);

		dataManager.setUniform1i("sky",	0);
		dataManager.setUniform1i("oceanNormals",	1);
		dataManager.setUniform1f("time",	world.time());
		dataManager.setUniform1f("seaLevel",0);
		dataManager.setUniform2f("center",	center.x,center.z);
		dataManager.setUniform3f("eyePos", eye.x, eye.y, eye.z);
		dataManager.setUniform1f("scale", radius);

		dataManager.setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
		dataManager.setUniformMatrix("modelTransform", Mat4f());

		dataManager.setUniform3f("lightPosition", graphics->getLightPosition());
		graphics->drawModelCustomShader("disk",center,Quat4f(),Vec3f(radius,1,radius));
	}

	graphics->setDepthMask(true);
	graphics->setDepthTest(true);
	if(shaderType == TERRAIN_ISLAND)
	{
		dataManager.bind("island terrain");
		dataManager.bind("sand",2);
		dataManager.bind("grass",3);
		dataManager.bind("rock",4);
		dataManager.bind("LCnoise",5);
		dataManager.bind("grass normals",6);
		dataManager.bind("noise",7);

		dataManager.setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());
		dataManager.setUniformMatrix("modelTransform",		Mat4f());
		dataManager.setUniform1f("time",					world.time());
		dataManager.setUniform3f("lightPosition",			graphics->getLightPosition());
		dataManager.setUniform3f("eyePos",					view->camera().eye);
		dataManager.setUniform3f("invScale",				1.0/(terrainScale.x),1.0/(terrainScale.y),1.0/(terrainScale.z));
		dataManager.setUniform1f("minHeight",				terrainPosition.y);
		dataManager.setUniform1f("heightRange",				terrainScale.y);

		dataManager.setUniform1i("sky",			0);
		dataManager.setUniform1i("sand",		2);
		dataManager.setUniform1i("grass",		3);
		dataManager.setUniform1i("rock",		4);
		dataManager.setUniform1i("LCnoise",		5);
		dataManager.setUniform1i("groundTex",	1);
		dataManager.setUniform1i("grass_normals", 6);
		dataManager.setUniform1i("noiseTex",	7);			// make sure uniform exists
	}
	else if(shaderType == TERRAIN_SNOW)
	{
		dataManager.bind("snow terrain");
		dataManager.bind("snow",2);
		dataManager.bind("LCnoise",3);
		dataManager.bind("snow normals",4);

		dataManager.setUniform1f("maxHeight",	terrainPosition.y + terrainScale.y);//shader should just accept minXYZ and maxXYZ vectors
		dataManager.setUniform1f("minHeight",	terrainPosition.y);
		dataManager.setUniform1f("XZscale",		terrainScale.x);
		dataManager.setUniform1f("time",		world.time());

		dataManager.setUniform3f("lightPosition", graphics->getLightPosition());

		dataManager.setUniform1i("snow",			2);
		dataManager.setUniform1i("LCnoise",			3);
		dataManager.setUniform1i("groundTex",		1);
		dataManager.setUniform1i("snow_normals",	4);
	}
	else if(shaderType == TERRAIN_DESERT)
	{
		dataManager.bind("desert terrain");
		dataManager.bind("desertSand",2);
		dataManager.bind("LCnoise",3);

		dataManager.bind("noise",4);

		dataManager.setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());
		dataManager.setUniformMatrix("modelTransform",		Mat4f());
		dataManager.setUniform1f("time",					world.time());
		dataManager.setUniform3f("lightPosition",			graphics->getLightPosition());
		dataManager.setUniform3f("eyePos",					view->camera().eye);
		dataManager.setUniform3f("invScale",				1.0/(terrainScale.x),1.0/(terrainScale.y),1.0/(terrainScale.z));

		dataManager.setUniform1i("sky",			0);
		dataManager.setUniform1i("sand",		2);
		dataManager.setUniform1i("LCnoise",		3);
		dataManager.setUniform1i("groundTex",	1);
		dataManager.setUniform1i("noiseTex",	4);
	}
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		(*i)->render(view);
	}
	dataManager.bind("model");
	if(waterPlane)
	{
		graphics->setColorMask(false);
		graphics->drawModel("disk",center,Quat4f(),Vec3f(radius,1,radius));
		graphics->setColorMask(true);
	}

	
	//if(!foliage.empty())
	//{
		//graphics->setDepthMask(false);
		//Vec3f up = Vec3f(0.0,1.0,0.0);
		Vec3f right = view->camera().fwd.cross(Vec3f(0,1,0)).normalize();

		//for(int i=0; i<foliage.size(); i++)
		//{
		//	foliageVertices[i*4+0].position = foliage[i].location - right * foliage[i].height;
		//	foliageVertices[i*4+1].position = foliage[i].location + right * foliage[i].height;
		//	foliageVertices[i*4+2].position = foliage[i].location + (up + right) * foliage[i].height;
		//	foliageVertices[i*4+3].position = foliage[i].location + (up - right) * foliage[i].height;
		//}
		
		//foliageVBO->setVertexData(sizeof(texturedVertex3D)*foliage.size()*4, &foliageVertices[0]);
		graphics->setAlphaToCoverage(true);
		dataManager.bind("tree");
		//dataManager.bind("skybox",1);
		skyTexture->bind(1);

		if(graphics->getMultisampling())
			dataManager.bind("trees shader");
		else
			dataManager.bind("trees alpha test shader");

		dataManager.setUniform1i("tex", 0);
		dataManager.setUniform1i("sky", 1);
		dataManager.setUniform3f("eyePos", eye);

		dataManager.setUniform3f("right", right);
		dataManager.setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());

		foliageIBO->drawBuffer(GraphicsManager::TRIANGLES,foliageVBO);
		graphics->setAlphaToCoverage(false);
	//}
	
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
Vec3f Terrain::terrainNormal(Vec2f v) const
{
	auto ptr = getPage(v);
	if(ptr && ptr->getHeight(v) > 0.0f)
	{
		return (ptr->getNormal(v)).normalize();
	}
	
	return Vec3f(0,1,0);
}
Vec3f Terrain::terrainNormal(float x, float z) const
{
	return terrainNormal(Vec2f(x,z));
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