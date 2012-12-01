

#include "engine.h"

const unsigned char LEFT		= 0x01; //patch to the left is one level above this patch
const unsigned char RIGHT		= 0x02; //patch to the right is one level above this patch
const unsigned char TOP			= 0x04; //patch to the top is one level above this patch
const unsigned char BOTTOM		= 0x08; //patch to the bottom is one level above this patch
//
//const unsigned char SUBDIVIDED	= 0x10;	//patch is divided for rendering
//const unsigned char LEVEL_USED	= 0x20; //patch is rendered at this level
//const unsigned char COMBINED	= 0x40; //patch is not subdivided this far

Terrain::Patch::Patch()//: parent(nullptr)
{
	divisionLevel = COMBINED;
	edgeFlags = 0;

	//children[0] = nullptr;
	//children[1] = nullptr;
	//children[2] = nullptr;
	//children[3] = nullptr;
	//neighbors[0] = nullptr;
	//neighbors[1] = nullptr;
	//neighbors[2] = nullptr;
	//neighbors[3] = nullptr;
}
//void Terrain::Patch::init(Patch* trunk, unsigned int levelsDeep, unsigned int totalLevels)
//{
//	level = levelsDeep;
//	if(levelsDeep < totalLevels)
//	{
//		//flags = flags | CAN_SUBDIVIDE;
//	
//		unsigned int cLevelOffset = levelOffset(levelsDeep);
//		unsigned int childLevelOffset = levelOffset(levelsDeep+1);
//	
//		row = (this - trunk - cLevelOffset) >> levelsDeep;
//		col = (this - trunk - cLevelOffset) % (1 << levelsDeep);
//	
//	
//		children[0] = trunk + childLevelOffset + col * 2 + row * 2 * (1 << (levelsDeep+1));
//		children[1] = children[0] + 1;
//	
//		children[2] = children[0] + (1 << (levelsDeep+1));
//		children[3] = children[2] + 1;
//	
//	
//	
//		for(int i=0; i<4;i++)
//		{
//			children[i]->parent = this;
//			children[i]->init(trunk, levelsDeep + 1, totalLevels);
//		}
//	}
//	else
//	{
//		//flags = flags & (~CAN_SUBDIVIDE);
//	
//		unsigned int cLevelOffset = levelOffset(levelsDeep);
//		row = (this - trunk - cLevelOffset) >> levelsDeep;
//		col = (this - trunk - cLevelOffset) % (1 << levelsDeep);
//	}
//}

void Terrain::Page::subdivide(quadTree<Patch>::node* n, const Vec3f& eye) const
{
	float d = (eye.x-n->element.center.x)*(eye.x-n->element.center.x) + (eye.z-n->element.center.z)*(eye.z-n->element.center.z);//eye.distanceSquared(center);
	if(n->children[0] && n->children[1] && n->children[2] && n->children[3] && d < n->element.minDistanceSquared)// || (bounds.minXYZ.x < eye.x && bounds.minXYZ.z < eye.z && bounds.maxXYZ.x > eye.x && bounds.maxXYZ.z > eye.z)
	{
		n->element.divisionLevel = Patch::SUBDIVIDED;

		subdivide(n->children[0],eye);
		subdivide(n->children[1],eye);
		subdivide(n->children[2],eye);
		subdivide(n->children[3],eye);
	}
	else
	{
		n->element.divisionLevel = Patch::LEVEL_USED;
	}
}
void Terrain::Page::setSubdivided(quadTree<Patch>::node* n) const
{
	n->element.divisionLevel = Patch::SUBDIVIDED;
	if(n->parent) setSubdivided(n->parent);
};
void Terrain::Page::checkEdges() const
{
	quadTree<Patch>::node* n;
	int d, x, y, i;
	for(d=patches->depth()-1; d>=2; d--)
	{
		for(x=0; x<patches->getSideLength(d); x++)
		{
			for(y=0; y<patches->getSideLength(d); y++)
			{
				n = patches->getNode(d,x,y);
				if(n->element.divisionLevel == Patch::LEVEL_USED)
				{
					for(i=0; i<4; i++)
					{
						if(n->neighbors[i] && n->neighbors[i]->parent && n->neighbors[i]->parent->element.divisionLevel == Patch::COMBINED)
						{
							n->neighbors[i]->parent->parent->children[0]->element.divisionLevel = Patch::LEVEL_USED;
							n->neighbors[i]->parent->parent->children[1]->element.divisionLevel = Patch::LEVEL_USED;
							n->neighbors[i]->parent->parent->children[2]->element.divisionLevel = Patch::LEVEL_USED;
							n->neighbors[i]->parent->parent->children[3]->element.divisionLevel = Patch::LEVEL_USED;
							setSubdivided(n->neighbors[i]->parent->parent);
						}
					}
				}
			}
		}
	}
}
void Terrain::Page::patchEdges(quadTree<Patch>::node* n) const
{
	if(n->element.divisionLevel == Patch::SUBDIVIDED)
	{
		patchEdges(n->children[0]);
		patchEdges(n->children[1]);
		patchEdges(n->children[2]);
		patchEdges(n->children[3]);
	}
	else if(n->element.divisionLevel == Patch::LEVEL_USED)
	{
		if(n->neighbors[0] && n->neighbors[0]->element.divisionLevel == Patch::SUBDIVIDED)
		{
			n->neighbors[0]->children[0]->element.edgeFlags |= 8;
			n->neighbors[0]->children[3]->element.edgeFlags |= 8;
		}
		if(n->neighbors[1] && n->neighbors[1]->element.divisionLevel == Patch::SUBDIVIDED)
		{
			n->neighbors[1]->children[0]->element.edgeFlags |= 4;
			n->neighbors[1]->children[1]->element.edgeFlags |= 4;
		}
		if(n->neighbors[2] && n->neighbors[2]->element.divisionLevel == Patch::SUBDIVIDED)
		{
			n->neighbors[2]->children[1]->element.edgeFlags |= 2;
			n->neighbors[2]->children[2]->element.edgeFlags |= 2;
		}
		if(n->neighbors[3] && n->neighbors[3]->element.divisionLevel == Patch::SUBDIVIDED)
		{
			n->neighbors[3]->children[2]->element.edgeFlags |= 1;
			n->neighbors[3]->children[3]->element.edgeFlags |= 1;
		}
	}
}
Terrain::Page::Page(unsigned short* Heights, unsigned int patchResolution, Vec3f position, Vec3f scale, unsigned int lod):minXYZ(position), maxXYZ(position+scale),LOD(lod), heights(Heights)//,trunk(nullptr)
{
	if(!isPowerOfTwo(patchResolution-1))
	{
		unsigned int nPatchResolution = uPowerOfTwo(patchResolution-1) + 1;
		unsigned short* nHeights = new unsigned short[nPatchResolution*nPatchResolution];

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
		delete[] nHeights;
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
		indexBuffers.push_back(vector<shared_ptr<GraphicsManager::indexBuffer>>());
		for(int eFlag=0; eFlag<16; eFlag++)
		{
			int i=0;
			unsigned int spacing = 1 << (levelsDeep-d);	
			for(int y=0; y < 16; y++)
			{
				for(int x = 0; x < 16; x++)
				{
					if((eFlag & 4) && x == 0 && (y%2 == 0))
					{
						if(!((eFlag & 8) && y == 0))
						{
							indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						}
						indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = (x*spacing)			+	((y+2)*spacing)*((width-1)/LOD+1);

						if(!((eFlag & 2) && y == 15))
						{
							indices[i++] = (x*spacing)			+	((y+2)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	((y+2)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						}
					}
					if((eFlag & 8) && y == 0 && (x%2 == 0))
					{
						if(!((eFlag & 4) && x == 0))
						{
							indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						}
						indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+2)*spacing)		+	(y*spacing)*((width-1)/LOD+1);

						if(!((eFlag & 1) && x == 15))
						{
							indices[i++] = ((x+2)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+2)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						}
					}
					if((eFlag & 1) && x == 15 && (y%2 == 0))
					{
						if(!((eFlag & 8) && y == 0)) //XXX
						{
							indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
						}
						indices[i++] = ((x+1)*spacing)			+	(y*spacing)*((width-1)/LOD+1);
						indices[i++] = (x*spacing)				+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+1)*spacing)			+	((y+2)*spacing)*((width-1)/LOD+1);

						if(!((eFlag & 2) && y == 15))
						{
							indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
							indices[i++] = (x*spacing)			+	((y+2)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	((y+2)*spacing)*((width-1)/LOD+1);
						}
					}
					if((eFlag & 2) && y == 15 && (x%2 == 0))
					{
						if(!((eFlag & 4) && x == 0))
						{
							indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
						}

						indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+2)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);

						if(!((eFlag & 1) && x == 15))
						{
							indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+2)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
							indices[i++] = ((x+2)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
						}
					}
					if(!((eFlag & 4) && x == 0) && !((eFlag & 8) && y == 0) && !((eFlag & 1) && x == 15) && !((eFlag & 2) && y == 15)) //default
					{
						indices[i++] = (x*spacing)			+	(y*spacing)*((width-1)/LOD+1);
						indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);

						indices[i++] = ((x+1)*spacing)		+	(y*spacing)*((width-1)/LOD+1);
						indices[i++] = (x*spacing)			+	((y+1)*spacing)*((width-1)/LOD+1);
						indices[i++] = ((x+1)*spacing)		+	((y+1)*spacing)*((width-1)/LOD+1);
					}
				}
			}

			auto iBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
			iBuffer->setData(indices, GraphicsManager::TRIANGLES, i);
			indexBuffers[d].push_back(iBuffer);
		}
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
	
			groundValues[(x + z * width)*4 + 0] = (unsigned short)(32767.5+n.x*32767.5);
			groundValues[(x + z * width)*4 + 1] = (unsigned short)(n.y*65535.0);
			groundValues[(x + z * width)*4 + 2] = (unsigned short)(32767.5+n.z*32767.5);
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

///////////////////////////////////////////////////QUAD TREE///////////////////////////////////////////////////
	
	patches = shared_ptr<quadTree<Patch>>(new quadTree<Patch>(levelsDeep+1));
	
	//unsigned int nPatches = 0;
	//for(int i=0; i<=levelsDeep; i++)
	//{
	//	nPatches += 1 << (2*i);
	//}
	//
	//trunk = new Patch[nPatches];
	//trunk->parent = nullptr;
	//trunk->init(trunk, 0, levelsDeep);

	quadTree<Patch>::node* p;
	float e;
	float r = sqrt(scale.x*scale.x + scale.z*scale.z) / 2;
	unsigned int spacing;
	float errorScale = (maxXYZ.y - minXYZ.y) / USHRT_MAX / 2.0;

	unsigned int x,y,h,k;

	float C = 549.0;//(1.0 / 512.0) / (2.0 * 4.0 / 1024); // A = verticalResoution / (2 * tan(fovy) * nearClipPlaneDistance * maxErrorInPixels) = (nearClipPlane / abs(topClipPlane)) / (2*maxErrorInPixels / verticalResolution)
	for(int l=levelsDeep; l>=0; l--)
	{
		for(x=0; x < patches->getSideLength(l); x++)
		{
			for(y=0; y < patches->getSideLength(l); y++)
			{
				p = patches->getNode(l,x,y);

				p->element.bounds.minXYZ.x = minXYZ.x + scale.x * x / (1<<l);
				p->element.bounds.minXYZ.z = minXYZ.z + scale.z * y / (1<<l);
				p->element.bounds.maxXYZ.x = minXYZ.x + scale.x * (x+1) / (1<<l);
				p->element.bounds.maxXYZ.z = minXYZ.z + scale.z * (y+1) / (1<<l);

				p->element.maxError = 0.0f;
				p->element.minDistanceSquared = 0.0f;
				if(l < levelsDeep)
				{
					spacing = 1 << (levelsDeep-l);
					for(h = x*16*spacing; h < (x*16+15)*spacing; h += spacing)
					{
						for(k = y*16*spacing; k < (y*16+15)*spacing; k += spacing)
						{
							if(heights[h*LOD + k*LOD*width] < p->element.bounds.minXYZ.y)
								p->element.bounds.minXYZ.y = heights[h*LOD + k*LOD*width];

							if(heights[h*LOD + k*LOD*width] > p->element.bounds.maxXYZ.y)
								p->element.bounds.maxXYZ.y = heights[h*LOD + k*LOD*width];

							e = errorScale * abs(heights[h*LOD + k*LOD*width] + heights[(h+spacing)*LOD + k*LOD*width] - 2*heights[(h+spacing/2)*LOD + k*LOD*width]);
							if(e > p->element.maxError) p->element.maxError = e;

							e = errorScale * abs(heights[h*LOD + k*LOD*width] + heights[h*LOD + (k+spacing)*LOD*width] - 2*heights[h*LOD + (k+spacing/2)*LOD*width]);
							if(e > p->element.maxError) p->element.maxError = e;
						}
					}
					p->element.minDistanceSquared = (p->element.maxError * C + r/(1<<l)) * (p->element.maxError * C + r/(1<<l));
				}
				else
				{
					spacing = 1 << (levelsDeep-l);
					for(h = x*16*spacing; h < (x*16+15)*spacing; h += spacing)
					{
						for(k = y*16*spacing; k < (y*16+15)*spacing; k += spacing)
						{
							if(heights[h*LOD + k*LOD*width] < p->element.bounds.minXYZ.y)
								p->element.bounds.minXYZ.y = heights[h*LOD + k*LOD*width];

							if(heights[h*LOD + k*LOD*width] > p->element.bounds.maxXYZ.y)
								p->element.bounds.maxXYZ.y = heights[h*LOD + k*LOD*width];
						}
					}
				}
				p->element.center = (p->element.bounds.minXYZ + p->element.bounds.maxXYZ) * 0.5;
			}
		}
	}
}
void Terrain::Page::generateFoliage(float foliageDensity) //foliageDensity in trees per km^2
{
	texturedVertex3D tmpVerts[16];

	tmpVerts[0].UV = Vec2f(0.0,1.0);
	tmpVerts[1].UV = Vec2f(0.5,1.0);
	tmpVerts[2].UV = Vec2f(0.5,0.0);
	tmpVerts[3].UV = Vec2f(0.0,0.0);

	tmpVerts[4].UV = Vec2f(0.5,1.0);
	tmpVerts[5].UV = Vec2f(1.0,1.0);
	tmpVerts[6].UV = Vec2f(1.0,0.0);
	tmpVerts[7].UV = Vec2f(0.5,0.0);

	tmpVerts[8].UV = Vec2f(0.5,1.0);
	tmpVerts[9].UV = Vec2f(0.0,1.0);
	tmpVerts[10].UV = Vec2f(0.0,0.0);
	tmpVerts[11].UV = Vec2f(0.5,0.0);

	tmpVerts[12].UV = Vec2f(1.0,1.0);
	tmpVerts[13].UV = Vec2f(0.5,1.0);
	tmpVerts[14].UV = Vec2f(0.5,0.0);
	tmpVerts[15].UV = Vec2f(1.0,0.0);

	//texturedVertex3D tmpTopVerts[4];
	//tmpTopVerts[0].UV = Vec2f(0.0,1.0);
	//tmpTopVerts[1].UV = Vec2f(0.5,1.0);
	//tmpTopVerts[2].UV = Vec2f(0.5,0.5);
	//tmpTopVerts[3].UV = Vec2f(0.0,0.5);

	foliageDensity = min(foliageDensity, 300); //keep foliageDensity reasonable

	int foliagePatchesX = 4*(width/16)/LOD;
	int foliagePatchesY = 4*(height/16)/LOD;
	int sLength = 16;
	float inv_sLength = 1.0f/sLength;

	Vec2f			dir;
	Vec3f			right, fwd;
	Vec3f			position;
	unsigned int	numPatches = foliagePatchesX * foliagePatchesY;
	float			patchArea = (maxXYZ.x-minXYZ.x) * (maxXYZ.z-minXYZ.z);
	float			treesPerPatch = patchArea * foliageDensity / (1000*1000) * 0.01;
	float			placementOdds = treesPerPatch / (sLength*sLength);

	vector<texturedVertex3D> vertices;

	double t=GetTime();

	unsigned int nQuads=0;
	unsigned int nQuadsStart=0;



	foliagePatch::plant p;
	foliagePatch patch;

	for(int x=0; x < foliagePatchesX; x++)
	{
		for(int y=0; y < foliagePatchesY; y++)
		{
			unsigned int n = 0;
			nQuadsStart = nQuads;



			for(int h=0; h<sLength; h++)
			{
				for(int k=0; k<sLength; k++)
				{
					if(random<float>() < placementOdds)
					{	//				minX +		total width		*    ()
						position.x = minXYZ.x + (maxXYZ.x-minXYZ.x) * (random<float>(inv_sLength)+h*inv_sLength + x) / foliagePatchesX;
						position.z = minXYZ.z + (maxXYZ.z-minXYZ.z) * (random<float>(inv_sLength)+k*inv_sLength + y) / foliagePatchesY;
						position.y = getHeight(Vec2f(position.x, position.z));
						if(position.y > 40.0)
						{
							n = random<float>(1.0) < 0.5 ? 0 : 8;

							dir = random2<float>();
							float s = (4.0 + random<float>(4.0) + random<float>(4.0))*0.4;
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

							//tmpTopVerts[0].position = position + Vec3f( 3.0*dir.x, 1.0,  3.0*dir.y)*s;
							//tmpTopVerts[1].position = position + Vec3f( 3.0*dir.y, 1.0, -3.0*dir.x)*s;
							//tmpTopVerts[2].position = position + Vec3f(-3.0*dir.x, 1.0, -3.0*dir.y)*s;
							//tmpTopVerts[3].position = position + Vec3f(-3.0*dir.y, 1.0,  3.0*dir.x)*s;

							vertices.push_back(tmpVerts[n+0]);
							vertices.push_back(tmpVerts[n+1]);
							vertices.push_back(tmpVerts[n+2]);
							vertices.push_back(tmpVerts[n+3]);

							vertices.push_back(tmpVerts[n+4]);
							vertices.push_back(tmpVerts[n+5]);
							vertices.push_back(tmpVerts[n+6]);
							vertices.push_back(tmpVerts[n+7]);

							//vertices.push_back(tmpTopVerts[0]);
							//vertices.push_back(tmpTopVerts[1]);
							//vertices.push_back(tmpTopVerts[2]);
							//vertices.push_back(tmpTopVerts[3]);

							foliagePatch::plant p;
							p.location = position;
							p.height = 5.0*s;
							patch.plants.push_back(p);


							nQuads += 2;
						}
					}
				}
			}

			if(nQuads-nQuadsStart > 0)
			{
				patch.center.x = minXYZ.x + (maxXYZ.x-minXYZ.x) * (0.5+x) / foliagePatchesX;
				patch.center.z = minXYZ.z + (maxXYZ.z-minXYZ.z) * (0.5+y) / foliagePatchesY;
				patch.center.y = getHeight(Vec2f(patch.center.x,patch.center.z)) + 12.0;
				Vec3f boundsMin = minXYZ + Vec3f(minXYZ.x + (maxXYZ.x-minXYZ.x) * x / foliagePatchesX, minXYZ.y, minXYZ.z + (maxXYZ.z-minXYZ.z) * y / foliagePatchesY);
				Vec3f boundsMax = minXYZ + Vec3f(minXYZ.x + (maxXYZ.x-minXYZ.x) * (1.0+x) / foliagePatchesX, maxXYZ.y+24.0, minXYZ.z + (maxXYZ.z-minXYZ.z) * (1.0+y) / foliagePatchesY);
				patch.bounds = BoundingBox<float>(boundsMin, boundsMax);


				patch.plantIndexBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);

				unsigned int* indices = new unsigned int[(nQuads-nQuadsStart)*6];
				for(unsigned int i=0; i<nQuads-nQuadsStart; i++)
				{
					indices[i*6+0] = (i+nQuadsStart)*4 + 0;
					indices[i*6+1] = (i+nQuadsStart)*4 + 1;
					indices[i*6+2] = (i+nQuadsStart)*4 + 2;
					indices[i*6+3] = (i+nQuadsStart)*4 + 0;
					indices[i*6+4] = (i+nQuadsStart)*4 + 2;
					indices[i*6+5] = (i+nQuadsStart)*4 + 3;
				}
				patch.plantIndexBuffer->setData(indices, GraphicsManager::TRIANGLES, (nQuads-nQuadsStart)*6);	

				foliagePatches.push_back(patch);
				patch.plants.clear();
			}
		}
	}
	t=GetTime()-t;
	t=GetTime();
	foliageVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STREAM);
	foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
	foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		3*sizeof(float));
	foliageVBO->setTotalVertexSize(sizeof(texturedVertex3D));
	foliageVBO->setVertexData(sizeof(texturedVertex3D)*vertices.size(), !vertices.empty() ? &vertices[0] : nullptr);


	t=GetTime()-t;
	t=GetTime();
}
Vec3f Terrain::Page::rasterNormal(Vec2u loc) const
{
	loc.x = clamp(loc.x, 0, width-1);
	loc.y = clamp(loc.y, 0, height-1);

	float By = (loc.y < height-1)	? ((float)heights[(loc.x)	+ (loc.y+1)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Ay = (loc.x < width-1)	? ((float)heights[(loc.x+1) + (loc.y)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Dy = (  loc.y > 0	)		? ((float)heights[(loc.x)	+ (loc.y-1)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;
	float Cy = (  loc.x > 0	)		? ((float)heights[(loc.x-1)	+ (loc.y)	* width] - heights[loc.x + loc.y*width])/255.0  : 0.0f;

	return Vec3f((Cy - Ay) * width / (maxXYZ.x-minXYZ.x), 2.0 /* /(maxXYZ.y-minXYZ.y)*/, (Dy - By) * height / (maxXYZ.z-minXYZ.z)).normalize();
}
float Terrain::Page::rasterHeight(Vec2u loc) const
{
	return minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[min(loc.x,width-1) + min(loc.y,height-1) * width] / USHRT_MAX;
}
Vec3f Terrain::Page::interpolatedNormal(Vec2f loc) const
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
float Terrain::Page::interpolatedHeight(Vec2f loc) const
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
Vec3f Terrain::Page::getNormal(Vec2f loc) const
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
float Terrain::Page::getHeight(Vec2f loc) const
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
void Terrain::Page::render(shared_ptr<GraphicsManager::View> view) const
{
	renderQueue.clear();

	unsigned int x, y;
	quadTree<Patch>::node* p;
	for(int l=0; l<=levelsDeep; l++)
	{
		for(x=0; x < (0x1<<l); x++)
		{
			for(y=0; y < (0x1<<l); y++)
			{
				p = patches->getNode(l,x,y);
				p->element.edgeFlags = 0;
				p->element.divisionLevel = Patch::COMBINED;
			}
		}
	}
	subdivide(patches->trunk(),view->camera().eye);
	checkEdges();
	patchEdges(patches->trunk());
	for(int l=0; l<=levelsDeep; l++)
	{
		for(unsigned int x=0; x < (1<<l); x++)
		{
			for(unsigned int y=0; y < (1<<l); y++)
			{
				p = patches->getNode(l,x,y);
				if(p->element.divisionLevel == Patch::LEVEL_USED && view->boundingBoxInFrustum(p->element.bounds))
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

	Vec3f eye;
	unsigned int bufferOffset;
	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		//float d = (eye.x-(*i)->center.x)*(eye.x-(*i)->center.x) + (eye.z-(*i)->center.z)*(eye.z-(*i)->center.z);
		//dataManager.setUniform1f("dist", (*i)->minDistanceSquared / d);
		bufferOffset = sizeof(float)*3 * ((*i)->y + ((*i)->x*((width-1)/LOD+1))) * (16<<(levelsDeep-(*i)->level));
		indexBuffers[(*i)->level][(*i)->element.edgeFlags]->bindBuffer(vertexBuffer, bufferOffset);
		indexBuffers[(*i)->level][(*i)->element.edgeFlags]->drawBufferX();
		//graphics->drawLine(Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z), Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z));
		//graphics->drawLine(Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z), Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z));
		//graphics->drawLine(Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.maxXYZ.z), Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z));
		//graphics->drawLine(Vec3f((*i)->bounds.maxXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z), Vec3f((*i)->bounds.minXYZ.x,(*i)->bounds.maxXYZ.y,(*i)->bounds.minXYZ.z));
	}

	//graphics->setDepthTest(false);
	//auto s = shaders.bind("model");
	//dataManager.bind("white");

	//s->setUniform4f("diffuse", 0,0,0,1);
	//graphics->setWireFrame(true);
	//for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	//{
	//	bufferOffset = sizeof(float)*3 * ((*i)->y + ((*i)->x*((width-1)/LOD+1))) * (16<<(levelsDeep-(*i)->level));
	//	indexBuffers[(*i)->level][(*i)->element.edgeFlags]->drawBuffer(GraphicsManager::TRIANGLES, vertexBuffer, bufferOffset);
	//}
	//graphics->setWireFrame(false);
	//graphics->setDepthTest(true);

}
void Terrain::Page::renderFoliage(shared_ptr<GraphicsManager::View> view) const
{
	//skyTexture->bind(1); <-- bound by caller
	bool trees3D = false;

	float distanceSquared;
	for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
	{
		if(!i->plantIndexBuffer || !view->boundingBoxInFrustum(i->bounds)/* || i->center.distanceSquared(view->camera().eye) > 10000.0*10000.0*/)
		{
			i->renderType = foliagePatch::DONT_RENDER;
		}
		else if(!trees3D || i->center.distanceSquared(view->camera().eye) > 250.0*250.0)
		{
			i->renderType = foliagePatch::RENDER_BILLBAORD;
		}
		else
		{
			i->renderType = foliagePatch::RENDER_MODEL;
		}
	}


	graphics->setDepthMask(true);
	dataManager.bind("tree");
	auto alphaTreesShader = shaders.bind("trees alpha test shader");
	alphaTreesShader->setUniform1i("tex", 0);
	alphaTreesShader->setUniform1i("sky", 1);
	alphaTreesShader->setUniform3f("eyePos", view->camera().eye);
	alphaTreesShader->setUniform3f("right", view->camera().right);
	alphaTreesShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	foliageVBO->bindBuffer();
	for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
	{
		if(i->renderType == foliagePatch::RENDER_BILLBAORD)
		{
			i->plantIndexBuffer->bindBuffer();
			i->plantIndexBuffer->drawBufferX();
		}
	}
	graphics->setDepthMask(false);
	auto treesShader = shaders.bind("trees shader");
	treesShader->setUniform1i("tex", 0);
	treesShader->setUniform1i("sky", 1);
	treesShader->setUniform3f("eyePos", view->camera().eye);
	treesShader->setUniform3f("right", view->camera().right);
	treesShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
	{
		if(i->renderType == foliagePatch::RENDER_BILLBAORD)
		{
			i->plantIndexBuffer->bindBuffer();
			i->plantIndexBuffer->drawBufferX();
		}
	}


	auto tree3DShader = shaders("trees3D shader");
	auto leavesMesh = dataManager.getModel("tree leaves");
	auto trunkMesh = dataManager.getModel("tree trunk");
	if(trees3D && tree3DShader && trunkMesh && !trunkMesh->materials.empty() && leavesMesh && !leavesMesh->materials.empty())
	{
		tree3DShader->bind();
		tree3DShader->setUniform1i("tex",0);
		trunkMesh->materials.front().tex->bind(0);
		trunkMesh->materials.front().indexBuffer->bindBuffer(trunkMesh->VBO);
		tree3DShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());

		for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
		{
			if(i->renderType == foliagePatch::RENDER_MODEL)
			{
				for(auto p = i->plants.begin(); p != i->plants.end(); p++)
				{
					tree3DShader->setUniform4f("position_scale",p->location.x, p->location.y, p->location.z, p->height / 23.73);
					trunkMesh->materials.front().indexBuffer->drawBufferX();
				}
			}
		}

		leavesMesh->materials.front().tex->bind(0);
		leavesMesh->materials.front().indexBuffer->bindBuffer(leavesMesh->VBO);
		for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
		{
			if(i->renderType == foliagePatch::RENDER_MODEL)
			{
				for(auto p = i->plants.begin(); p != i->plants.end(); p++)
				{
					tree3DShader->setUniform4f("position_scale",p->location.x, p->location.y, p->location.z, p->height / 23.73);
					leavesMesh->materials.front().indexBuffer->drawBufferX();
				}
			}
		}
	}
}
Terrain::Page::~Page()
{
	delete[] heights;
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
	float constants[5][3] =	{{-0.0193f*L - 0.2592f,		 0.1787f*L - 1.4630f,		-0.0167f*L - 0.2608f},
							 {-0.0665f*L + 0.0008f,		-0.3554f*L + 0.4275f,		-0.0950f*L + 0.0092f},
							 {-0.0004f*L + 0.2125f,		-0.0227f*L + 5.3251f,		-0.0079f*L + 0.2102f},
							 {-0.0641f*L - 0.8989f,		 0.1206f*L - 2.5771f,		-0.0441f*L - 1.6537f},
							 {-0.0033f*L + 0.0452f,		-0.0670f*L + 0.3703f,		-0.0109f*L + 0.0529f}};



	float theta2 = theta*theta;
	float theta3 = theta*theta*theta;
	float chi = (4.0/9.0 - L/120.0)*(PI - 2*theta);
	float zenithLuminance = abs((4.0453*L - 4.9710) * tan(chi) - 0.2155*L + 0.24192);// * 1000;
	float zenith_x = L*L*(0.00166*theta3 - 0.00375*theta2 + 0.00209*theta) + L*(-0.02903*theta3 + 0.06377*theta2 - 0.03202*theta + 0.00394) + (0.11693*theta3 - 0.21196*theta2 + 0.06052*theta + 0.25886);
	float zenith_y = L*L*(0.00275*theta3 - 0.00610*theta2 + 0.00317*theta) + L*(-0.04214*theta3 + 0.08970*theta2 - 0.04153*theta + 0.00516) + (0.15346*theta3 - 0.26756*theta2 + 0.06670*theta + 0.26688);
	float zenithOverSun[3] = {static_cast<float>(0.85f*zenith_x /		((1.0 + constants[0][0] * exp(constants[1][0])) * (1.0f + constants[2][0]*exp(constants[3][0]*(float)theta) + constants[4][0]*cos(theta)*cos(theta)))),
							  static_cast<float>(0.20f*zenithLuminance / ((1.0 + constants[0][1] * exp(constants[1][1])) * (1.0f + constants[2][1]*exp(constants[3][1]*(float)theta) + constants[4][1]*cos(theta)*cos(theta)))),
							  static_cast<float>(0.85f*zenith_y /		((1.0 + constants[0][2] * exp(constants[1][2])) * (1.0f + constants[2][2]*exp(constants[3][2]*(float)theta) + constants[4][2]*cos(theta)*cos(theta))))};

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


		cubeMap[i + 1] = 0.65 * cubeMap[i + 1] / (1.0 + cubeMap[i + 1]);	//tone mapping (optional?)
		//cubeMap[i + 1] = pow((double)cubeMap[i + 1], invGamma);

		cubeMap[i + 0] = cubeMap[i + 1] * x / y;
		cubeMap[i + 2] = cubeMap[i + 1] * (1.0 - x - y) / y;
	}

	t = GetTime() - t;
	t = GetTime();

	//const float invGamma = 1.0/1.8;
	for(i=0; i < l*l*6*3; i+=3)// XYZ -> rgb
	{
	//	float brightness = pow(clamp(cubeMap[i + 0], 0.0, 1.0),invGamma);
		cubeMapTex[i + 2] = clamp((cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.498530),0.0,1.0) * 255.0;
		cubeMapTex[i + 1] = clamp((cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556),0.0,1.0) * 255.0;
		cubeMapTex[i + 0] = clamp((cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311),0.0,1.0) * 255.0;
	}

	t = GetTime() - t;
	t = GetTime();

	skyTexture = graphics->genTextureCube();
	skyTexture->setData(l, l, GraphicsManager::texture::BGR, cubeMapTex);

	delete[] cubeMap;
	delete[] cubeMapTex;

	t = GetTime() - t;
	t = GetTime();
}
void Terrain::generateTreeTexture(shared_ptr<SceneManager::mesh> treeMeshPtr) const
{
/*	const int numDirections=4;
	for(int i=0; i <numDirections; i++)
	{
		shared_ptr<GraphicsManager::texture2D> tex = graphics->genTexture2D();
		tex->setData(1024,1024,GraphicsManager::texture::BGRA, nullptr);
		graphics->setDepthTest(false);


		shared_ptr<GraphicsManager::View> view(new GraphicsManager::View());
		view->perspective(60.0,1.0,0.1,100.0);
		view->lookAt(Vec3f(25.0*sin(2.0*PI*i/numDirections),25.0,25*cos(2.0*PI*i/numDirections)), Vec3f(0,15.0,0), Vec3f(0,1,0));



		graphics->startRenderToTexture(tex);

		graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);

		//for(int n=0;n<10;n++);
		sceneManager.drawMesh(view, dataManager.getModel("tree trunk"), Mat4f(Quat4f(),Vec3f(),Vec3f(1,1,1)));
		sceneManager.drawMesh(view, dataManager.getModel("tree leaves"), Mat4f(Quat4f(),Vec3f(),Vec3f(1,1,1)));

		tex->generateMipmaps();
		graphics->setDepthTest(true);

		graphics->endRenderToTexture();

		unsigned char* texData = tex->getData();

		for(int x=0;x<1024;x++)
		{
			for(int y=0; y<1024;y++)
			{
				if(texData[(x+y*1024)*4+3] < 255 && texData[(x+y*1024)*4+3] != 0)
				{
					texData[(x+y*1024)*4+0] = static_cast<float>(texData[(x+y*1024)*4+0]*255.0) / texData[(x+y*1024)*4+3];
					texData[(x+y*1024)*4+1] = static_cast<float>(texData[(x+y*1024)*4+1]*255.0) / texData[(x+y*1024)*4+3];
					texData[(x+y*1024)*4+2] = static_cast<float>(texData[(x+y*1024)*4+2]*255.0) / texData[(x+y*1024)*4+3];
				}
			}
		}

		shared_ptr<FileManager::textureFile> textureFile(new FileManager::textureFile(string("../treeTexture") + lexical_cast<string>(i) + ".png",FileManager::PNG));
		textureFile->channels = 4;
		textureFile->contents = texData;
		textureFile->width = 1024;
		textureFile->height = 1024;
		fileManager.writeFile(textureFile);
	}*/
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

				n = Vec3f(Cy - Ay, 0.06, Dy - By).normalize();

				textureData[(x + y*resolution + z * resolution * resolution)*4 + 2] = (n.x + 1.0) * 0.5 * 255.0;
				textureData[(x + y*resolution + z * resolution * resolution)*4 + 1] = n.y * 255.0;
				textureData[(x + y*resolution + z * resolution * resolution)*4 + 0] = (n.z + 1.0) * 0.5 * 255.0;
				textureData[(x + y*resolution + z * resolution * resolution)*4 + 3] = heights[x + y*resolution + z * resolution * resolution] * 255.0;
			}
		}
	}
	t = GetTime() - t;
	t = GetTime();

	oceanTexture = graphics->genTexture3D();
	oceanTexture->setData(resolution, resolution, depth, GraphicsManager::texture::BGRA, textureData, true);

	delete[] heights;
	delete[] layer;
	delete[] textureData;
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
void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, TerrainType shader, float foliageDensity, unsigned int LOD)
{
	shaderType = shader;
	waterPlane = (shader==TERRAIN_ISLAND);
	shared_ptr<Page> p(new Page(Heights,patchResolution,position,scale,LOD));
	p->generateFoliage(foliageDensity);
	terrainPages.push_back(p);
	terrainPosition = position;
	terrainScale = scale;
	mBounds = Circle<float>(Vec2f(position.x + scale.x * 0.5, position.z + scale.z * 0.5), max(scale.x, scale.z)/2.0);

	Vec3f sun = (graphics->getLightPosition()).normalize();

	generateSky(acos(sun.y), atan2(sun.z,sun.x), 1.8); //should actually make the sun be at the position of the light source...
	//generateOceanTexture(); //we now load this texture from disk to reduce startup time

	generateTreeTexture(dataManager.getModel("tree model"));

	graphics->checkErrors();

//	skyTexture = static_pointer_cast<GraphicsManager::textureCube>(dataManager.getTexture("skybox"));

}
void Terrain::renderTerrain(shared_ptr<GraphicsManager::View> view) const
{
	Vec3f eye = view->camera().eye;
	Vec3f center = Vec3f(eye.x,0,eye.z);
	float h = eye.y;	//height off the ground
	float r = 6367500;	//radius of the earth

	double radius = max(h*tan(asin(r/(r+h))), 2.0*max(terrainScale.x,terrainScale.z));
	//radius = 20000.0;
	//float h2 = sqrt(2.0*r*r+2.0*r*h+h*h) / r;

	graphics->setDepthMask(false);
	graphics->setDepthTest(false);
	auto sky = shaders.bind("sky shader");

	sky->setUniform1i("tex", 0);
	sky->setUniform1i("noise", 1);
	skyTexture->bind();
	dataManager.bind("noise",1);
	sceneManager.drawMeshCustomShader(view, dataManager.getModel("sky dome"), Mat4f(Quat4f(), Vec3f(eye.x,0,eye.z), radius), sky);
	if(!waterPlane)
	{
		sceneManager.drawMeshCustomShader(view, dataManager.getModel("sky dome"), Mat4f(Quat4f(), Vec3f(eye.x,0,eye.z), Vec3f(radius,-radius,radius)), sky);
	}

	//Vec3f sunDirection = (graphics->getLightPosition()+eye).normalize();
	//Vec3f sunPos = view->project3((graphics->getLightPosition()+eye).normalize() * 300000.0);
	//if(sunPos.z < 1.0)
	//{
	//	shaders.bind("ortho");
	//
	//	Vec3f sunUp = sunDirection.cross(Vec3f(0,0,1));	//sun cannot be (0,0,1) at!!!
	//	float up = view->camera().up.dot(sunUp);
	//	float right = view->camera().right.dot(sunUp);

	//	Angle rotateAng = atan2A(up,right);
	//	graphics->drawRotatedOverlay(Rect::CWH(sunPos.x,sunPos.y,0.25,0.25), rotateAng, "sun"); //sun get stretched in split screen mode...
	//}

	if(waterPlane)
	{
		auto ocean = shaders.bind("ocean");

		dataManager.bind("ocean normals", 1);
		//oceanTexture->bind(1);

		ocean->setUniform1i("sky",	0);
		ocean->setUniform1i("oceanNormals",	1);
		ocean->setUniform1f("time",	world.time());
		ocean->setUniform1f("seaLevel",0);
		ocean->setUniform2f("center",	center.x,center.z);
		ocean->setUniform3f("eyePos", eye.x, eye.y, eye.z);
		ocean->setUniform1f("scale", radius);

		ocean->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
		ocean->setUniformMatrix("modelTransform", Mat4f());

		//graphics->setWireFrame(true);

		ocean->setUniform3f("lightPosition", graphics->getLightPosition());
		sceneManager.drawMeshCustomShader(view, dataManager.getModel("disk"), Mat4f(Quat4f(), center, Vec3f(radius,1,radius)), ocean);
	
		//graphics->setWireFrame(false);

		//graphics->drawModelCustomShader("disk",center,Quat4f(),Vec3f(radius,1,radius));
	}

	graphics->setDepthMask(true);
	graphics->setDepthTest(true);
	if(shaderType == TERRAIN_ISLAND)
	{
		auto islandShader = shaders.bind("island terrain");
		dataManager.bind("sand",2);
		dataManager.bind("grass",3);
		dataManager.bind("rock",4);
		dataManager.bind("LCnoise",5);
		dataManager.bind("grass normals",6); //can take 100+ ms to complete under linux?
		dataManager.bind("noise",7);

		islandShader->setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());
		islandShader->setUniformMatrix("modelTransform",	Mat4f());
		islandShader->setUniform1f("time",					world.time());
		islandShader->setUniform3f("lightPosition",			graphics->getLightPosition());
		islandShader->setUniform3f("eyePos",				view->camera().eye);
		islandShader->setUniform3f("invScale",				1.0/(terrainScale.x),1.0/(terrainScale.y),1.0/(terrainScale.z));
		islandShader->setUniform1f("minHeight",				terrainPosition.y);
		islandShader->setUniform1f("heightRange",			terrainScale.y);

		islandShader->setUniform1i("sky",			0);
		islandShader->setUniform1i("sand",		2);
		islandShader->setUniform1i("grass",		3);
		islandShader->setUniform1i("rock",		4);
		islandShader->setUniform1i("LCnoise",		5);
		islandShader->setUniform1i("groundTex",	1);
		islandShader->setUniform1i("grass_normals", 6);
		islandShader->setUniform1i("noiseTex",	7);			// make sure uniform exists

		sceneManager.bindLights(islandShader);

		for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
		{
			(*i)->render(view);
		}
	}
	else if(shaderType == TERRAIN_MOUNTAINS)
	{
		auto mountainShader = shaders.bind("mountain terrain");
		dataManager.bind("sand",2);
		dataManager.bind("grass",3);
		dataManager.bind("rock",4);
		dataManager.bind("LCnoise",5);
		dataManager.bind("grass normals",6); //can take 100+ ms to complete under linux?
		dataManager.bind("noise",7);

		mountainShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
		mountainShader->setUniformMatrix("modelTransform",	Mat4f());
		mountainShader->setUniform1f("time",				world.time());
		mountainShader->setUniform3f("lightPosition",		graphics->getLightPosition());
		mountainShader->setUniform3f("eyePos",				view->camera().eye);
		mountainShader->setUniform3f("invScale",			1.0/(terrainScale.x),1.0/(terrainScale.y),1.0/(terrainScale.z));
		mountainShader->setUniform1f("minHeight",			terrainPosition.y);
		mountainShader->setUniform1f("heightRange",			terrainScale.y);
		


		mountainShader->setUniform1i("sky",			0);
		mountainShader->setUniform1i("sand",		2);
		mountainShader->setUniform1i("grass",		3);
		mountainShader->setUniform1i("rock",		4);
		mountainShader->setUniform1i("LCnoise",		5);
		mountainShader->setUniform1i("groundTex",	1);
		mountainShader->setUniform1i("grass_normals", 6);
		mountainShader->setUniform1i("noiseTex",	7);			// make sure uniform exists

		sceneManager.bindLights(mountainShader);

		for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
		{
			mountainShader->setUniform2f("invTerrainResolution",	1.0/(*i)->width, 1.0/(*i)->height);
			(*i)->render(view);
		}
	}
	else if(shaderType == TERRAIN_SNOW)
	{
		auto snowShader = shaders.bind("snow terrain");
		dataManager.bind("snow",2);
		dataManager.bind("LCnoise",3);
		dataManager.bind("snow normals",4);

		snowShader->setUniform1f("maxHeight",	terrainPosition.y + terrainScale.y);//shader should just accept minXYZ and maxXYZ vectors
		snowShader->setUniform1f("minHeight",	terrainPosition.y);
		snowShader->setUniform1f("XZscale",		terrainScale.x);
		snowShader->setUniform1f("time",		world.time());

		//snowShader->setUniform3f("lightPosition", graphics->getLightPosition());

		snowShader->setUniform1i("snow",			2);
		snowShader->setUniform1i("LCnoise",			3);
		snowShader->setUniform1i("groundTex",		1);
		snowShader->setUniform1i("snow_normals",	4);

		sceneManager.bindLights(snowShader);

		for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
		{
			(*i)->render(view);
		}
	}
	else if(shaderType == TERRAIN_DESERT)
	{
		auto desertShader = shaders.bind("desert terrain");

		dataManager.bind("desertSand",2);
		dataManager.bind("sand", 3);
		dataManager.bind("LCnoise",4);
		dataManager.bind("noise",5);

		desertShader->setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());
		desertShader->setUniformMatrix("modelTransform",	Mat4f());
		desertShader->setUniform1f("time",					world.time());
		desertShader->setUniform3f("lightPosition",			graphics->getLightPosition());
		desertShader->setUniform3f("eyePos",				view->camera().eye);
		desertShader->setUniform3f("invScale",				1.0/(terrainScale.x),1.0/(terrainScale.y),1.0/(terrainScale.z));

		desertShader->setUniform1i("sky",		0);
		desertShader->setUniform1i("groundTex",	1);
		desertShader->setUniform1i("sand",		2);
		desertShader->setUniform1i("sand2",		3);
		desertShader->setUniform1i("LCnoise",	4);
		desertShader->setUniform1i("noiseTex",	5);

		sceneManager.bindLights(desertShader);

		for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
		{
			(*i)->render(view);
		}
	}

	shaders.bind("model");
	if(waterPlane)
	{
		graphics->setColorMask(false);
		sceneManager.drawMesh(view, dataManager.getModel("disk"), Mat4f(Quat4f(), center, Vec3f(radius,1,radius)));
		//graphics->drawModel("disk",center,Quat4f(),Vec3f(radius,1,radius));
		graphics->setColorMask(true);
	}


		//Vec3f right = view->camera().fwd.cross(Vec3f(0,1,0)).normalize();
		//graphics->setAlphaToCoverage(true);
		//dataManager.bind("tree");
		//skyTexture->bind(1);

		//if(graphics->getMultisampling())
		//	dataManager.bind("trees shader");
		//else
		//	dataManager.bind("trees alpha test shader");
		//
		//dataManager.setUniform1i("tex", 0);
		//dataManager.setUniform1i("sky", 1);
		//dataManager.setUniform3f("eyePos", eye);
		//
		//dataManager.setUniform3f("right", right);
		//dataManager.setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
		//
		//foliageIBO->drawBuffer(GraphicsManager::TRIANGLES,foliageVBO);
		//graphics->setAlphaToCoverage(false);
	


	//graphics->setDepthMask(false);
	//dataManager.bind("cirrus cloud shader");
	//dataManager.bind("noise");
	//dataManager.setUniform1i("tex", 0);
	//dataManager.setUniform3f("eyePos", view->camera().eye);
	//graphics->drawModelCustomShader("sky dome",Vec3f(eye.x,900,eye.z),Quat4f(),Vec3f(radius,100,radius));
	//graphics->setDepthMask(true);
}
void Terrain::renderFoliage(shared_ptr<GraphicsManager::View> view) const
{
	//for(auto i=trees.begin(); i!=trees.end(); i++)
	//{
	//	sceneManager.drawMesh(view, dataManager.getModel("tree model"), Mat4f(Quat4f(),i->location));
	//}
	skyTexture->bind(1);
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		(*i)->renderFoliage(view);
	}

	//dataManager.bind("tree");
	//skyTexture->bind(1);
	//
	//auto alphaTreesShader = shaders.bind("trees alpha test shader");
	//alphaTreesShader->setUniform1i("tex", 0);
	//alphaTreesShader->setUniform1i("sky", 1);
	//alphaTreesShader->setUniform3f("eyePos", view->camera().eye);
	//alphaTreesShader->setUniform3f("right", view->camera().right);
	//alphaTreesShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	//for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	//{
	//	(*i)->renderFoliage(view);
	//}
	//
	//graphics->setDepthMask(false);
	//auto treesShader = shaders.bind("trees shader");
	//treesShader->setUniform1i("tex", 0);
	//treesShader->setUniform1i("sky", 1);
	//treesShader->setUniform3f("eyePos", view->camera().eye);
	//treesShader->setUniform3f("right", view->camera().right);
	//treesShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	//for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	//{
	//	(*i)->renderFoliage(view);
	//}
}
shared_ptr<Terrain::Page> Terrain::getPage(Vec2f position) const
{
	for(auto i=terrainPages.begin(); i!=terrainPages.end(); i++)
	{
		if((*i)->minXYZ.x <= position.x && (*i)->maxXYZ.x >= position.x && (*i)->minXYZ.z <= position.y && (*i)->maxXYZ.z >= position.y)
		{
			return *i;
		}
	}
	return shared_ptr<Page>();
}
shared_ptr<Terrain::Page> Terrain::getPage(Vec3f position) const // position.y is ignored
{
	for(auto i=terrainPages.begin(); i!=terrainPages.end(); i++)
	{
		if((*i)->minXYZ.x <= position.x && (*i)->maxXYZ.x >= position.x && (*i)->minXYZ.z <= position.z && (*i)->maxXYZ.z >= position.z)
		{
			return *i;
		}
	}
	return shared_ptr<Page>();
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
//
//void checkPatch(quadTree<Terrain::Patch>::node* n, const Vec3f& rayStart, const Vec3f& dirfrac, vector<quadTree<Patch>::node*>& potentialHits)
//{
//	float t1 = (n->element.bounds.minXYZ.x - rayStart.x)*dirfrac.x;
//	float t2 = (n->element.bounds.maxXYZ.x - rayStart.x)*dirfrac.x;
//	float t3 = (n->element.bounds.minXYZ.y - rayStart.y)*dirfrac.y;
//	float t4 = (n->element.bounds.maxXYZ.y - rayStart.y)*dirfrac.y;
//	float t5 = (n->element.bounds.minXYZ.z - rayStart.z)*dirfrac.z;
//	float t6 = (n->element.bounds.maxXYZ.z - rayStart.z)*dirfrac.z;
//
//	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
//	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
//
//	if(!(tmax < 0 || tmin > tmax))
//	{
//		if(n->children[0] && n->children[1] && n->children[2] && n->children[3])
//		{
//			checkPatch(n->children[0]);
//		}
//
//	}
//}
//
//bool Terrain::rayIntersection(Vec3f rayStart, Vec3f direction, Vec3f& intersectionPoint) const
//{	//see: http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
//	direction = direction.normalize();
//	Vec3f dirfrac = Vec3f(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
//
//
//
//	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
//	// r.org is origin of ray
//
//	float t1, t2, t3, t4, t5, t6, tmin, tmax;
//	vector<quadTree<Patch>::node*> potentialHits;
//
//	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
//	{
//
//		checkPatch((*i)->patches->trunk()->);
//	}
//
//	return true;
//}