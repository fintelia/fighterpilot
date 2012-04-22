

#include "engine.h"
#include "GL/glee.h"
#include <GL/glu.h>

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
void TerrainPatch::subdivide(const Vec3f& eye, float error) const
{
	float d = eye.distanceSquared(center);
	if(children[0] && children[1] && children[2] && children[3] && d < minDistanceSquared)// || (bounds.minXYZ.x < eye.x && bounds.minXYZ.z < eye.z && bounds.maxXYZ.x > eye.x && bounds.maxXYZ.z > eye.z)
	{
		divisionLevel = SUBDIVIDED;

		children[0]->subdivide(eye, error);
		children[1]->subdivide(eye, error);
		children[2]->subdivide(eye, error);
		children[3]->subdivide(eye, error);
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
TerrainPage::TerrainPage(unsigned short* Heights, unsigned int patchResolution, Vec3f position, Vec3f scale):minXYZ(position), maxXYZ(position+scale), heights(Heights), trunk(nullptr), indexBuffer(nullptr)
{
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

	unsigned int v = (patchResolution-1) >> 4;
	levelsDeep = 0;
	while(v >>= 1) ++levelsDeep;

//////////////////////////////////////////////////////VBO and texture//////////////////////////////////////////////////////
	glGenBuffers(1,&VBO);
	

	texture = graphics->genTexture2D();

	float* heightMap = new float[width*height*3];
	for(int x = 0; x < width; x++)
	{
		for(int y=0; y < height; y++)
		{
			heightMap[(x+y*width)*3 + 0] = minXYZ.x + (maxXYZ.x - minXYZ.x) * x / width;
			heightMap[(x+y*width)*3 + 1] = minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[x+y*width] / (float)USHRT_MAX;
			heightMap[(x+y*width)*3 + 2] = minXYZ.z + (maxXYZ.z - minXYZ.z) * y / height;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*width*height*3, heightMap, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indexBuffer = new IndexBuffer[levelsDeep+1];
	for(unsigned int d = 0; d <= levelsDeep; d++)
	{
		indexBuffer[d].numVertices = 16*16 * 6;
		unsigned int* indices = new unsigned int[indexBuffer[d].numVertices];
		int i=0;
		unsigned int spacing = 1 << (levelsDeep-d);	
		for(int y=0; y < 16; y++)
		{
			for(int x = 0; x < 16; x++)
			{

				indices[i++] = (x*spacing)			+	(y*spacing)*width;
				indices[i++] = (x*spacing)			+	((y+1)*spacing)*width;
				indices[i++] = ((x+1)*spacing)		+	(y*spacing)*width;

				indices[i++] = ((x+1)*spacing)		+	(y*spacing)*width;
				indices[i++] = (x*spacing)			+	((y+1)*spacing)*width;
				indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*width;
			}
		}

		glGenBuffers(1,&indexBuffer[d].id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[d].id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indexBuffer[d].numVertices, indices, GL_STATIC_DRAW);	
		delete[] indices;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	//////////////////
	Vec3f n;
	//int xPOT = uPowerOfTwo(width);
	//int zPOT = uPowerOfTwo(height);
	unsigned char* groundValues = new unsigned char[(width)*(height)*4];
	for(unsigned int x=0; x < width; x++)
	{
		for(unsigned int z=0; z < height; z++)
		{
			n = interpolatedNormal(Vec2f(x,z));
			if(n.magnitudeSquared() < 0.001)
				n = Vec3f(0.0,1.0,0.0);

			groundValues[(x + z * width)*4 + 0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x + z * width)*4 + 1] = (unsigned char)(n.y*255.0);
			groundValues[(x + z * width)*4 + 2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x + z * width)*4 + 3] = (unsigned char)((interpolatedHeight(Vec2f(x,z))-minXYZ.y)/(maxXYZ.y-minXYZ.y));
		}
	}
	texture->setData(width, height, GraphicsManager::texture::RGBA, groundValues);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, xPOT, zPOT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xPOT, zPOT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	//glBindTexture(GL_TEXTURE_2D, 0);

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

	float C = 32.0;//(1.0 / 512.0) / (2.0 * 4.0 / 1024); // A = (nearClipPlane / abs(topClipPlane)) / (2*maxErrorInPixels / verticalResolution)

	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (1<<l); x++)
		{
			for(unsigned int y=0; y < (1<<l); y++)
			{
				p = getPatch(l,y,x);

				if(x > 0)			p->neighbors[0] = getPatch(l,x-1,y);
				if(x < (1<<l)-1)	p->neighbors[1] = getPatch(l,x+1,y);
				if(y > 0)			p->neighbors[2] = getPatch(l,x,y-1);
				if(y < (1<<l)-1)	p->neighbors[3] = getPatch(l,x,y+1);

				p->bounds.minXYZ = minXYZ + Vec3f(scale.x * x / (1<<l), 0, scale.z * y / (1<<l));
				p->bounds.maxXYZ = minXYZ + Vec3f(scale.x * (x+1) / (1<<l), scale.y, scale.z * (y+1) / (1<<l));
				p->center = (p->bounds.minXYZ + p->bounds.maxXYZ) / 2.0;

				p->maxError = 0.0f;
				p->minDistanceSquared = 0.0f;

				if(l < levelsDeep)
				{
					spacing = 1 << (levelsDeep-l);
					for(unsigned int h = x*16*spacing; h < (x*16+15)*spacing; h += spacing)
					{
						for(unsigned int k = y*16*spacing; k < (y*16+15)*spacing; k += spacing)
						{
							e = errorScale * abs(heights[h + k*width] + heights[(h+spacing) + k*width] - 2*heights[(h+spacing/2) + k*width]);
							if(e > p->maxError) p->maxError = e;

							e = errorScale * abs(heights[h + k*width] + heights[h + (k+spacing)*width] - 2*heights[h + (k+spacing/2)*width]);
							if(e > p->maxError) p->maxError = e;
						}
					}
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

	//trunk->subdivide(view->camera().eye, 0.1);
	//trunk->patchEdges();
	//for(int l=0; l<=levelsDeep; l++)
	//{
	//	for(unsigned int x=0; x < (1<<l); x++)
	//	{
	//		for(unsigned int y=0; y < (1<<l); y++)
	//		{
	//			p = getPatch(l,x,y);
	//			if(p->divisionLevel == TerrainPatch::LEVEL_USED)
	//			{
	//				renderQueue.push_back(p);
	//			}
	//		}
	//	}
	//}

	for(unsigned int x=0; x < (1<<(levelsDeep)); x++) //for now just render the highest level of detail
	{
		for(unsigned int y=0; y < (1<<(levelsDeep)); y++)
		{
			p = getPatch(levelsDeep,x,y);
			if(view->boundingBoxInFrustum(p->bounds))
				renderQueue.push_back(p);
		}
	}

	dataManager.bind("island new terrain");
	dataManager.bind("sand",1);
	dataManager.bind("grass",2);
	dataManager.bind("rock",3);
	dataManager.bind("LCnoise",4);
	texture->bind(5);
	dataManager.bind("grass normals",6);
	dataManager.bind("noise",7);


	dataManager.setUniform1f("maxHeight",	maxXYZ.y);//shader should just accept minXYZ and maxXYZ vectors
	dataManager.setUniform1f("minHeight",	minXYZ.y);
	dataManager.setUniform1f("XZscale",		maxXYZ.z-minXYZ.z);
	dataManager.setUniform1f("time",		world.time());

	Vec3f eye = view->camera().eye;
	dataManager.setUniform3f("lightPosition", graphics->getLightPosition());
	dataManager.setUniform3f("eyePos",		eye.x, eye.y, eye.z);
	dataManager.setUniform1i("sky",			0);
	dataManager.setUniform1i("sand",		1);
	dataManager.setUniform1i("grass",		2);
	dataManager.setUniform1i("rock",		3);
	dataManager.setUniform1i("LCnoise",		4);
	dataManager.setUniform1i("groundTex",	5);
	dataManager.setUniform1i("grass_normals", 6);
	dataManager.setUniform1i("noiseTex",	7);			// make sure uniform exists

	//dataManager.bind("snow terrain");
	//dataManager.bind("snow",0);
	//dataManager.bind("LCnoise",1);
	//dataManager.bindTex(texture,2);
	//dataManager.bind("snow normals",3);

	//dataManager.setUniform1f("maxHeight",	maxXYZ.y);//shader should just accept minXYZ and maxXYZ vectors
	//dataManager.setUniform1f("minHeight",	minXYZ.y);
	//dataManager.setUniform1f("XZscale",		maxXYZ.z-minXYZ.z);
	//dataManager.setUniform1f("time",		world.time());

	//dataManager.setUniform3f("lightPosition", graphics->getLightPosition());

	//dataManager.setUniform1i("snow",			0);
	//dataManager.setUniform1i("LCnoise",			1);
	//dataManager.setUniform1i("groundTex",		2);
	//dataManager.setUniform1i("snow_normals",	3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableClientState(GL_VERTEX_ARRAY);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDisable(GL_CULL_FACE);

	unsigned int bufferOffset;
	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		bufferOffset = sizeof(float)*3 * (  (*i)->row * (16<<(levelsDeep-(*i)->level)) + (*i)->col*width * (16<<(levelsDeep-(*i)->level))  );


		glVertexPointer(3, GL_FLOAT, 0, (void*)bufferOffset);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[(*i)->level].id);
		glDrawElements(GL_TRIANGLES,indexBuffer[(*i)->level].numVertices, GL_UNSIGNED_INT,0);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	dataManager.unbindShader();
	dataManager.unbindTextures();
}
TerrainPage::~TerrainPage()
{
	if(VBO != 0)
		glDeleteBuffers(1, &VBO);

	delete[] indexBuffer;
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


		cubeMap[i + 1] = cubeMap[i + 1] / (1.0 + cubeMap[i + 1]);	//tone mapping (optional?)
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
	//	cubeMapTex[i + 0] = ((brightness) * 1.0 + (1.0-brightness) * 0.2) * 255.0;
	//	cubeMapTex[i + 1] = ((brightness) * 1.0 + (1.0-brightness) * 0.2) * 255.0;
	//	cubeMapTex[i + 2] = ((brightness) * 1.0 + (1.0-brightness) * 1.0) * 255.0;

		cubeMapTex[i + 0] = clamp((cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.49853),0.0,1.0) * 255.0;
		cubeMapTex[i + 1] = clamp((cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556),0.0,1.0) * 255.0;
		cubeMapTex[i + 2] = clamp((cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311),0.0,1.0) * 255.0;

	//	cubeMapTex[i + 0] = cubeMap[i + 0] > 1.0 ? 255 : 0;//clamp(cubeMap[i + 0],0.0,1.0) * 255;
	//	cubeMapTex[i + 1] = cubeMap[i + 0] > 1.0 ? 255 : 0;//clamp(cubeMap[i + 1],0.0,1.0) * 255;
	//	cubeMapTex[i + 2] = cubeMap[i + 0] > 1.0 ? 255 : 0;//clamp(cubeMap[i + 2],0.0,1.0) * 255;
	}

	t = GetTime() - t;
	t = GetTime();

	skyTexture = graphics->genTextureCube();

	skyTexture->setData(l, l, GraphicsManager::texture::RGB, cubeMapTex);

	//glGenTextures(1, &skyTextureId);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 0]);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 1]);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 2]);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 3]);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 4]);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 5]);

	////gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 0]);
	////gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 1]);
	////gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 2]);
	////gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 3]);
	////gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 4]);
	////gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 5]);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

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

	//glGenTextures(1,&oceanTextureId);
	//glBindTexture(GL_TEXTURE_3D, oceanTextureId);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, TRUE);
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, resolution, resolution, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	//	t = GetTime() - t;
	//t = GetTime();

	//glBindTexture(GL_TEXTURE_3D, 0);

	delete[] heights;
	delete[] layer;
	delete[] textureData;
}
void Terrain::resetTerrain()
{
	//skyTexture.reset();	//no reason to get rid of these if we don't have to
	//oceanTexture.reset();
	terrainPages.clear();
}
Terrain::~Terrain()
{
	resetTerrain();
}
void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, bool water)
{
	waterPlane = water;
	std::shared_ptr<TerrainPage> p(new TerrainPage(Heights,patchResolution,position,scale));
	terrainPages.push_back(p);
	mBounds = Circle<float>(Vec2f(position.x + scale.x * 0.5, position.z + scale.z * 0.5), max(scale.x, scale.z));

	Vec3f sun = (graphics->getLightPosition()).normalize();

	generateSky(acos(sun.y), atan2(sun.z,sun.x), 1.8); //should actually make the sun be at the position of the light source...
	generateOceanTexture();
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

	glEnableClientState(GL_VERTEX_ARRAY);
	graphics->setDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	dataManager.bind("sky shader");

	dataManager.setUniform1i("tex", 0);
	//dataManager.setUniform1i("clouds", 0);
	skyTexture->bind();
	//graphics->drawModelCustomShader("sky dome",Vec3f(0,-10000,0),Quat4f(),Vec3f(600000,100000,600000));
	//graphics->drawModelCustomShader("sky dome",Vec3f(0,-10000,0),Quat4f(),Vec3f(600000,-100000,600000));
	graphics->drawModelCustomShader("sky dome",Vec3f(eye.x,0,eye.z),Quat4f(),Vec3f(radius,radius,radius));

	if(!waterPlane)
	{
		graphics->drawModelCustomShader("sky dome",Vec3f(eye.x,0,eye.z),Quat4f(),Vec3f(radius,-radius,radius));
	}

	dataManager.bind("ortho");
	Vec3f sunPos = view->project3((graphics->getLightPosition()+eye).normalize() * 3000000.0);
	if(sunPos.z > 0)
	{
		Angle rotateAng = acosA(view->camera().up.dot(Vec3f(0,1,0)));
		graphics->setBlendMode(GraphicsManager::ADDITIVE);
		graphics->drawRotatedOverlay(Rect::CWH(sunPos.x,sunPos.y,0.25,0.25), rotateAng, "sun");
		graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
	}

	if(waterPlane)
	{
		dataManager.bind("ocean");

		//dataManager.bind("ocean normals");
		oceanTexture->bind(1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_3D, oceanTextureId);

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

		//glBindTexture(GL_TEXTURE_3D, 0);
	}
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //could be unbound earlier but left to use for refletions from the water
	dataManager.unbindShader();



	graphics->setDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		(*i)->render(view);
	}
	glDisable(GL_CULL_FACE);
	if(waterPlane)
	{
		graphics->setColorMask(false);
		graphics->drawModel("disk",center,Quat4f(),Vec3f(radius,1,radius));
		graphics->setColorMask(true);
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