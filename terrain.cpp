
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

		children[0] = trunk + childLevelOffset + col * 2 + row * 2 * (1 << levelsDeep+1);
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
TerrainPatch* TerrainPage::getPatch(unsigned int level,unsigned int x, unsigned int y)
{
	return trunk + trunk->levelOffset(level) + x + y*(1<<level);
}
void TerrainPage::initQuadTree(unsigned int LevelsDeep)
{
	levelsDeep = LevelsDeep;
	width = height = (1 << LevelsDeep) + 1;

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
	return;
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

	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (1<<l); x++)
		{
			for(unsigned int y=0; y < (1<<l); y++)
			{
				p = getPatch(l,x,y);
				if(l != 0 && p->parent->flags == SUBDIVIDED)
				{
					//todo: write code to decide whether to subdivide patch or just render it
					//if patch is to be rendered then it should be pushed back in renderQueue
				}
			}
		}
	}

	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		//todo: render patch
	}
}
void Terrain::initTerrain()
{
	TerrainPage p;
	p.initQuadTree(4);
	terrainPages.push_back(p);
}
void Terrain::renderTerrain()
{
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		i->render();
	}
}