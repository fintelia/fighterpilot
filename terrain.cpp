

#include "engine.h"
#include "GL/glee.h"
#include <GL/glu.h>

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

	indexBuffer.numVertices = patchResolution*patchResolution * 6;
	unsigned int* indices = new unsigned int[indexBuffer.numVertices];
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
	dataManager.bind("grass normals",5);

	dataManager.setUniform1f("maxHeight",	maxXYZ.y);//shader should just accept minXYZ and maxXYZ vectors
	dataManager.setUniform1f("minHeight",	minXYZ.y);
	dataManager.setUniform1f("XZscale",		maxXYZ.z-minXYZ.z);
	dataManager.setUniform1f("time",		world.time());

	dataManager.setUniform3f("lightPosition", graphics->getLightPosition());

	dataManager.setUniform1i("sand",		0);
	dataManager.setUniform1i("grass",		1);
	dataManager.setUniform1i("rock",		2);
	dataManager.setUniform1i("LCnoise",		3);
	dataManager.setUniform1i("groundTex",	4);
	dataManager.setUniform1i("grass_normals", 5);

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

	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.id);
		glDrawElements(GL_TRIANGLES,indexBuffer.numVertices, GL_UNSIGNED_INT,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	dataManager.unbindShader();
	dataManager.unbindTextures();
}
void Terrain::generateSky(Angle theta, Angle phi, float L)//see "Rendering Physically-Based Skyboxes" - Game Engine Gems 1
{
	float e = 2.718281828;

	unsigned int l = 256; //length of sides of square for each face in cube map
	float m = 0.5 * l - 0.5;
	float* cubeMap = new float[l*l * 6 * 3];
	unsigned char* cubeMapTex = new unsigned char[l*l * 6 * 3];

	Vec3f sunDirection(cos(phi)*sin(theta), cos(theta), sin(phi)*sin(theta));
	sunDirection = sunDirection.normalize();



	//xyY color space stored as:				x						  Y						  y			
	float constants[5][3] =	{{-0.0193*L - 0.2592,		 0.1787*L - 1.4630,		-0.0167*L - 0.2608},
							 {-0.0665*L + 0.0008,		-0.3554*L + 0.4275,		-0.0950*L + 0.0092},
							 {-0.0004*L + 0.2125,		-0.0227*L + 5.3251,		-0.0079*L + 0.2102},
							 {-0.0641*L - 0.8989,		 0.1206*L - 2.5771,		-0.0441*L - 1.6537},
							 {-0.0033*L + 0.0452,		-0.0670*L + 0.3703,		-0.0109*L + 0.0529}};



	//float zenithOverSun[3] = {0.186220 / ((1.0 + constants[0][0] * pow(e, constants[1][0] / (float)cos(theta))) * (2.0 + constants[4][0])),
	//						  0.13 / ((1.0 + constants[0][1] * pow(e, constants[1][1] / (float)cos(theta))) * (2.0 + constants[4][1])),
	//						  0.130598 / ((1.0 + constants[0][2] * pow(e, constants[1][2] / (float)cos(theta))) * (2.0 + constants[4][2]))};

	float zenithOverSun[3] = {1.0/0.186220,
							  1.0/0.13,
							  1.0/0.130598};


	Vec3f direction;
	unsigned int i = 0;
	for(int face=0; face<6; face++)
	{
		for(int x=0;x<l;x++)
		{
			for(int y=0;y<l;y++)
			{
				if(face == 0)		direction.set(-m + y, m, -m + x); //top
				else if(face == 1)	direction.set(-m + y, -m, m - x); //bottom
				else if(face == 2)	direction.set(-m + y, m - x, m); //back
				else if(face == 3)	direction.set(m - y, m - x, -m); //front
				else if(face == 4)	direction.set(m, m - x, m - y); //right
				else if(face == 5)	direction.set(-m, m - x, -m + y); //left
				direction = direction.normalize();

				float cos_sunAngle = direction.dot(sunDirection);
				float sunAngle = acos(cos_sunAngle);
				float cos_upAngle = max(direction.y,0.1);//cos_upAng should really be just direction.y but this prevents the horizon from having artifacts

				cubeMap[i + 0] = (1.0 + constants[0][0] * pow(e, constants[1][0]/cos_upAngle)) * (1.0 + pow(constants[2][0], constants[3][0] * sunAngle) + constants[4][0]*cos_sunAngle*cos_sunAngle) * zenithOverSun[0];
				cubeMap[i + 1] = (1.0 + constants[0][1] * pow(e, constants[1][1]/cos_upAngle)) * (1.0 + pow(constants[2][1], constants[3][1] * sunAngle) + constants[4][1]*cos_sunAngle*cos_sunAngle) * zenithOverSun[1];
				cubeMap[i + 2] = (1.0 + constants[0][2] * pow(e, constants[1][2]/cos_upAngle)) * (1.0 + pow(constants[2][2], constants[3][2] * sunAngle) + constants[4][2]*cos_sunAngle*cos_sunAngle) * zenithOverSun[2];
			
				i +=3;
			}
		}
	}
	
	for(i=0; i < l*l*6*3; i+=3)// xyY -> XYZ (actually xYy -> XYZ)
	{
		float x = cubeMap[i + 0];
		float y = cubeMap[i + 2];


		cubeMap[i + 1] = cubeMap[i + 1] / (1.0 + cubeMap[i + 1]);	//tone mapping (optional?)
		//cubeMap[i + 1] = pow((double)cubeMap[i + 1], invGamma);

		cubeMap[i + 0] = cubeMap[i + 1] * x / y;
		cubeMap[i + 2] = cubeMap[i + 1] * (1.0 - x - y) / y;
	}

	float R, G, B, f;
	const float invGamma = 1.0/1.8;
	for(i=0; i < l*l*6*3; i+=3)// XYZ -> rgb
	{
		R = cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.49853;
		G = cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556;
		B = cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311;

		f = max(max(R,G),max(G,1.0)); //find the max component or 1.0, whichever is bigger
		cubeMapTex[i + 0] = pow(clamp(R/f, 0.0, 1.0),invGamma) * 255.0;
		cubeMapTex[i + 1] = pow(clamp(G/f, 0.0, 1.0),invGamma) * 255.0;
		cubeMapTex[i + 2] = pow(clamp(B/f, 0.0, 1.0),invGamma) * 255.0;

		float brightness = pow(clamp(cubeMap[i + 0], 0.0, 1.0),invGamma);
		cubeMapTex[i + 0] = ((brightness) * 1.0 + (1.0-brightness) * 0.2) * 255.0;
		cubeMapTex[i + 1] = ((brightness) * 1.0 + (1.0-brightness) * 0.2) * 255.0;
		cubeMapTex[i + 2] = ((brightness) * 1.0 + (1.0-brightness) * 1.0) * 255.0;


		//cubeMapTex[i + 0] = ((cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.49853) + minC) / (maxC-minC) * 255.0;
		//cubeMapTex[i + 1] = ((cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556) + minC) / (maxC-minC) * 255.0;
		//cubeMapTex[i + 2] = ((cubeMap[ i+ 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311) + minC) / (maxC-minC) * 255.0;

		//cubeMapTex[i + 0] = clamp((cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.49853),0.0,1.0) * 255.0;
		//cubeMapTex[i + 1] = clamp((cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556),0.0,1.0) * 255.0;
		//cubeMapTex[i + 2] = clamp((cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311),0.0,1.0) * 255.0;

		//cubeMapTex[i + 0] = clamp(abs(cubeMap[i + 0] *  1.4628067		+		cubeMap[i + 1] * -0.1840623		+		cubeMap[i + 2] * -0.2743606),0.0,1.0) * 255.0;
		//cubeMapTex[i + 1] = clamp(abs(cubeMap[i + 0] * -0.5217933		+		cubeMap[i + 1] *  1.4472381		+		cubeMap[i + 2] *  0.0677227),0.0,1.0) * 255.0;
		//cubeMapTex[i + 2] = clamp(abs(cubeMap[i + 0] *  0.0349342		+		cubeMap[i + 1] * -0.0968930		+		cubeMap[i + 2] *  1.2884099),0.0,1.0) * 255.0;

		//cubeMapTex[i + 0] = cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.49853;
		//cubeMapTex[i + 1] = cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556;
		//cubeMapTex[i + 2] = cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311;

		//cubeMapTex[i + 0] = clamp(cubeMap[i + 0],0.0,1.0) * 255;
		//cubeMapTex[i + 1] = clamp(cubeMap[i + 1],0.0,1.0) * 255;
		//cubeMapTex[i + 2] = clamp(1.0-cubeMap[i + 2],0.0,1.0) * 255;
	}

	glGenTextures(1, &skyTextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, l, l, 0, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 5]);

	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 0]);
	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 1]);
	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 2]);
	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 3]);
	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 4]);
	//gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_RGB8, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, &cubeMapTex[l*l*3 * 5]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] cubeMap;
	delete[] cubeMapTex;
}
Terrain::~Terrain()
{
	if(skyTextureId != 0)
	{
		glDeleteTextures(1, &skyTextureId);
	}
}
void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, bool water)
{
	waterPlane = water;
	std::shared_ptr<TerrainPage> p(new TerrainPage(Heights,0,patchResolution,position,scale));
	terrainPages.push_back(p);
	mBounds = Circle<float>(Vec2f(position.x + scale.x * 0.5, position.z + scale.z * 0.5), max(scale.x, scale.z));

	generateSky(3.0*PI/8, 0.0, 20); //should actually make the sun be at the position of the light source...

	graphics->checkErrors();
}
void Terrain::renderTerrain(Vec3f eye) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	Vec3d center(eye.x,0,eye.z);
	double radius = (eye.y)*tan(asin(6000000/(6000000+eye.y)));

	graphics->setDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	dataManager.bind("sky shader");

	dataManager.setUniform1i("tex", 0);
	dataManager.setUniform1i("clouds", 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);
	dataManager.bind("clouds",1);
	graphics->drawModelCustomShader("sky dome",Vec3f(0,-10000,0),Quat4f(),Vec3f(600000,200000,600000));
	dataManager.bind("clouds",1);
	graphics->drawModelCustomShader("sky dome",Vec3f(0,-10000,0),Quat4f(),Vec3f(600000,-200000,600000));
	dataManager.bindTex(0,1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	dataManager.bindTex(0,0);
	dataManager.unbindShader();
	glEnable(GL_DEPTH_TEST);

	if(waterPlane)
	{
		dataManager.bind("horizon2");
		dataManager.setUniform1i("bumpMap",	0);
		dataManager.setUniform1f("time",	world.time());
		dataManager.setUniform1f("seaLevel",0);
		dataManager.setUniform2f("center",	center.x,center.z);
		dataManager.setUniform3f("eyePos", eye.x, eye.y, eye.z);
		dataManager.setUniform1f("scale", radius);
		
		dataManager.setUniform3f("lightPosition", graphics->getLightPosition());

		graphics->drawModelCustomShader("disk",center,Quat4f(),Vec3f(radius,1,radius));

		dataManager.unbindTextures();
		dataManager.unbindShader();
	}
	graphics->setDepthMask(true);

	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		(*i)->render(eye);
	}
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
bool Terrain::isLand(Vec2f v) const
{
	auto ptr = getPage(v);
	return ptr && ptr->getHeight(v) > 0.0f;
}
bool Terrain::isLand(float x, float z) const
{
	return isLand(Vec2f(x,z));
}