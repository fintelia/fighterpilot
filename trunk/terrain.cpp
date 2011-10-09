

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
//////////////////////////////////////////////////////VBO//////////////////////////////////////////////////////
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&indexBuffer.id);


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
TerrainPatch* TerrainPage::getPatch(unsigned int level,unsigned int x, unsigned int y)
{
	return trunk + trunk->levelOffset(level) + x + y*(1<<level);
}

void TerrainPage::render()
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
	dataManager.bind("white",4);

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
void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale)
{
	std::shared_ptr<TerrainPage> p(new TerrainPage(Heights,0,patchResolution,position,scale));
	terrainPages.push_back(p);
	glError()
}
void Terrain::renderTerrain()
{
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		(*i)->render();
	}
}
