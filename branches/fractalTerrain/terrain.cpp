
#include "engine.h"

unsigned int Terrain::FractalNode::totalNodes = 0;
unsigned int Terrain::FractalNode::frameNumber = 0;
shared_ptr<GraphicsManager::vertexBuffer> Terrain::FractalNode::vertexBuffer;
std::array<shared_ptr<GraphicsManager::indexBuffer>, 16> Terrain::FractalNode::indexBuffers;
shared_ptr<GraphicsManager::texture2D> Terrain::FractalNode::subdivideTexture;
shared_ptr<GraphicsManager::texture2D> Terrain::FractalNode::queryTexture;

Terrain::ClipMap::ClipMap(unsigned int lResolution, float sLength):
	layerResolution(lResolution), sideLength(sLength)
{
	
}
void Terrain::ClipMap::addLayer(unique_ptr<float[]> heights)
{
	layers.emplace_back();
	auto& layer = layers.back();
	layer.heights = std::move(heights);
	layer.minHeight = layer.maxHeight = layer.heights[0];
	for(unsigned int i = 1; i < layerResolution*layerResolution; i++){
		layer.minHeight = min(layer.minHeight, layer.heights[i]);
		layer.maxHeight = max(layer.maxHeight, layer.heights[i]);
	}

	unique_ptr<unsigned short[]> groundValues(
		new unsigned short[layerResolution*layerResolution*4]);

	float hRange = layer.maxHeight - layer.minHeight;
	for(unsigned int x=0; x < layerResolution; x++)
	{
		for(unsigned int z=0; z < layerResolution; z++)
		{
			float xSlope = 0, zSlope = 0;
			if(x > 0 && x < layerResolution-1)
				xSlope = (layer.heights[(x+1)+z*layerResolution] 
						  - layer.heights[(x-1)+z*layerResolution]) / 2;

			if(z > 0 && z < layerResolution-1)
				zSlope = (layer.heights[x+(z+1)*layerResolution] 
						  - layer.heights[x+(z-1)*layerResolution]) / 2;
			
			groundValues[(x + z * layerResolution)*4] =	USHRT_MAX *
				(layer.heights[x+z*layerResolution] - layer.minHeight) / hRange;
			groundValues[(x + z * layerResolution)*4 + 1] = 
				USHRT_MAX * (0.5 + xSlope / hRange);
			groundValues[(x + z * layerResolution)*4 + 2] = 
				USHRT_MAX * (0.5 + zSlope / hRange);
		}
	}
	layer.texture = graphics->genTexture2D();
	layer.texture->setData(layerResolution, layerResolution,
						   GraphicsManager::texture::RGBA16, false, false,
						   (unsigned char*)groundValues.get());
}

void Terrain::FractalNode::recursiveEnvoke(std::function<void(FractalNode*)> func)
{
	func(this);
	for(auto& c : children)
	{
		if(c)
		{
			c->recursiveEnvoke(func);
		}
	}
}
void Terrain::FractalNode::reverseRecursiveEnvoke(std::function<void(FractalNode*)> func)
{
	func(this);
	if(parent)
	{
		parent->reverseRecursiveEnvoke(func);
	}
}
Terrain::FractalNode::~FractalNode()
{
	totalNodes--;
}
Terrain::FractalNode::FractalNode(FractalNode* parent_, 
								  unsigned int level_, 
								  Vec2i coordinates_, 
								  shared_ptr<ClipMap> clipMap_): 
	divisionLevel(DivisionLevel::COMBINED),waterDivisionLevel(DivisionLevel::COMBINED), level(level_), coordinates(coordinates_), parent(parent_), clipMap(clipMap_)
{
	totalNodes++;
	lastUseFrame = frameNumber;

	if(level <= 1)
	{
		clipMapLayer = 0;
	}
	else
	{
		clipMapLayer = 0;//min(layer - 1, clipMapLayer < clipMap->layers.size());
		/*unsigned int x = coordinates.x * (tileResolution-1);
		  unsigned int y = coordinates.y * (tileResolution-1);
		  unsigned int cx = (clipMap->layerResolution << clipLayer) / 2;
		
		  while(clipMapLayer > 0)
		  {
		  x >>= 1;
		  y >>= 1;

		  if()
		  cli
		  ++clipMapLayer;
		  layerSize = layerSize / 2 + 1;
		  }
		  clipMapLayer = min(layer - 1, clipMap->layers.size());*/




	}

	unsigned int layerSize = ((tileResolution-1) << level) + 1;
	unsigned int clipMapLayerSize = ((clipMap->layerResolution-1)<<clipMapLayer) + 1;
	clipMapStep = (float)(clipMapLayerSize-1) / (layerSize-1);
	clipMapOrigin = coordinates * (tileResolution-1) * clipMapStep
		- clipMap->layerResolution * ((1 << clipMapLayer) - 1) / 2;

	sideLength = clipMap->sideLength / (1 << level);

	//minHeight = maxHeight = 
	//for(int x = clipMapOrigin.x; x <= clipMapOrigin.x + ceil(clipMapStep*tileResolution); x++){
	//	for(int z = clipMapOrigin.y; z <= clipMapOrigin.y + ceil(clipMapStep*tileResolution); z++){
	//		float h = clipMap->layers[clipMapLayer].heights[]
	//	}
	//}

	minHeight = maxHeight = clipMap->layers[clipMapLayer].heights[
		clipMapOrigin.x + clipMapOrigin.y * clipMap->layerResolution];

	auto& heights = clipMap->layers[clipMapLayer].heights;
	for(unsigned int x = 0; x < clipMapStep * (tileResolution-1)+1; x++)
	{
		for(unsigned int y = 0; y < clipMapStep * (tileResolution-1)+1; y++)
		{
			float h = heights[x+clipMapOrigin.x + (y+clipMapOrigin.y)*clipMap->layerResolution];
			minHeight = min(minHeight, h);
			maxHeight = max(maxHeight, h);

		}
	}
	
	//minHeight = clipMap->layers[clipMapLayer].minHeight;
	//maxHeight = clipMap->layers[clipMapLayer].maxHeight;
	
	texture = graphics->genTexture2D();
	texture->setData(textureResolution, textureResolution, GraphicsManager::texture::Format::RGBA16, false, false, nullptr);

	if(!parent/* || clipMapStep*(layerResolution-1)+1 >= textureResolution*/)
	{
		float p = 1.0 - 1.0 / textureResolution;
		float t = 0.5 / clipMap->layers[clipMapLayer].texture->getWidth();
		graphics->startRenderToTexture(texture, 0, nullptr, 0, true);
		graphics->setBlendMode(GraphicsManager::REPLACE);
		auto shader = shaders.bind("fractal subdivide");
		shader->setUniform1f("amplitudeScale", 0.03 / (1 << level));
		shader->setUniform1i("tex", 0);
		graphics->drawPartialOverlay(Rect::XYXY(-p, -p, p, p), 
									 Rect::XYWH(t, t, 1-t, 1-t),
									 clipMap->layers[clipMapLayer].texture);
		graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);
		graphics->endRenderToTexture();
		texture->generateMipmaps();
	}
	else
	{
		auto& layer = clipMap->layers[clipMapLayer];
		Vec2f tOrigin = Vec2f(coordinates)/2 - Vec2f(parent->coordinates);
		Vec2i textureOrigin = tOrigin * textureResolution;
		float heightRange = layer.maxHeight - layer.minHeight;
//		float slopeScale = heightRange * (clipMap->layerResolution-1)
//			/ clipMap->sideLength;
		//Subdivide X
		graphics->startRenderToTexture(texture, 0, nullptr, 0, false);
		graphics->setBlendMode(GraphicsManager::REPLACE);
		auto shader = shaders.bind("fractal subdivideX");
		shader->setUniform1f("amplitudeScale",0.00007*sideLength/(tileResolution*(1<<level)));
		shader->setUniform1f("slopeScale", 1.0*(clipMap->layerResolution-1)
							 / (textureResolution-1) * (1 << level));
		shader->setUniform2i("textureOrigin", textureOrigin.x, textureOrigin.y);
		shader->setUniform2i("origin", coordinates.x * (textureResolution-1),
							 coordinates.y * (textureResolution-1));
		shader->setUniform1i("tex", 0);
		shader->setUniform1f("levelScale", 2.0 / (1 << level));
		shader->setUniform1f("invLevelScale", 0.5 * (1 << level));
		graphics->drawPartialOverlay(Rect::XYXY(-1,-1,1,1), 
									 Rect::XYWH(tOrigin.x,tOrigin.y,0.5,0.5),
									 parent->texture);
		graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);
		graphics->endRenderToTexture();
		texture->generateMipmaps();
		//subdivide Y
/*		graphics->startRenderToTexture(texture, 0, nullptr, 0, false);
		shader = shaders.bind("fractal subdivide");
		shader->setUniform1f("amplitudeScale", 0.03 / (1 << level));
		shader->setUniform2f("origin", coordinates * textureResolution);
		shader->setUniform1i("tex", 0);
		graphics->drawPartialOverlay(Rect::XYXY(-1,-1,1,1), 
									 Rect::XYWH(0,0,1,1),
									 subdivideTexture);
		graphics->endRenderToTexture();
		texture->generateMipmaps();
*/	}

	origin = Vec2f((coordinates.x - 0.5*(1<<level))*sideLength,
				   (coordinates.y - 0.5*(1<<level))*sideLength);

	worldCenter = Vec3f(origin.x+0.5*sideLength,
						0.5*(maxHeight-minHeight),
						origin.y+0.5*sideLength);

	worldBounds.minXYZ = Vec3f(origin.x, minHeight, origin.y);
	worldBounds.maxXYZ = Vec3f(origin.x+sideLength, maxHeight, origin.y+sideLength);

	computeError();

//	if(clipMapStep > 1 || clipMapLayer+1 < clipMap->layers.size()
//	   || (parent && parent->clipMapStep > 1))
	//if(sideLength / (tileResolution-1) > 30.0)
	//{
	//	subdivide();
	//}
}
void Terrain::FractalNode::computeError()
{
	auto& heights = clipMap->layers[clipMapLayer].heights;
	for(unsigned int x = 0; x < clipMapStep * (tileResolution-1)+1; x++)
	{
		for(unsigned int y = 0; y < clipMapStep * (tileResolution-1)+1; y++)
		{
			float h = heights[x+clipMapOrigin.x + (y+clipMapOrigin.y)*clipMap->layerResolution];
			minHeight = min(minHeight, h);
			maxHeight = max(maxHeight, h);

		}
	}

	minDistance = sideLength / (tileResolution-1) * 15;
}
/**
 * Preconditions: neighbors are valid. This means that it contains null values
 * only if they represent the edge of the map or nodes that have not been 
 * subdivided that far yet.
 *
 * Note that the precondition implies that subdivide must not be applied 
 * recursively, and thus FractalNode() cannot subdivide on itself.
 *
 * Postcondition: all children are initialized and have valid neighbors fields
 */
void Terrain::FractalNode::subdivide()
{
	if(!children[0])
		children[0] = std::make_shared<FractalNode>(this, level+1, coordinates*2, clipMap);
	if(!children[1])
		children[1] = std::make_shared<FractalNode>(this, level+1, coordinates*2 + Vec2i(1,0), clipMap);
	if(!children[2])
		children[2] = std::make_shared<FractalNode>(this, level+1, coordinates*2 + Vec2i(0,1), clipMap);
	if(!children[3])
		children[3] = std::make_shared<FractalNode>(this, level+1, coordinates*2 + Vec2i(1,1), clipMap);

	//child 0
	if(!neighbors[0].expired()) children[0]->neighbors[0] = neighbors[0].lock()->children[1];
	if(!neighbors[2].expired()) children[0]->neighbors[2] = neighbors[2].lock()->children[2];
	if(!children[0]->neighbors[0].expired()) children[0]->neighbors[0].lock()->neighbors[1] = children[0]; 
	if(!children[0]->neighbors[2].expired()) children[0]->neighbors[2].lock()->neighbors[3] = children[0]; 
	children[0]->neighbors[1] = children[1];
	children[0]->neighbors[3] = children[2];

	//child 1
	if(!neighbors[1].expired()) children[1]->neighbors[1] = neighbors[1].lock()->children[0];
	if(!neighbors[2].expired()) children[1]->neighbors[2] = neighbors[2].lock()->children[3];
	if(!children[1]->neighbors[1].expired()) children[1]->neighbors[1].lock()->neighbors[0] = children[1]; 
	if(!children[1]->neighbors[2].expired()) children[1]->neighbors[2].lock()->neighbors[3] = children[1]; 
	children[1]->neighbors[0] = children[0];
	children[1]->neighbors[3] = children[3];

	//child 2
	if(!neighbors[0].expired()) children[2]->neighbors[0] = neighbors[0].lock()->children[3];
	if(!neighbors[3].expired()) children[2]->neighbors[3] = neighbors[3].lock()->children[0];
	if(!children[2]->neighbors[0].expired()) children[2]->neighbors[0].lock()->neighbors[1] = children[2]; 
	if(!children[2]->neighbors[3].expired()) children[2]->neighbors[3].lock()->neighbors[2] = children[2]; 
	children[2]->neighbors[1] = children[3];
	children[2]->neighbors[2] = children[0];

	//child 3
	if(!neighbors[1].expired()) children[3]->neighbors[1] = neighbors[1].lock()->children[2];
	if(!neighbors[3].expired()) children[3]->neighbors[3] = neighbors[3].lock()->children[1];
	if(!children[3]->neighbors[1].expired()) children[3]->neighbors[1].lock()->neighbors[0] = children[3]; 
	if(!children[3]->neighbors[3].expired()) children[3]->neighbors[3].lock()->neighbors[2] = children[3]; 
	children[3]->neighbors[0] = children[2];
	children[3]->neighbors[2] = children[1];
}
void Terrain::FractalNode::pruneGrandchildren()
{
	if(!children[0])
		return;


	for(auto& c : children)
	{
		if(!c->children[0])
			continue;

		double lFrame = min(min(children[0]->lastUseFrame,
								children[1]->lastUseFrame),
							min(children[2]->lastUseFrame,
								children[3]->lastUseFrame));

		if(frameNumber - lFrame >= 150){
			for(auto& g : c->children)
				g.reset();
		}
	}
}
unsigned int Terrain::FractalNode::computeSubdivision(shared_ptr<GraphicsManager::View> view, unsigned int maxDivisions)
{
	unsigned int divisions = 0;
	const unsigned int maxTiles = 250;
	unsigned int tiles = 1;
	std::queue<FractalNode*> nodes;
	std::queue<FractalNode*> usedNodes;
	nodes.push(this);
	recursiveEnvoke([](FractalNode* f){f->divisionLevel=DivisionLevel::COMBINED;});

	while(divisions < maxDivisions && tiles < maxTiles && !nodes.empty())
	{
		auto n = nodes.front();
		nodes.pop();

		if(!view->boundingBoxInFrustum(n->worldBounds)){
			n->recursiveEnvoke([](FractalNode* f){f->divisionLevel=DivisionLevel::FRUSTUM_CULLED;});
			n->pruneGrandchildren();
			tiles--;
		}
		else
		{
			Vec3f eye = view->camera().eye;
			float hMinDistance = 0.5*n->sideLength + n->minDistance;

			if(eye.x > n->worldCenter.x - hMinDistance && 
			   eye.x < n->worldCenter.x + hMinDistance &&
			   eye.y < n->maxHeight + n->minDistance && 
			   eye.y > n->minHeight - n->minDistance &&
			   eye.z > n->worldCenter.z - hMinDistance && 
			   eye.z < n->worldCenter.z + hMinDistance &&
			   n->sideLength / (tileResolution-1) > 15.0 &&
			   n->maxHeight > -2.0f)
			{
				n->divisionLevel = DivisionLevel::SUBDIVIDED;
				if(!n->children[0])
				{
					divisions++;
					n->subdivide();
				}
		
				tiles--;
				for(auto& c : n->children)
				{
					tiles++;
					nodes.push(c.get());
				}
			}
			else
			{
				n->recursiveEnvoke([](FractalNode* f){f->divisionLevel=DivisionLevel::COMBINED;});
				n->divisionLevel = DivisionLevel::LEVEL_USED;
				usedNodes.push(n);
			}
		}
	}
	while(!nodes.empty())
	{
		if(view->boundingBoxInFrustum(nodes.front()->worldBounds))
		{
			nodes.front()->recursiveEnvoke([](FractalNode* f){f->divisionLevel=DivisionLevel::COMBINED;});
			nodes.front()->divisionLevel = DivisionLevel::LEVEL_USED;
			usedNodes.push(nodes.front());
		}
		else
		{
			nodes.front()->recursiveEnvoke([](FractalNode* f){f->divisionLevel=DivisionLevel::FRUSTUM_CULLED;});
			nodes.front()->pruneGrandchildren();
			tiles--;
		}
		nodes.pop();
	}

	while(!usedNodes.empty())
	{
		auto node = usedNodes.front();
		auto checkNeighbor = [node](unsigned int neighbor, unsigned int child1, unsigned int child2)->bool{
			shared_ptr<FractalNode> n = node->neighbors[neighbor].lock();
			if(n && n->divisionLevel == DivisionLevel::SUBDIVIDED)
			{
				auto c1 = n->children[child1];
				auto c2 = n->children[child2];
				return (c1 && c1->divisionLevel == DivisionLevel::SUBDIVIDED) ||
					(c2 && c2->divisionLevel == DivisionLevel::SUBDIVIDED);
			}
			return false;
		};

		FractalNode* ptr;
		if(checkNeighbor(0, 1, 3) || checkNeighbor(1, 0, 2) || checkNeighbor(2, 2, 3) || checkNeighbor(3, 0, 1))
		{
			node->divisionLevel = DivisionLevel::SUBDIVIDED;
			if(!node->children[0])
			{
				divisions++;
				node->subdivide();
			}
		
			tiles--;
			for(auto& c : node->children)
			{
				if(view->boundingBoxInFrustum(c->worldBounds))
				{
					tiles++;
					c->divisionLevel = DivisionLevel::LEVEL_USED;
					usedNodes.push(c.get());
				}
				else
				{
					c->divisionLevel = DivisionLevel::FRUSTUM_CULLED;
					c->pruneGrandchildren();
				}
			}
		}
		else
		{
			node->pruneGrandchildren();
		}

		usedNodes.pop();
	}


//	cout << tiles << " (" << ((totalNodes * 257 * 257 * 8) >> 20) << " MiB)" << endl;

	return divisions;
}
unsigned int Terrain::FractalNode::computeWaterSubdivision(shared_ptr<GraphicsManager::View> view, unsigned int maxDivisions)
{
	unsigned int divisions = 0;
	std::queue<FractalNode*> nodes;
	nodes.push(this);

	while(divisions < maxDivisions && !nodes.empty())
	{
		auto n = nodes.front();
		nodes.pop();

		BoundingBox<float> bounds = n->worldBounds;
		Vec2f closestPoint = Vec2f(min(abs(bounds.minXYZ.x), 
									   abs(bounds.maxXYZ.x)),
								   min(abs(bounds.minXYZ.z), 
									   abs(bounds.maxXYZ.z)));

		Vec2f furthestPoint = Vec2f(max(abs(bounds.minXYZ.x), 
										abs(bounds.maxXYZ.x)),
									max(abs(bounds.minXYZ.z), 
										abs(bounds.maxXYZ.z)));

		bounds.minXYZ.y =  earthRadius * (cos(asin(furthestPoint.magnitude() 
												   / earthRadius)) - 1.0) - 5.0;
		bounds.maxXYZ.y =  earthRadius * (cos(asin(closestPoint.magnitude() 
												   / earthRadius)) - 1.0) + 5.0;

		if(!view->boundingBoxInFrustum(bounds))
		{
			n->recursiveEnvoke([](FractalNode* f){f->waterDivisionLevel=DivisionLevel::FRUSTUM_CULLED;});
		}
		else
		{			
			Vec3f eye = view->camera().eye;
			float hMinDistance = 0.5*n->sideLength + n->minDistance * 0.25;
			if(eye.x > n->worldCenter.x - hMinDistance && 
			   eye.x < n->worldCenter.x + hMinDistance &&
			   eye.y < bounds.maxXYZ.y + n->minDistance * 0.25 && 
			   eye.y > bounds.minXYZ.y - n->minDistance * 0.25 &&
			   eye.z > n->worldCenter.z - hMinDistance && 
			   eye.z < n->worldCenter.z + hMinDistance &&
			   n->sideLength / (tileResolution-1) > 30.0)
			{
				n->waterDivisionLevel = DivisionLevel::SUBDIVIDED;
				if(!n->children[0])
				{
					divisions++;
					n->subdivide();
				}
		
				for(auto& c : n->children)
				{
					nodes.push(c.get());
				}
			}
			else
			{
				n->recursiveEnvoke([](FractalNode* f){f->waterDivisionLevel=DivisionLevel::COMBINED;});
				n->waterDivisionLevel = DivisionLevel::LEVEL_USED;
			}
		}
	}
	while(!nodes.empty())
	{
		nodes.front()->waterDivisionLevel = DivisionLevel::LEVEL_USED;
		nodes.pop();
	}
	return divisions;
}
void Terrain::FractalNode::patchEdges()
{
	if(divisionLevel == DivisionLevel::SUBDIVIDED ||
	   waterDivisionLevel == DivisionLevel::SUBDIVIDED)
	{
		for(auto& c : children)
			c->patchEdges();
	}
	
}
float Terrain::FractalNode::getHeight(unsigned int x, unsigned int z) const
{
	//TODO: add interpolation

	float cx = x*clipMapStep + clipMapOrigin.x;
	float cz = z*clipMapStep + clipMapOrigin.y;
	
	int ix = cx;
	int iz = cz;
	return clipMap->layers[clipMapLayer].heights[ix + iz*clipMap->layerResolution];
}
void Terrain::FractalNode::render(shared_ptr<GraphicsManager::View> view, shared_ptr<GraphicsManager::shader> shader)
{
	if(divisionLevel == DivisionLevel::SUBDIVIDED)
	{
		lastUseFrame = frameNumber;
		for(auto& c : children)
		{
			c->render(view, shader);
		}
	}
	else if(divisionLevel == DivisionLevel::LEVEL_USED)
	{
		lastUseFrame = frameNumber;
		auto& layer = clipMap->layers[clipMapLayer];

		float slopeScale = (layer.maxHeight - layer.minHeight) 
			* ((textureResolution-1) / clipMap->sideLength);

		shader->setUniform3f("origin", origin.x, layer.minHeight, origin.y);
		shader->setUniform3f("scale", Vec3f(sideLength, layer.maxHeight-layer.minHeight, sideLength)); 
		shader->setUniform1f("slopeScale", slopeScale);
		texture->bind(0);

		unsigned int IBO_index = 0;
		if(neighbors[0].expired() || neighbors[0].lock()->divisionLevel == DivisionLevel::COMBINED)
			IBO_index |= 8;
		if(neighbors[1].expired() || neighbors[1].lock()->divisionLevel == DivisionLevel::COMBINED)
			IBO_index |= 2;
		if(neighbors[2].expired() || neighbors[2].lock()->divisionLevel == DivisionLevel::COMBINED)
			IBO_index |= 4;
		if(neighbors[3].expired() || neighbors[3].lock()->divisionLevel == DivisionLevel::COMBINED)
			IBO_index |= 1;

		indexBuffers[IBO_index]->bindBuffer(vertexBuffer);
		indexBuffers[IBO_index]->drawBuffer();
	}
}
void Terrain::FractalNode::renderWater(shared_ptr<GraphicsManager::View> view, shared_ptr<GraphicsManager::shader> shader)
{
	if(waterDivisionLevel == DivisionLevel::SUBDIVIDED)
	{
		lastUseFrame = frameNumber;
		for(auto& c : children)
		{
			c->renderWater(view, shader);
		}
	}
	else if(waterDivisionLevel == DivisionLevel::LEVEL_USED)
	{
		lastUseFrame = frameNumber;
		auto& layer = clipMap->layers[clipMapLayer];
		auto texture = clipMap->layers[clipMapLayer].texture;
		Vec2f tScale = Vec2f(1,1) / (1 << level) * (1 >> clipMapLayer);
		Vec2f tOrigin = Vec2f(coordinates.x, coordinates.y) / (1 << level) * (1 >> clipMapLayer);
		float slopeScale = (layer.maxHeight - layer.minHeight) 
			* ((clipMap->layerResolution<<clipMapLayer) / clipMap->sideLength);

		shader->setUniform3f("origin", origin.x, layer.minHeight, origin.y);
		shader->setUniform3f("scale", Vec3f(sideLength, layer.maxHeight-layer.minHeight, sideLength)); 
		shader->setUniform2f("tOrigin", tOrigin);
		shader->setUniform2f("tScale", tScale);
		shader->setUniform1f("slopeScale", slopeScale);

		texture->bind(3);

		indexBuffers[0]->bindBuffer(vertexBuffer);
		indexBuffers[0]->drawBuffer();
	}
}
void Terrain::FractalNode::initialize()
{
	int numHeights=0;
	unique_ptr<float[]> heightMap(new float[tileResolution*tileResolution*2]);
	for(int x = 0; x < tileResolution; x++)
	{
		for(int y=0; y < tileResolution; y++)
		{
			heightMap[numHeights*2 + 0] = (float)x / (tileResolution-1);
			heightMap[numHeights*2 + 1] = (float)y / (tileResolution-1);
			numHeights++;
		}
	}

	auto v = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
	vertexBuffer = v;
	vertexBuffer->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION2, 0*sizeof(float));
	vertexBuffer->setTotalVertexSize(sizeof(float)*2);
	vertexBuffer->setVertexData(sizeof(float)*tileResolution*tileResolution*2, heightMap.get());

	const unsigned int numIndices = tileResolution*tileResolution*6;
	unique_ptr<unsigned int[]> indices((new unsigned int[numIndices]));
	for(int eFlag=0; eFlag<16; eFlag++)
	{
		int i=0;
		for(int y=0; y < tileResolution-1; y++)
		{
			for(int x = 0; x < tileResolution-1; x++)
			{
				if((eFlag & 4) && x == 0 && (y%2 == 0))
				{
					if(!((eFlag & 8) && y == 0))
					{
						indices[i++] = x		+	y*tileResolution;
						indices[i++] = (x+1)	+	y*tileResolution;
						indices[i++] = (x+1)	+	(y+1)*tileResolution;
					}
					indices[i++] = x			+	y*tileResolution;
					indices[i++] = (x+1)		+	(y+1)*tileResolution;
					indices[i++] = x			+	(y+2)*tileResolution;

					if(!((eFlag & 2) && y == tileResolution-2))
					{
						indices[i++] = x		+	(y+2)*tileResolution;
						indices[i++] = (x+1)	+	(y+2)*tileResolution;
						indices[i++] = (x+1)	+	(y+1)*tileResolution;
					}
				}
				if((eFlag & 8) && y == 0 && (x%2 == 0))
				{
					if(!((eFlag & 4) && x == 0))
					{
						indices[i++] = x		+	y*tileResolution;
						indices[i++] = x		+	(y+1)*tileResolution;
						indices[i++] = (x+1)	+	(y+1)*tileResolution;
					}
					indices[i++] = x			+	y*tileResolution;
					indices[i++] = (x+1)		+	(y+1)*tileResolution;
					indices[i++] = (x+2)		+	y*tileResolution;

					if(!((eFlag & 1) && x == tileResolution-2))
					{
						indices[i++] = (x+2)	+	y*tileResolution;
						indices[i++] = (x+1)	+	(y+1)*tileResolution;
						indices[i++] = (x+2)	+	(y+1)*tileResolution;
					}
				}
				if((eFlag & 1) && x == tileResolution-2 && (y%2 == 0))
				{
					if(!((eFlag & 8) && y == 0)) //XXX
					{
						indices[i++] = x		+	y*tileResolution;
						indices[i++] = x		+	(y+1)*tileResolution;
						indices[i++] = (x+1)	+	y*tileResolution;
					}
					indices[i++] = (x+1)		+	y*tileResolution;
					indices[i++] = x			+	(y+1)*tileResolution;
					indices[i++] = (x+1)		+	(y+2)*tileResolution;

					if(!((eFlag & 2) && y == tileResolution-2))
					{
						indices[i++] = x		+	(y+1)*tileResolution;
						indices[i++] = x		+	(y+2)*tileResolution;
						indices[i++] = (x+1)	+	(y+2)*tileResolution;
					}
				}
				if((eFlag & 2) && y == tileResolution-2 && (x%2 == 0))
				{
					if(!((eFlag & 4) && x == 0))
					{
						indices[i++] = x		+	y*tileResolution;
						indices[i++] = x		+	(y+1)*tileResolution;
						indices[i++] = (x+1)	+	y*tileResolution;
					}

					indices[i++] = x			+	(y+1)*tileResolution;
					indices[i++] = (x+2)		+	(y+1)*tileResolution;
					indices[i++] = (x+1)		+	y*tileResolution;

					if(!((eFlag & 1) && x == tileResolution-2))
					{
						indices[i++] = (x+1)	+	y*tileResolution;
						indices[i++] = (x+2)	+	(y+1)*tileResolution;
						indices[i++] = (x+2)	+	y*tileResolution;
					}
				}

				//default case
				if(!((eFlag & 4) && x == 0) && !((eFlag & 8) && y == 0) && !((eFlag & 1) && x == tileResolution-2) && !((eFlag & 2) && y == tileResolution-2))
				{
					indices[i++] = x			+	y*tileResolution;
					indices[i++] = x			+	(y+1)*tileResolution;
					indices[i++] = (x+1)		+	y*tileResolution;

					indices[i++] = (x+1)		+	y*tileResolution;
					indices[i++] = x			+	(y+1)*tileResolution;
					indices[i++] = x+1			+	(y+1)*tileResolution;
				}
			}
		}

		indexBuffers[eFlag] = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
		indexBuffers[eFlag]->setData(indices.get(),graphics->hasShaderModel5() ? GraphicsManager::PATCHES : GraphicsManager::TRIANGLES, i);
	}	

//	queryTexture = graphics->genTexture2D();
//	queryTexture.setData(1, 1, ****, false, false, nullptr);

	subdivideTexture = graphics->genTexture2D();
	subdivideTexture->setData(textureResolution, textureResolution, GraphicsManager::texture::Format::RGBA16, false, false, nullptr);
}
void Terrain::FractalNode::cleanUp()
{
	vertexBuffer.reset();
	subdivideTexture.reset();
	queryTexture.reset();
	for(auto i : indexBuffers)
		i.reset();
}
//const unsigned char LEFT		= 0x01; //patch to the left is one level above this patch
//const unsigned char RIGHT		= 0x02; //patch to the right is one level above this patch
//const unsigned char TOP			= 0x04; //patch to the top is one level above this patch
//const unsigned char BOTTOM		= 0x08; //patch to the bottom is one level above this patch
//
//const unsigned char SUBDIVIDED	= 0x10;	//patch is divided for rendering
//const unsigned char LEVEL_USED	= 0x20; //patch is rendered at this level
//const unsigned char COMBINED	= 0x40; //patch is not subdivided this far

/*Terrain::Patch::Patch()//: parent(nullptr)
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
	}*/
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

/*void Terrain::Page::subdivide(quadTree<Patch>::node* n, const Vec3f& eye) const
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

	//const float EARTH_RADIUS = 3.3675e6;
	//const float EARTH_RADIUS_SQUARED = EARTH_RADIUS * EARTH_RADIUS;

	//float xCenterDist;
	//float zCenterDist;
	//float elevationAdjust;
	int numHeights=0;
	float* heightMap = new float[((width-1)/LOD+1)*((height-1)/LOD+1)*3];
	for(int x = 0; x < width; x += LOD)
	{
		for(int y=0; y < height; y += LOD)
		{
			//xCenterDist = scale.x * abs(0.5*width - x) / width;
			//zCenterDist = scale.z * abs(0.5*height - y) / height;
			//elevationAdjust = sqrt(EARTH_RADIUS_SQUARED - xCenterDist*xCenterDist - zCenterDist*zCenterDist) - EARTH_RADIUS;

			heightMap[numHeights*3 + 0] = minXYZ.x + (maxXYZ.x - minXYZ.x) * x / (width-LOD);
			heightMap[numHeights*3 + 1] = minXYZ.y + (maxXYZ.y - minXYZ.y) * heights[x+y*width] / (float)USHRT_MAX;
			heightMap[numHeights*3 + 2] = minXYZ.z + (maxXYZ.z - minXYZ.z) * y / (height-LOD);
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
			iBuffer->setData(indices,graphics->hasShaderModel5() ? GraphicsManager::PATCHES : GraphicsManager::TRIANGLES, i);
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
	texture->setData(width, height, GraphicsManager::texture::RGBA16, false, false, (unsigned char*)groundValues);
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
	return;
	if(foliageDensity <= 0.0)
	{
		unsigned char texData[] = {1,1,1,1};
		treeTexture = graphics->genTexture2D();
		treeTexture->setData(1,1, GraphicsManager::texture::RGBA, false, false, texData); 
		return;
	}
	foliageDensity = min(foliageDensity, 150); //keep foliageDensity reasonable


	int				foliagePatchesX = width/8;//64;//16*(width/16)/LOD;
	int				foliagePatchesY = height/8;//64;//16*(height/16)/LOD;
	unsigned int	numPatches = foliagePatchesX * foliagePatchesY;	
	float			patchArea = (maxXYZ.x-minXYZ.x) * (maxXYZ.z-minXYZ.z) / numPatches;
	float			treesPerPatch = patchArea * foliageDensity / (1000*1000) * 15.0;
	int				sLength = ceil(1.5 * sqrt(treesPerPatch));
	float			inv_sLength = 1.0f/sLength;
	float			placementOdds = treesPerPatch / (sLength*sLength);

	const int MAX_QUADS_WITHOUT_TF = 100000 * 10000; // 100,000 quads = 50,000 trees = 400,000 vertices
	bool hasTransformFeedback = graphics->hasShaderModel4();
	foliagePatch patch;

	double t=GetTime();
	if(hasTransformFeedback)
	{
		texture->bind();

		auto emptyVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
		emptyVBO->setTotalVertexSize(0);
		emptyVBO->setVertexData(0, nullptr);
		auto counterVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
		counterVBO->setTotalVertexSize(1);
		counterVBO->setVertexData(4 * foliagePatchesX * foliagePatchesY * sLength * sLength, nullptr);

		//count the number of trees that there will be
		auto countTreesShader = shaders.bind("count trees");
		countTreesShader->setUniform1i("groundTex", 0);
		countTreesShader->setUniform1i("width", foliagePatchesX * sLength);
		countTreesShader->setUniform3f("worldOrigin", minXYZ);
		countTreesShader->setUniform3f("worldSpacing", Vec3f((maxXYZ.x-minXYZ.x) / (foliagePatchesX*sLength), (maxXYZ.y-minXYZ.y), (maxXYZ.z-minXYZ.z) / (foliagePatchesY*sLength)));
		countTreesShader->setUniform2f("texOrigin", 0.5/texture->getWidth(), 0.5/texture->getHeight());
		countTreesShader->setUniform2f("texSpacing", float((texture->getWidth()-1.0) / texture->getWidth())/(foliagePatchesX*sLength), float((texture->getHeight()-1.0) / texture->getHeight())/(foliagePatchesY*sLength));
		countTreesShader->setUniform1f("placementOdds", placementOdds);
		countTreesShader->setUniform1i("patch_width", sLength);
		countTreesShader->setUniform1i("patch_height", sLength);
		//countTreesShader->setUniform1i("vertexID_offset", 0);
		unsigned int numTreesInPatch;
		numTrees = 0;
		emptyVBO->bindBuffer();
		patch.plantIndexBuffer = nullptr;	
		for(int y = 0; y < foliagePatchesY; y++)
		{
			for(int x = 0; x < foliagePatchesX; x++)
			{
				//countTreesShader->setUniform1i("x_offset", x * sLength);
				//countTreesShader->setUniform1i("y_offset", y * sLength);
				countTreesShader->setUniform1i("vertexID_offset", (x+y*foliagePatchesX) * sLength*sLength);

				counterVBO->bindTransformFeedback(GraphicsManager::POINTS);
				emptyVBO->drawBuffer(GraphicsManager::POINTS, 0,  sLength * sLength);
				numTreesInPatch = counterVBO->unbindTransformFeedback();graphics->checkErrors();
				if(numTreesInPatch > 0)
				{
					patch.center.x = minXYZ.x + (maxXYZ.x-minXYZ.x) * (0.5+x) / foliagePatchesX;
					patch.center.z = minXYZ.z + (maxXYZ.z-minXYZ.z) * (0.5+y) / foliagePatchesY;
					patch.center.y = getHeight(Vec2f(patch.center.x,patch.center.z)) + 12.0;
					Vec3f boundsMin = Vec3f(minXYZ.x + (maxXYZ.x-minXYZ.x) * x / foliagePatchesX, minXYZ.y, minXYZ.z + (maxXYZ.z-minXYZ.z) * y / foliagePatchesY);
					Vec3f boundsMax = Vec3f(minXYZ.x + (maxXYZ.x-minXYZ.x) * (1.0+x) / foliagePatchesX, maxXYZ.y+24.0, minXYZ.z + (maxXYZ.z-minXYZ.z) * (1.0+y) / foliagePatchesY);
					patch.bounds = BoundingBox<float>(boundsMin, boundsMax);
					patch.sm4_treeVboOffset = 12 * numTrees;
					patch.sm4_treeVboSize = 12 * numTreesInPatch;
					foliagePatches.push_back(patch);
					numTrees += numTreesInPatch;
				}
			}
		}
		counterVBO.reset();
	//	counterVBO->bindTransformFeedback(GraphicsManager::POINTS);
	//	emptyVBO->bindBuffer();
	//	emptyVBO->drawBuffer(GraphicsManager::POINTS, 0, foliagePatchesX * foliagePatchesY * sLength * sLength);
	//	numTrees = counterVBO->unbindTransformFeedback();
	//	counterVBO.reset();

		if(numTrees > 0)
		{
			//prepare foliage VBO
			foliageVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
			foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
			foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		3*sizeof(float));
			foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::COLOR3,		5*sizeof(float));
			foliageVBO->setTotalVertexSize(32);
			foliageVBO->setVertexData(32 * 12 * numTrees, nullptr);
			//render trees into VBO
			auto placeTreesShader = shaders.bind("place trees");
			placeTreesShader->setUniform1i("groundTex", 0);
			placeTreesShader->setUniform1i("width", foliagePatchesX*sLength);
			placeTreesShader->setUniform3f("worldOrigin", minXYZ);
			placeTreesShader->setUniform3f("worldSpacing", Vec3f((maxXYZ.x-minXYZ.x) / (foliagePatchesX*sLength), (maxXYZ.y-minXYZ.y), (maxXYZ.z-minXYZ.z) / (foliagePatchesY*sLength)));
			placeTreesShader->setUniform2f("texOrigin", 0.5/texture->getWidth(), 0.5/texture->getHeight());
			placeTreesShader->setUniform2f("texSpacing", float((texture->getWidth()-1.0) / texture->getWidth())/(foliagePatchesX*sLength), float((texture->getHeight()-1.0) / texture->getHeight())/(foliagePatchesY*sLength));
			placeTreesShader->setUniform1f("placementOdds", placementOdds);
			placeTreesShader->setUniform1i("patch_width", sLength);//foliagePatchesX *sLength
			placeTreesShader->setUniform1i("patch_height", sLength);
			placeTreesShader->setUniform1i("vertexID_offset", 0);


			foliageVBO->bindTransformFeedback(GraphicsManager::TRIANGLES);
			emptyVBO->bindBuffer();
			emptyVBO->drawBuffer(GraphicsManager::POINTS, 0, foliagePatchesX * foliagePatchesY * sLength * sLength / 10); //<-causes error on Linux (intel graphics)
#ifdef _DEBUG
			unsigned int n = foliageVBO->unbindTransformFeedback();
			debugAssert(n == 4*numTrees);
#else
			foliageVBO->unbindTransformFeedback();
#endif

			treeTexture = graphics->genTexture2D();
			treeTexture->setData(4096, 4096, GraphicsManager::texture::RGBA, false, false, nullptr);

			//auto depthTex = graphics->genTexture2D();
			//depthTex->setData(4096,4096,GraphicsManager::texture::DEPTH, false, false, nullptr);

			graphics->setDepthTest(false);
			shared_ptr<GraphicsManager::View> view(new GraphicsManager::View());
			view->ortho(-2048,2048, -2048,2048, -1, 1);
			view->lookAt(Vec3f(0.5, 2.0, 0.5), Vec3f(0.5, 0.0, 0.5), Vec3f(0, 0, 1));
			auto createTreeTexture = shaders.bind("create tree texture");
			createTreeTexture->setUniform1i("groundTex", 0); texture->bind();
			createTreeTexture->setUniform1i("width", foliagePatchesX*sLength);
			createTreeTexture->setUniform3f("worldOrigin", minXYZ);
			createTreeTexture->setUniform3f("worldSpacing", Vec3f((maxXYZ.x-minXYZ.x) / (foliagePatchesX*sLength), (maxXYZ.y-minXYZ.y), (maxXYZ.z-minXYZ.z) / (foliagePatchesY*sLength)));
			createTreeTexture->setUniform2f("texOrigin", 0.5/texture->getWidth(), 0.5/texture->getHeight());
			createTreeTexture->setUniform2f("texSpacing", float((texture->getWidth()-1.0) / texture->getWidth())/(foliagePatchesX*sLength), float((texture->getHeight()-1.0) / texture->getHeight())/(foliagePatchesY*sLength));
			createTreeTexture->setUniform1f("placementOdds", placementOdds);
			createTreeTexture->setUniform1i("patch_width", sLength);//foliagePatchesX *sLength
			createTreeTexture->setUniform1i("patch_height", sLength);
			createTreeTexture->setUniform1i("vertexID_offset", 0);
			createTreeTexture->setUniform2f("transform", 4096 / (maxXYZ.x-minXYZ.x), 4096 / (maxXYZ.z-minXYZ.z));
			graphics->startRenderToTexture(treeTexture, nullptr);
			graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);
			emptyVBO->bindBuffer();
			emptyVBO->drawBuffer(GraphicsManager::POINTS, 0, foliagePatchesX * foliagePatchesY * sLength * sLength);
			graphics->endRenderToTexture();
			treeTexture->generateMipmaps();

			//unsigned char* texData = treeTexture->getData();
			//shared_ptr<FileManager::textureFile> textureFile(new FileManager::textureFile("../treesTexture.png",FileManager::PNG));
			//textureFile->channels = 4;
			//textureFile->contents = texData;
			//textureFile->width = 4096;
			//textureFile->height = 4096;
			//fileManager.writeFile(textureFile); //will delete[] texData
		}
		else
		{
			unsigned char texData[] = {1,1,1,1};
			treeTexture = graphics->genTexture2D();
			treeTexture->setData(1,1, GraphicsManager::texture::RGBA, false, false, texData); 
		}
	}
	else
	{
		unsigned int nQuads=0;
		unsigned int nQuadsStart=0;
		Vec2f dir;
		Vec3f right, fwd;
		Vec3f position;
		
		vector<texturedColoredVertex3D> vertices;
		//foliagePatch::plant p;
		float normal_y;

		texturedColoredVertex3D tmpVerts[16];

		tmpVerts[0].UV = Vec2f(0.0,1.0);	//Vec2f(0.0,1.0);
		tmpVerts[1].UV = Vec2f(0.5,1.0);	//Vec2f(1.0,1.0);
		tmpVerts[2].UV = Vec2f(0.5,0.0);	//Vec2f(1.0,0.0);
		tmpVerts[3].UV = Vec2f(0.0,0.0);	//Vec2f(0.0,0.0);

		tmpVerts[4].UV = Vec2f(0.5,1.0);	//Vec2f(0.0,1.0);
		tmpVerts[5].UV = Vec2f(1.0,1.0);	//Vec2f(1.0,1.0);
		tmpVerts[6].UV = Vec2f(1.0,0.0);	//Vec2f(1.0,0.0);
		tmpVerts[7].UV = Vec2f(0.5,0.0);	//Vec2f(0.0,0.0);

		tmpVerts[8].UV =  Vec2f(0.5,1.0);	//Vec2f(0.0,1.0);
		tmpVerts[9].UV =  Vec2f(0.0,1.0);	//Vec2f(1.0,1.0);
		tmpVerts[10].UV = Vec2f(0.0,0.0);	//Vec2f(1.0,0.0);
		tmpVerts[11].UV = Vec2f(0.5,0.0);	//Vec2f(0.0,0.0);

		tmpVerts[12].UV = Vec2f(1.0,1.0);	//Vec2f(0.0,1.0);
		tmpVerts[13].UV = Vec2f(0.5,1.0);	//Vec2f(1.0,1.0);
		tmpVerts[14].UV = Vec2f(0.5,0.0);	//Vec2f(1.0,0.0);
		tmpVerts[15].UV = Vec2f(1.0,0.0);	//Vec2f(0.0,0.0);

		for(int x=0; x < foliagePatchesX && (hasTransformFeedback || nQuads < MAX_QUADS_WITHOUT_TF); x++)
		{
			for(int y=0; y < foliagePatchesY && (hasTransformFeedback || nQuads < MAX_QUADS_WITHOUT_TF); y++)
			{
				unsigned int n = 0;
				nQuadsStart = nQuads;

				for(int h=0; h<sLength && nQuads < MAX_QUADS_WITHOUT_TF; h++)
				{
					for(int k=0; k<sLength && nQuads < MAX_QUADS_WITHOUT_TF; k++)
					{
						if(random<float>() < placementOdds)
						{
							//position.x = minXYZ.x + (maxXYZ.x-minXYZ.x) * (/*random<float>* /(0.5*std::sin(10.0*x*sLength+h+11.0*y*sLength+k)+0.5)*(inv_sLength)+h*inv_sLength + x) / foliagePatchesX;
							//position.z = minXYZ.z + (maxXYZ.z-minXYZ.z) * (/*random<float>* /(0.5*std::cos(10.0*x*sLength+h+11.0*y*sLength+k)+0.5)*(inv_sLength)+k*inv_sLength + y) / foliagePatchesY;
							position.x = minXYZ.x + (maxXYZ.x-minXYZ.x) * (random<float>(inv_sLength)+h*inv_sLength + x) / foliagePatchesX;
							position.z = minXYZ.z + (maxXYZ.z-minXYZ.z) * (random<float>(inv_sLength)+k*inv_sLength + y) / foliagePatchesY;

							position.y = getHeight(Vec2f(position.x, position.z));
							normal_y = getNormal(Vec2f(position.x, position.z)).y;
							if(position.y > 40.0 && normal_y > 0.9211 && (position.y < 150.0 || normal_y < 0.98))
							{
								n = random<float>(1.0) < 0.5 ? 0 : 8;

								//set all vertices to have the same color
								tmpVerts[n+0].color = tmpVerts[n+1].color = tmpVerts[n+2].color = tmpVerts[n+3].color = 
								tmpVerts[n+4].color = tmpVerts[n+5].color = tmpVerts[n+6].color = tmpVerts[n+7].color = 
									Color3(random<float>(0.8,1.1), random<float>(0.8,1.1), random<float>(0.8,1.1));

								dir = random2<float>();
								float s = (4.0 + random<float>(4.0) + random<float>(4.0))*0.4 * 1.5;
								tmpVerts[n+0].position = position + Vec3f(-2.5*dir.x, 0.0, -2.5*dir.y)*s;	//width: 2.0 -> 2.5
								tmpVerts[n+1].position = position + Vec3f( 2.5*dir.x, 0.0,  2.5*dir.y)*s;
								tmpVerts[n+2].position = position + Vec3f( 2.5*dir.x, 5.0,  2.5*dir.y)*s;
								tmpVerts[n+3].position = position + Vec3f(-2.5*dir.x, 5.0, -2.5*dir.y)*s;

								swap(dir.x, dir.y);
								dir.x = - dir.x;
								tmpVerts[n+4].position = position + Vec3f(-2.5*dir.x, 0.0, -2.5*dir.y)*s;
								tmpVerts[n+5].position = position + Vec3f( 2.5*dir.x, 0.0,  2.5*dir.y)*s;
								tmpVerts[n+6].position = position + Vec3f( 2.5*dir.x, 5.0,  2.5*dir.y)*s;
								tmpVerts[n+7].position = position + Vec3f(-2.5*dir.x, 5.0, -2.5*dir.y)*s;

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

								//foliagePatch::plant p;
								//p.location = position;
								//p.height = 5.0*s;
								//patch.plants.push_back(p);


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
					Vec3f boundsMin = Vec3f(minXYZ.x + (maxXYZ.x-minXYZ.x) * x / foliagePatchesX, minXYZ.y, minXYZ.z + (maxXYZ.z-minXYZ.z) * y / foliagePatchesY);
					Vec3f boundsMax = Vec3f(minXYZ.x + (maxXYZ.x-minXYZ.x) * (1.0+x) / foliagePatchesX, maxXYZ.y+24.0, minXYZ.z + (maxXYZ.z-minXYZ.z) * (1.0+y) / foliagePatchesY);
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
					//patch.plants.clear();
				}
			}
		}

		foliageVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
		foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
		foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		3*sizeof(float));
		foliageVBO->addVertexAttribute(GraphicsManager::vertexBuffer::COLOR3,		5*sizeof(float));
		foliageVBO->setTotalVertexSize(sizeof(texturedColoredVertex3D));
		foliageVBO->setVertexData(sizeof(texturedColoredVertex3D)*vertices.size(), !vertices.empty() ? &vertices[0] : nullptr);

		if(nQuads >= MAX_QUADS_WITHOUT_TF)
		{
			messageBox("Warning: level contains more trees than supported by Shader Model 3 graphics cards. Some trees may be missing!");
		}

		unsigned char texData[] = {1,1,1,1};
		treeTexture = graphics->genTexture2D();
		treeTexture->setData(1,1, GraphicsManager::texture::RGBA, false, false, texData); 
	}
	t = GetTime() - t;
	t = GetTime();
}
Vec3f Terrain::Page::rasterNormal(Vec2u loc) const
{
	loc.x = clamp(loc.x, 0, width-1);
	loc.y = clamp(loc.y, 0, height-1);

	float By = (loc.y < height-1)	? heights[(loc.x)	+ (loc.y+1)	* width] - heights[loc.x + loc.y*width]  : 0.0f;
	float Ay = (loc.x < width-1)	? heights[(loc.x+1) + (loc.y)	* width] - heights[loc.x + loc.y*width]  : 0.0f;
	float Dy = (  loc.y > 0	)		? heights[(loc.x)	+ (loc.y-1)	* width] - heights[loc.x + loc.y*width]  : 0.0f;
	float Cy = (  loc.x > 0	)		? heights[(loc.x-1)	+ (loc.y)	* width] - heights[loc.x + loc.y*width]  : 0.0f;

	return Vec3f((Cy - Ay) * width / (maxXYZ.x-minXYZ.x), 2.0 * 65536.0 / (maxXYZ.y-minXYZ.y), (Dy - By) * height / (maxXYZ.z-minXYZ.z)).normalize();
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
	loc.x *= static_cast<float>(width-1) / (maxXYZ.x - minXYZ.x);
	loc.y *= static_cast<float>(height-1) / (maxXYZ.z - minXYZ.z);

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
	loc.x *= static_cast<float>(width-1) / (maxXYZ.x - minXYZ.x);
	loc.y *= static_cast<float>(height-1) / (maxXYZ.z - minXYZ.z);

	float A, B, C, D;
	//  A  _____  B
	//    |    /|
	//	  |  /  |
	//  D |/____| C

	float x_fract = loc.x-floor(loc.x);
	float y_fract = loc.y-floor(loc.y);

	if(x_fract + y_fract < 1.0)
	{
		A = rasterHeight(Vec2u(floor(loc.x),floor(loc.y)));
		B = rasterHeight(Vec2u(ceil(loc.x),floor(loc.y)));
		D = rasterHeight(Vec2u(floor(loc.x),ceil(loc.y)));
		C = B + D - A;
	}
	else
	{
		B = rasterHeight(Vec2u(ceil(loc.x),floor(loc.y)));
		C = rasterHeight(Vec2u(ceil(loc.x),ceil(loc.y)));
		D = rasterHeight(Vec2u(floor(loc.x),ceil(loc.y)));
		A = B + D - C;
	}
	return lerp(lerp(A,B,x_fract), lerp(D,C,x_fract), y_fract);
}
void Terrain::Page::render(shared_ptr<GraphicsManager::View> view, TerrainType shaderType) const
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




	shared_ptr<GraphicsManager::shader> terrainShader;
	if(shaderType == TERRAIN_ISLAND)
	{
		terrainShader = shaders.bind("island terrain");
	}
	else if(shaderType == TERRAIN_MOUNTAINS)
	{
		terrainShader = shaders.bind("mountain terrain");
	}
	else if(shaderType == TERRAIN_SNOW)
	{
		terrainShader = shaders.bind("snow terrain");
	}
	else if(shaderType == TERRAIN_DESERT)
	{
		terrainShader = shaders.bind("desert terrain");
	}

	//global
	terrainShader->setUniform1i("sky",			0); //bound by caller
	terrainShader->setUniform1i("groundTex",	1);	texture->bind(1);
	terrainShader->setUniform1i("LCnoise",		2);	dataManager.bind("LCnoise",2);
	terrainShader->setUniform1i("noiseTex",		3);	dataManager.bind("noise",3);
	terrainShader->setUniform1i("sand",			4);	dataManager.bind("sand", 4);
	terrainShader->setUniform1i("sand2",		5);	dataManager.bind("desertSand",5);
	terrainShader->setUniform1i("snow",			6);	dataManager.bind("snow",6);
	terrainShader->setUniform1i("snow_normals",	7);	dataManager.bind("snow normals",7);
	terrainShader->setUniform1i("grass",		8);	dataManager.bind("grass",8);
	terrainShader->setUniform1i("rock",			9);	dataManager.bind("rock",9);
	terrainShader->setUniform1i("grass_normals",10);dataManager.bind("grass normals",10); //can take 100+ ms to complete under linux?
	terrainShader->setUniform1i("fractalNormals",11);dataManager.bind("fractal normals",11); //can take 100+ ms to complete under linux?
	terrainShader->setUniform1i("treesTexture",12);treeTexture->bind(12);
	terrainShader->setUniform1i("grassDetail",	13);	dataManager.bind("grass detail",13);

	terrainShader->setUniformMatrix("cameraProjection",	view->projectionMatrix() * view->modelViewMatrix());
	terrainShader->setUniformMatrix("modelTransform",	Mat4f());
	terrainShader->setUniform1f("time",					world->time());
	terrainShader->setUniform3f("lightPosition",		graphics->getLightPosition());
	terrainShader->setUniform3f("eyePos",				view->camera().eye);
	terrainShader->setUniform2f("gtex_halfPixel",		0.5 / (width), 0.5 / (height));
	terrainShader->setUniform2f("gtex_origin",			minXYZ.x, minXYZ.z);
	terrainShader->setUniform2f("gtex_invScale",		1.0/(maxXYZ.x-minXYZ.x) * (width-1)/width, 1.0/(maxXYZ.z-minXYZ.z) * (height-1)/height);
	terrainShader->setUniform2f("ttex_halfPixel",		0.5 / (4096), 0.5 / (4096));
	terrainShader->setUniform2f("ttex_origin",			minXYZ.x, minXYZ.z);
	terrainShader->setUniform2f("ttex_invScale",		1.0/(maxXYZ.x-minXYZ.x) * (4096-1)/4096, 1.0/(maxXYZ.z-minXYZ.z) * (4096-1)/4096);
	terrainShader->setUniform1f("minHeight",			minXYZ.y);
	terrainShader->setUniform1f("heightRange",			maxXYZ.y-minXYZ.y);
	sceneManager.bindLights(terrainShader);

	Vec3f eye;
	unsigned int bufferOffset;
	for(auto i = renderQueue.begin(); i != renderQueue.end(); i++)
	{
		//float d = (eye.x-(*i)->center.x)*(eye.x-(*i)->center.x) + (eye.z-(*i)->center.z)*(eye.z-(*i)->center.z);
		//dataManager.setUniform1f("dist", (*i)->minDistanceSquared / d);
		bufferOffset = sizeof(float)*3 * ((*i)->y + ((*i)->x*((width-1)/LOD+1))) * (16<<(levelsDeep-(*i)->level));
		indexBuffers[(*i)->level][(*i)->element.edgeFlags]->bindBuffer(vertexBuffer, bufferOffset);
		indexBuffers[(*i)->level][(*i)->element.edgeFlags]->drawBuffer();
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
	//bool trees3D = false;

	//static vector<foliagePatch::plant> treeModels;
	//treeModels.clear();

	//Profiler.startElement("checkDepth");
	//for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
	//{
	//	if(!i->plantIndexBuffer || !view->boundingBoxInFrustum(i->bounds) || i->center.distanceSquared(view->camera().eye) > 10000.0*10000.0)
	//	{
	//		i->renderType = foliagePatch::DONT_RENDER;
	//	}
	//	else// if(!trees3D || i->center.distanceSquared(view->camera().eye) > 250.0*250.0)
	//	{
	//		i->renderType = foliagePatch::RENDER_BILLBAORD;
	//	}
	//	//else
	//	//{
	//	//	for(auto p = i->plants.begin(); p != i->plants.end(); p++)
	//	//	{
	//	//		(*p).screenDepth = view->project3(p->location).z;
	//	//	}
	//	//	treeModels.insert(treeModels.begin(), i->plants.begin(), i->plants.end());
	//	//	i->renderType = foliagePatch::RENDER_MODEL;
	//	//}
	//}
	//Profiler.endElement("checkDepth");
	//std::sort(treeModels.begin(), treeModels.end(), [](const foliagePatch::plant& p1, const foliagePatch::plant& p2)->bool{return p1.screenDepth > p2.screenDepth;});

	//graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);
	
	return;

	if(foliageVBO)
	{
		bool hasTransformFeedback = graphics->hasShaderModel4();

		unsigned int treesRendered = 0;

		graphics->setDepthMask(true);
		//world->treeTexture->bind();
		dataManager.bind("tree");
		auto alphaTreesShader = shaders.bind("trees alpha test shader");
		alphaTreesShader->setUniform1i("tex", 0);
		alphaTreesShader->setUniform1i("sky", 1);
		alphaTreesShader->setUniform3f("eyePos", view->camera().eye);
		alphaTreesShader->setUniform3f("right", view->camera().right);
		alphaTreesShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
		foliageVBO->bindBuffer();
		if(hasTransformFeedback)
		{
			//foliageVBO->drawBuffer(GraphicsManager::TRIANGLES, 0, numTrees * 12);
			for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
			{
				if(view->boundingBoxInFrustum(i->bounds) && i->center.distanceSquared(view->camera().eye) < 10000.0*10000.0)
				{
					foliageVBO->drawBuffer(GraphicsManager::TRIANGLES, i->sm4_treeVboOffset, i->sm4_treeVboSize);
					treesRendered += i->sm4_treeVboSize / 12;
				}
			}
		}
		else
		{
			for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
			{
				//if(i->renderType == foliagePatch::RENDER_BILLBAORD /*|| i->renderType == foliagePatch::RENDER_MODEL* /)
				if(view->boundingBoxInFrustum(i->bounds) && i->center.distanceSquared(view->camera().eye) < 10000.0*10000.0)
				{
					i->plantIndexBuffer->bindBuffer();
					i->plantIndexBuffer->drawBuffer();
				}
			}
		}
		graphics->setDepthMask(false);
		if(graphics->getMultisampling() > 1)
		{
			auto treesShader = shaders.bind("trees shader");
			treesShader->setUniform1i("tex", 0);
			treesShader->setUniform1i("sky", 1);
			treesShader->setUniform3f("eyePos", view->camera().eye);
			treesShader->setUniform3f("right", view->camera().right);
			treesShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
			if(hasTransformFeedback)
			{
				//foliageVBO->drawBuffer(GraphicsManager::TRIANGLES, 0, numTrees * 12);
				for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
				{
					if(view->boundingBoxInFrustum(i->bounds) && i->center.distanceSquared(view->camera().eye) < 10000.0*10000.0)
					{
						foliageVBO->drawBuffer(GraphicsManager::TRIANGLES, i->sm4_treeVboOffset, i->sm4_treeVboSize);
					}
				}
			}
			else
			{
				for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
				{
					//if(i->renderType == foliagePatch::RENDER_BILLBAORD /*|| i->renderType == foliagePatch::RENDER_MODEL* /)
					if(view->boundingBoxInFrustum(i->bounds) && i->center.distanceSquared(view->camera().eye) < 10000.0*10000.0)
					{
						i->plantIndexBuffer->bindBuffer();
						i->plantIndexBuffer->drawBuffer();
					}
				}
			}
		}
	}


	//auto tree3DShader = shaders("trees3D shader");
	//auto leavesMesh = dataManager.getModel("tree leaves");
	//auto trunkMesh = dataManager.getModel("tree trunk");
	//if(trees3D && tree3DShader && trunkMesh && !trunkMesh->materials.empty() && leavesMesh && !leavesMesh->materials.empty())
	//{
	//	tree3DShader->bind();
	//	tree3DShader->setUniform1i("tex",0);
	//	trunkMesh->materials.front().tex->bind(0);
	//	trunkMesh->materials.front().indexBuffer->bindBuffer(trunkMesh->VBO);
	//	tree3DShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	//
	//	//for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
	//	//{
	//	//	if(i->renderType == foliagePatch::RENDER_MODEL)
	//	//	{
	//	//		for(auto p = i->plants.begin(); p != i->plants.end(); p++)
	//	//		{
	//	for(auto p = treeModels.begin(); p != treeModels.end(); p++)
	//	{
	//		if(view->camera().eye.distanceSquared(p->location) < 200.0*200.0)
	//		{
	//				tree3DShader->setUniform1f("transparency", clamp((200.0 - view->camera().eye.distance(p->location))*0.02, 0.0, 1.0));
	//				tree3DShader->setUniform4f("position_scale",p->location.x, p->location.y, p->location.z, p->height / 23.73);
	//				trunkMesh->materials.front().indexBuffer->drawBuffer();
	//		}
	//	}
	//	//		}
	//	//	}
	//	//}
	//
	//	leavesMesh->materials.front().tex->bind(0);
	//	leavesMesh->materials.front().indexBuffer->bindBuffer(leavesMesh->VBO);
	//	//for(auto i = foliagePatches.begin(); i != foliagePatches.end(); i++)
	//	//{
	//	//	if(i->renderType == foliagePatch::RENDER_MODEL)
	//	//	{
	//	//		for(auto p = i->plants.begin(); p != i->plants.end(); p++)
	//	//		{
	//	for(auto p = treeModels.begin(); p != treeModels.end(); p++)
	//	{
	//		if(view->camera().eye.distanceSquared(p->location) < 200.0*200.0)
	//		{
	//				tree3DShader->setUniform1f("transparency", 1.0/*clamp((200.0 - view->camera().eye.distance(p->location))*0.2, 0.0, 1.0)* /);
	//				tree3DShader->setUniform4f("position_scale",p->location.x, p->location.y, p->location.z, p->height / 23.73);
	//				leavesMesh->materials.front().indexBuffer->drawBuffer();
	//		}
	//	}
	//	//		}
	//	//	}
	//	//}
	//}
	//graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
}
Terrain::Page::~Page()
{
	delete[] heights;
}*/
// Terrain::decal::decal(string tex, shared_ptr<GraphicsManager::vertexBuffer> vbo, shared_ptr<GraphicsManager::indexBuffer> ibo, double sTime, double fLength): texture(tex), vertexBuffer(vbo), indexBuffer(ibo), startTime(sTime), fadeLength(fLength)
// {

// }
Terrain::Terrain(shared_ptr<ClipMap> clipMap): wireframe(false)
{
	debugAssert(isPowerOfTwo(clipMap->layerResolution - 1));

	graphics->setClearColor(Color4(0, 0.2, 0.3, 1.0));

	FractalNode::initialize();

/*	shaderType = shader;
	waterPlane = (shader==TERRAIN_ISLAND);
	shared_ptr<Page> p(new Page(Heights,patchResolution,position,scale,LOD));
	p->generateFoliage(foliageDensity);
	terrainPages.push_back(p);
	terrainPosition = position;
	terrainScale = scale; */
//	mBounds = bounds;

	Vec3f sun = (graphics->getLightPosition()).normalize();

	//generateSky(acos(sun.y), atan2(sun.z,sun.x), 1.8); //should actually make the sun be at the position of the light source...
	generateSky(sun);
	generateOceanTexture(); //we now load this texture from disk to reduce startup time

	//generateTreeTexture(dataManager.getModel("tree model"));

	graphics->checkErrors();

//	skyTexture = static_pointer_cast<GraphicsManager::textureCube>(dataManager.getTexture("skybox"));


	fractalTerrain = unique_ptr<FractalNode>(new FractalNode(nullptr, 0, Vec2i(0,0), clipMap));

	//////////////////
/*	Vec3f n;
	unique_ptr<unsigned short[]> groundValues(new unsigned short[patchResolution*patchResolution*4]);
	for(unsigned int x=0; x < patchResolution; x++)
	{
		for(unsigned int z=0; z < patchResolution; z++)
		{
			(n = rasterNormal(Vec2u(x,z));
			//n = interpolatedNormal(Vec2f(x,z));
			if(n.magnitudeSquared() < 0.001)
				n = Vec3f(0.0,1.0,0.0);
	
			groundValues[(x + z * patchResolution)*4 + 0] = (unsigned short)(32767.5+n.x*32767.5);
			groundValues[(x + z * patchResolution)*4 + 1] = (unsigned short)(n.y*65535.0);
			groundValues[(x + z * patchResolution)*4 + 2] = (unsigned short)(32767.5+n.z*32767.5);
			groundValues[(x + z * patchResoultion)*4 + 3] = heights[x+z*width];
		}
	}
	texture->setData(patchResolution, patchResolution, GraphicsManager::texture::RGBA16, false, false, (unsigned char*)groundValues);
*/

	
	// float s = 16.0 / waveTextureScale;
	for(unsigned int i = 0; i < waves.size(); i++)
	{
		int fx = random<int>(1, waveTextureResolution / 16 + 1);
		int fz = random<int>(1, waveTextureResolution / 16 + 1);
		float f = sqrt(fx*fx + fz*fz);//0.5 * pow(2.0, random<float>(-1.0, 1.0));
		float A = 1.0 / (20.0*f);//random<float>(1.0 / (40 * f), 1.0 / (20 * f));
		waves.frequencies[i] = f;
		waves.amplitudes[i] = A;
		waves.speeds[i] = sqrt(9.8 * waveTextureScale / (3.14259 * 2 * f)) 
			/ waveTextureScale;
		waves.directions[i] = Vec2f(fx*(2*random<int>(0,2)-1), 
									fz*(2*random<int>(0,2)-1)).normalize();

		cout << waves.directions[i].x << " " << waves.directions[i].y << endl;
	}

	waveTexture = graphics->genTexture2D();
	waveTexture->setData(waveTextureResolution,waveTextureResolution,
						 GraphicsManager::texture::RGBA, true, false, nullptr);

	const unsigned int numRings = 96;
	const unsigned int vertsPerRing = 64;
	unique_ptr<float[]> vertices{new float[2+2*numRings*vertsPerRing]};
	vertices[0] = vertices[1] = 0.0f;
	float angScale = 2 * PI / vertsPerRing;
	float radiusScale = 1.0 / pow(numRings,4);
	for(unsigned int ring = 0; ring < numRings; ring++)
	{
		for(unsigned int vert = 0; vert< vertsPerRing; vert++)
		{
			float r = radiusScale * pow(ring+1, 4);
			vertices[2+2*(ring*vertsPerRing+vert)] = r * cos(vert*angScale);
			vertices[2+2*(ring*vertsPerRing+vert)+1] = r * sin(vert*angScale);
		}
	}
	waterVBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
	waterVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION2, 0*sizeof(float));
	waterVBO->setTotalVertexSize(sizeof(float)*2);
	waterVBO->setVertexData(sizeof(float)*(2+2*numRings*vertsPerRing), vertices.get());

	unsigned int i = 0;
	unique_ptr<uint16_t[]> indices{new uint16_t[3*(2*numRings-1)*vertsPerRing]};
	for(int v = 0; v < vertsPerRing; v++)
	{
		indices[i++] = 0;
		indices[i++] = v+1;
		indices[i++] = (v+2) % vertsPerRing;
	}
	for(unsigned int ring = 1; ring < numRings; ring++)
	{
		for(unsigned int vert = 0; vert < vertsPerRing; vert++)
		{
			indices[i++] = 1+(ring)*vertsPerRing + vert;
			indices[i++] = 1+(ring+1)*vertsPerRing + vert;
			indices[i++] = 1+(ring)*vertsPerRing + (vert+1) % vertsPerRing;

			indices[i++] = 1+(ring+1)*vertsPerRing + vert;
			indices[i++] = 1+(ring+1)*vertsPerRing + (vert+1) % vertsPerRing;
			indices[i++] = 1+(ring)*vertsPerRing + (vert+1) % vertsPerRing;
		}
	}
	waterIBO = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
	waterIBO->setData(indices.get(), GraphicsManager::TRIANGLES, 3*(2*numRings-1)*vertsPerRing);

}
void Terrain::generateSky(Vec3f sunDirection)
{
	//const double AOI = 45.0 * PI/180.0;
	
	double t = GetTime();

	skyTexture = graphics->genTextureCube();
	skyTexture->setData(256,256,GraphicsManager::texture::BGR, nullptr);

	t = GetTime() - t;
	t = GetTime();


	graphics->setDepthTest(false);
	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);


	auto skyShader = shaders.bind("sky2 shader");

	dataManager.bind("noise",0);
	skyShader->setUniform1f("time", 0); //note: this is a snapshot at t=0
	skyShader->setUniform1i("noise", 0);
	skyShader->setUniform3f("eye", 0, 0, 0);
	skyShader->setUniform3f("lightDirection", sunDirection.normalize());
	skyShader->setUniformMatrix("cameraProjection", Mat4f(1,0,0,0,		0,1,0,0,	0,0,1,0,	0,0,0,1));
	skyShader->setUniform2f("mapCenter", 0, 0);

	//Positive X
	graphics->startRenderToTexture(skyTexture,GraphicsManager::textureCube::POSITIVE_X);
	skyShader->setUniform3f("fwd",		1,0,0);
	skyShader->setUniform3f("up",		0,-1,0);
	skyShader->setUniform3f("right",	0,0,-1);
	graphics->drawOverlay(Rect::XYXY(	-1, -1, 1, 1));
	graphics->endRenderToTexture();

	//Positive Y
	graphics->startRenderToTexture(skyTexture,GraphicsManager::textureCube::POSITIVE_Y);
	skyShader->setUniform3f("fwd",		0,1,0);
	skyShader->setUniform3f("up",		0,0,1);
	skyShader->setUniform3f("right",	1,0,0);
	graphics->drawOverlay(Rect::XYXY(	-1, -1, 1, 1));
	graphics->endRenderToTexture();

	//Positive Z
	graphics->startRenderToTexture(skyTexture,GraphicsManager::textureCube::POSITIVE_Z);
	skyShader->setUniform3f("fwd",		0,0,1);
	skyShader->setUniform3f("up",		0,-1,0);
	skyShader->setUniform3f("right",	1,0,0);
	graphics->drawOverlay(Rect::XYXY(	-1, -1, 1, 1));
	graphics->endRenderToTexture();

	//Negative X
	graphics->startRenderToTexture(skyTexture,GraphicsManager::textureCube::NEGATIVE_X);
	skyShader->setUniform3f("fwd",		-1,0,0);
	skyShader->setUniform3f("up",		0,-1,0);
	skyShader->setUniform3f("right",	0,0,1);
	graphics->drawOverlay(Rect::XYXY(	-1, -1, 1, 1));
	graphics->endRenderToTexture();

	//Negative Y
	graphics->startRenderToTexture(skyTexture,GraphicsManager::textureCube::NEGATIVE_Y);
	skyShader->setUniform3f("fwd",		0,-1,0);
	skyShader->setUniform3f("up",		0,0,-1);
	skyShader->setUniform3f("right",	-1,0,0);
	graphics->drawOverlay(Rect::XYXY(	-1, -1, 1, 1));
	graphics->endRenderToTexture();

	//Negative Z
	graphics->startRenderToTexture(skyTexture,GraphicsManager::textureCube::NEGATIVE_Z);
	skyShader->setUniform3f("fwd",		0,0,-1);
	skyShader->setUniform3f("up",		0,-1,0);
	skyShader->setUniform3f("right",	-1 ,0,0);
	graphics->drawOverlay(Rect::XYXY(	-1, -1, 1, 1));
	graphics->endRenderToTexture();

	//skyTexture->generateMipmaps(); <- might be needed...
//	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);

	t = GetTime() - t;
	t = GetTime();

	//unsigned char* texData = skyTexture->getData();
	//shared_ptr<FileManager::textureFile> textureFile(new FileManager::textureFile("../skyTexture.png",FileManager::PNG));
	//textureFile->channels = 3;
	//textureFile->contents = texData;
	//textureFile->width = 256;
	//textureFile->height = 256*3;
	//fileManager.writeFile(textureFile); //will delete[] texData
}
//void Terrain::generateSky(Angle theta, Angle phi, float L)//see "Rendering Physically-Based Skyboxes" - Game Engine Gems 1 and http://www.cs.utah.edu/~shirley/papers/sunsky/sunsky.pdf
//{//theta = angle from sun to zenith, phi = angle from south axis (positive is towards east)
//	double t = GetTime();
//
//	unsigned int l = 64; //length of sides of square for each face in cube map
//	float m = 0.5 * l - 0.5;
//	float* cubeMap = new float[l*l * 6 * 3];
//	unsigned char* cubeMapTex = new unsigned char[l*l * 6 * 3];
//
//	Vec3f sunDirection(cos(phi)*sin(theta), cos(theta), sin(phi)*sin(theta));
//	sunDirection = sunDirection.normalize();//is this necessary?
//
//
//	//xyY color space stored as:		x						  Y						  y			
//	float constants[5][3] =	{{-0.0193f*L - 0.2592f,		 0.1787f*L - 1.4630f,		-0.0167f*L - 0.2608f},
//							 {-0.0665f*L + 0.0008f,		-0.3554f*L + 0.4275f,		-0.0950f*L + 0.0092f},
//							 {-0.0004f*L + 0.2125f,		-0.0227f*L + 5.3251f,		-0.0079f*L + 0.2102f},
//							 {-0.0641f*L - 0.8989f,		 0.1206f*L - 2.5771f,		-0.0441f*L - 1.6537f},
//							 {-0.0033f*L + 0.0452f,		-0.0670f*L + 0.3703f,		-0.0109f*L + 0.0529f}};
//
//
//
//	float theta2 = theta*theta;
//	float theta3 = theta*theta*theta;
//	float chi = (4.0/9.0 - L/120.0)*(PI - 2*theta);
//	float zenithLuminance = abs((4.0453*L - 4.9710) * tan(chi) - 0.2155*L + 0.24192);// * 1000;
//	float zenith_x = L*L*(0.00166*theta3 - 0.00375*theta2 + 0.00209*theta) + L*(-0.02903*theta3 + 0.06377*theta2 - 0.03202*theta + 0.00394) + (0.11693*theta3 - 0.21196*theta2 + 0.06052*theta + 0.25886);
//	float zenith_y = L*L*(0.00275*theta3 - 0.00610*theta2 + 0.00317*theta) + L*(-0.04214*theta3 + 0.08970*theta2 - 0.04153*theta + 0.00516) + (0.15346*theta3 - 0.26756*theta2 + 0.06670*theta + 0.26688);
//	float zenithOverSun[3] = {static_cast<float>(0.85f*zenith_x /		((1.0 + constants[0][0] * exp(constants[1][0])) * (1.0f + constants[2][0]*exp(constants[3][0]*(float)theta) + constants[4][0]*cos(theta)*cos(theta)))),
//							  static_cast<float>(0.20f*zenithLuminance / ((1.0 + constants[0][1] * exp(constants[1][1])) * (1.0f + constants[2][1]*exp(constants[3][1]*(float)theta) + constants[4][1]*cos(theta)*cos(theta)))),
//							  static_cast<float>(0.85f*zenith_y /		((1.0 + constants[0][2] * exp(constants[1][2])) * (1.0f + constants[2][2]*exp(constants[3][2]*(float)theta) + constants[4][2]*cos(theta)*cos(theta))))};
//
//	//float zenithOverSun[3] = {1.0/0.186220,
//	//						  1.0/0.13,
//	//						  1.0/0.130598};
//
//	Vec3f direction;
//	unsigned int i = 0;
//	for(int face=0; face<6; face++)
//	{
//		for(int x=0;x<l;x++)
//		{
//			for(int y=0;y<l;y++)
//			{
//				if(face == 0)		direction.set(m, m - x, m - y); //right
//				else if(face == 1)	direction.set(-m, m - x, -m + y); //left
//				else if(face == 2)	direction.set(-m + y, m, -m + x); //top
//				else if(face == 3)	direction.set(-m + y, -m, m - x); //bottom
//				else if(face == 4)	direction.set(-m + y, m - x, m); //back
//				else if(face == 5)	direction.set(m - y, m - x, -m); //front
//
//				direction = direction.normalize();
//
//				float cos_sunAngle = direction.dot(sunDirection);
//				float sunAngle = acos(cos_sunAngle);
//				float reciprocal_cos_upAngle = 1.0 / max(direction.y,0.01);//cos_upAng should really be just direction.y but this prevents the horizon from having artifacts

			//	if(sunAngle > PI/2) sunAngle = PI/4 + sunAngle * 0.5;
				
																												  //pow(constants[2][i], constants[3][i] * sunAngle) is the only term that dramatically effects the color accros the sky
//				cubeMap[i + 0] = (1.0 + constants[0][0] * exp(constants[1][0]*reciprocal_cos_upAngle)) * (1.0 + constants[2][0]*exp(constants[3][0] * sunAngle) + constants[4][0]*cos_sunAngle*cos_sunAngle) * zenithOverSun[0];
	// 			cubeMap[i + 1] = (1.0 + constants[0][1] * exp(constants[1][1]*reciprocal_cos_upAngle)) * (1.0 + constants[2][1]*exp(constants[3][1] * sunAngle) + constants[4][1]*cos_sunAngle*cos_sunAngle) * zenithOverSun[1];
	// 			cubeMap[i + 2] = (1.0 + constants[0][2] * exp(constants[1][2]*reciprocal_cos_upAngle)) * (1.0 + constants[2][2]*exp(constants[3][2] * sunAngle) + constants[4][2]*cos_sunAngle*cos_sunAngle) * zenithOverSun[2];
			
	// 			i +=3;

	// 		}
	// 	}
	// }
	
// 	t = GetTime() - t;
// 	t = GetTime();

// 	for(i=0; i < l*l*6*3; i+=3)// xyY -> XYZ (actually xYy -> XYZ)
// 	{
// 		float x = cubeMap[i + 0];
// 		float y = cubeMap[i + 2];


// 		cubeMap[i + 1] = 0.65 * cubeMap[i + 1] / (1.0 + cubeMap[i + 1]);	//tone mapping (optional?)
// 		//cubeMap[i + 1] = pow((double)cubeMap[i + 1], invGamma);

// 		cubeMap[i + 0] = cubeMap[i + 1] * x / y;
// 		cubeMap[i + 2] = cubeMap[i + 1] * (1.0 - x - y) / y;
// 	}

// 	t = GetTime() - t;
// 	t = GetTime();

// 	//const float invGamma = 1.0/1.8;
// 	for(i=0; i < l*l*6*3; i+=3)// XYZ -> rgb
// 	{
// 	//	float brightness = pow(clamp(cubeMap[i + 0], 0.0, 1.0),invGamma);
// 		cubeMapTex[i + 2] = clamp((cubeMap[i + 0] *  3.240479		+		cubeMap[i + 1] * -1.53715		+		cubeMap[i + 2] * -0.498530),0.0,1.0) * 255.0;
// 		cubeMapTex[i + 1] = clamp((cubeMap[i + 0] * -0.969256		+		cubeMap[i + 1] *  1.875991		+		cubeMap[i + 2] *  0.041556),0.0,1.0) * 255.0;
// 		cubeMapTex[i + 0] = clamp((cubeMap[i + 0] *  0.055648		+		cubeMap[i + 1] * -0.204043		+		cubeMap[i + 2] *  1.057311),0.0,1.0) * 255.0;
// 	}

// 	t = GetTime() - t;
// 	t = GetTime();

// 	skyTexture = graphics->genTextureCube();
// 	skyTexture->setData(l, l, GraphicsManager::texture::BGR, cubeMapTex);

// 	delete[] cubeMap;
// 	delete[] cubeMapTex;

// 	t = GetTime() - t;
// 	t = GetTime();
// }
void Terrain::generateTreeTexture(shared_ptr<SceneManager::mesh> treeMeshPtr)
{
	const double AOI = 45.0 * PI/180.0;
	
	double t = GetTime();

	treeTexture = graphics->genTexture2D();
	treeTexture->setData(512,512,GraphicsManager::texture::RGBA, false, false, nullptr);

	auto depthTex = graphics->genTexture2D();
	depthTex->setData(512,512,GraphicsManager::texture::DEPTH, false, false, nullptr);

	graphics->setDepthTest(false);

	graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);

	shared_ptr<GraphicsManager::View> view(new GraphicsManager::View());
	view->ortho(-15,15, -15.0*cos(AOI),15.0*cos(AOI), 0, 100);
	view->lookAt(Vec3f(30.0*cos(AOI), 15.0+30.0*sin(AOI), 0), Vec3f(0, 15, 0), Vec3f(0,-1,0));

	auto tree3DShader = shaders("trees3D shader");
	auto leavesMesh = dataManager.getModel("tree leaves");
	auto trunkMesh = dataManager.getModel("tree trunk");
	if(tree3DShader && trunkMesh && !trunkMesh->materials.empty() && leavesMesh && !leavesMesh->materials.empty())
	{	

		tree3DShader->bind();
		tree3DShader->setUniform1i("tex",0);
		tree3DShader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
		tree3DShader->setUniform1f("transparency", 0.9);
		tree3DShader->setUniform4f("position_scale",0, 0, 0, 30.0 / 23.73);

		graphics->startRenderToTexture(treeTexture, depthTex);
		graphics->setBlendMode(GraphicsManager::TRANSPARENCY);

		trunkMesh->materials.front().tex->bind(0);
		trunkMesh->materials.front().indexBuffer->bindBuffer(trunkMesh->VBO);
		trunkMesh->materials.front().indexBuffer->drawBuffer();

		leavesMesh->materials.front().tex->bind(0);
		leavesMesh->materials.front().indexBuffer->bindBuffer(leavesMesh->VBO);
		leavesMesh->materials.front().indexBuffer->drawBuffer();

		graphics->endRenderToTexture();
	}

	treeTexture->generateMipmaps();
	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);

	t = GetTime() - t;
	t = GetTime();

	unsigned char* texData = treeTexture->getData();
	shared_ptr<FileManager::textureFile> textureFile(new FileManager::textureFile("../treeTexture.png",FileManager::PNG));
	textureFile->channels = 1;
	textureFile->contents = texData;
	textureFile->width = 512;
	textureFile->height = 512;
	fileManager.writeFile(textureFile); //will delete[] texData
}
void Terrain::addDecal(Vec2f center, float width, float height, string texture, double startTime, double fadeInLength)
{
	cout << "decals not yet implemented.\n";
	/*if(width <= 0 || height <= 0)
		return;

	auto p = terrainPages.end();
	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
	{
		if((*i)->minXYZ.x < center.x && (*i)->maxXYZ.x > center.x && (*i)->minXYZ.z < center.y && (*i)->maxXYZ.z > center.y)
		{
			p = i;
			break;
		}
	}
	if(p != terrainPages.end())
	{
		auto page = *p;
		Vec2f worldMinXY(center.x - 0.5*width,  center.y - 0.5*height);
		Vec2f worldMaxXY(center.x + 0.5*width,  center.y + 0.5*height);

		Vec2f minXY((worldMinXY.x - page->minXYZ.x) * (page->width-1) / (page->maxXYZ.x - page->minXYZ.x), 
					(worldMinXY.y - page->minXYZ.z) * (page->height-1) / (page->maxXYZ.z - page->minXYZ.z));
		Vec2f maxXY((worldMaxXY.x - page->minXYZ.x) * (page->width-1) / (page->maxXYZ.x - page->minXYZ.x), 
					(worldMaxXY.y - page->minXYZ.z) * (page->height-1) / (page->maxXYZ.z - page->minXYZ.z));

		int x1 = clamp(floor(minXY.x), 0, page->width-2);
		int x2 = clamp(ceil(maxXY.x), x1+1, page->width-1);
		int y1 = clamp(floor(minXY.y), 0, page->width-2);
		int y2 = clamp(ceil(maxXY.y), y1+1, page->height-1);

		texturedVertex3D* verts = new texturedVertex3D[(1+x2-x1) * (1+y2-y1)];
		int i=0;
		for(int y = y1; y <= y2; y++)
		{
			for(int x = x1; x <= x2; x++)
			{
				Vec2f loc = Vec2f(page->minXYZ.x + (page->maxXYZ.x-page->minXYZ.x) * x / (page->width-1),  page->minXYZ.z + (page->maxXYZ.z-page->minXYZ.z) * y / (page->height-1)); 
				verts[i].position = Vec3f(loc.x, page->getHeight(loc)+0.2, loc.y);
				verts[i++].UV = Vec2f((loc.x-worldMinXY.x)/width, (loc.y-worldMinXY.y)/height);
			}
		}
		auto vertexBuffer = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
		vertexBuffer->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0*sizeof(float));
		vertexBuffer->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,	3*sizeof(float));
		vertexBuffer->setTotalVertexSize(sizeof(texturedVertex3D));
		vertexBuffer->setVertexData(sizeof(texturedVertex3D)*(1+x2-x1) * (1+y2-y1), verts);

		unsigned short* indices = new unsigned short[6*(x2-x1)*(y2-y1)];

		i=0;
		for(int y = 0; y < y2-y1; y++)
		{
			for(int x = 0; x < x2-x1; x++)
			{
				indices[i++] = x + y * (1+x2-x1);
				indices[i++] = x + (y+1) * (1+x2-x1);
				indices[i++] = (x+1) + y * (1+x2-x1);

				indices[i++] = (x+1) + y * (1+x2-x1);
				indices[i++] = (x+1) + (y+1) * (1+x2-x1);
				indices[i++] = x + (y+1) * (1+x2-x1);
			}
		}
		auto indexBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
		indexBuffer->setData(indices, GraphicsManager::TRIANGLES, 6*(x2-x1)*(y2-y1));

		delete[] verts;
		delete[] indices;

		decals.push_back(std::make_shared<decal>(texture, vertexBuffer, indexBuffer, fadeInLength));
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
//void Terrain::resetTerrain()
//{
//	decals.clear();
//	skyTexture.reset();
//	oceanTexture.reset();
//	treeTexture.reset();
//	terrainPages.clear();
//}
//void Terrain::initTerrain(unsigned short* Heights, unsigned short patchResolution, Vec3f position, Vec3f scale, TerrainType shader, Circle<float> bounds, float foliageDensity, unsigned int LOD)
//{
//}
Terrain::~Terrain()
{
	FractalNode::cleanUp();
}
void Terrain::terrainFrameUpdate(double cTime)
{
	FractalNode::frameNumber++;
	currentTime = cTime;

	graphics->startRenderToTexture(waveTexture, 0, nullptr, 0, false);
	graphics->setBlendMode(GraphicsManager::REPLACE);

	auto shader = shaders.bind("waves");
	shader->setUniform1f("time", currentTime * 20);
	shader->setUniform1fv("amplitudes", waves.size(), (float*)waves.amplitudes);
	shader->setUniform1fv("frequencies", waves.size(), (float*)waves.frequencies);
	shader->setUniform1fv("waveSpeeds", waves.size(), (float*)waves.speeds);
	shader->setUniform2fv("waveDirections", waves.size(), (Vec2f*)waves.directions);
	float p = 1.0 - 1.0 / waveTextureResolution;
	float t = 0.5 / waveTextureResolution;
	graphics->drawPartialOverlay(Rect::XYXY(-1, -1, 1, 1), 
								 Rect::XYWH(0, 0, 1, 1),
								 "wave lookup");
	graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);
	graphics->endRenderToTexture();
	waveTexture->generateMipmaps();

}
void Terrain::renderTerrain(shared_ptr<GraphicsManager::View> view) const
{
//	Vec3f sunDirection = (graphics->getLightPosition()+eye).normalize();
//	Vec3f sunPos = view->project3((graphics->getLightPosition()/*+eye*/).normalize() * 300000.0);
//	if(sunPos.z < 1.0)
//	{
//		shaders.bind("ortho");
//		graphics->setColor(1,1,0.7,0.2);
//		
//		Vec3f sunUp = sunDirection.cross(Vec3f(0,0,1));	//sun cannot be (0,0,1) at!!!
//		float up = view->camera().up.dot(sunUp);
//		float right = view->camera().right.dot(sunUp);
//
//		Angle rotateAng = atan2A(up,right);
//		graphics->setBlendMode(GraphicsManager::ADDITIVE);
//		graphics->drawRotatedOverlay(Rect::CWH(sunPos.x,sunPos.y,0.25,0.25), rotateAng, "sun"); //sun get stretched in split screen mode...
//		graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
//	}

	/*if(!decals.empty())
	{
		graphics->setDepthMask(false);
		auto shader = shaders.bind("decal");
		shader->setUniform1i("tex", 0);
		shader->setUniformMatrix("cameraProjection", view->projectionMatrix() * view->modelViewMatrix());
		shader->setUniformMatrix("modelTransform", Mat4f());
		for(auto i = decals.begin(); i != decals.end(); i++)
		{
			shader->setUniform4f("color", 1,1,1, clamp((world->time() - (*i)->startTime) /(*i)->fadeLength, 0.0,1.0));
			dataManager.bind((*i)->texture);
			(*i)->vertexBuffer->bindBuffer();
			(*i)->indexBuffer->bindBuffer();
			(*i)->indexBuffer->drawBuffer();
		}
		shader->setUniform4f("color", 1,1,1,1);
		graphics->setDepthMask(true);
	}*/


	fractalTerrain->computeSubdivision(view, 5);
	fractalTerrain->computeWaterSubdivision(view, 3);


//	graphics->setWireFrame(true);
	renderFractalTerrain(view);
//	graphics->setWireFrame(false);


//	graphics->setWireFrame(true);
	renderFractalWater(view);
//	graphics->setWireFrame(false);

	auto skyShader = shaders.bind("sky2 shader");
	Vec3f fwd = view->camera().fwd;
	Vec3f up = view->camera().up;
	Vec3f right = view->camera().right;

	dataManager.bind("noise",0);
	skyShader->setUniform1f("time", currentTime);
	skyShader->setUniform1i("noise", 0);
	skyShader->setUniform3f("eye", view->camera().eye);
	skyShader->setUniform3f("fwd", fwd);
	skyShader->setUniform3f("up", up * tan(view->projection().fovy/2 * PI/180.0));
	skyShader->setUniform3f("right", right * tan(view->projection().fovy/2 * PI/180.0)*view->projection().aspect);
	skyShader->setUniform3f("lightDirection", graphics->getLightPosition().normalize());
	skyShader->setUniformMatrix("cameraProjection", view->projectionMatrix() * view->modelViewMatrix());
	skyShader->setUniform2f("mapCenter", 0, 0);
	graphics->drawOverlay(Rect::XYWH(	-1.0 - 2.0*view->viewConstraint().x / view->viewConstraint().w, 
										-1.0 - 2.0*view->viewConstraint().y / view->viewConstraint().h,
										 2.0 / view->viewConstraint().w,
										 2.0 / view->viewConstraint().h));//(view->viewport().x,view->viewport().y,view->viewport().width,view->viewport().height));
}
void Terrain::renderFractalTerrain(shared_ptr<GraphicsManager::View> view) const
{
	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);

	auto shader = shaders.bind("fractal terrain");
//		auto texture = clipMap->layers[clipMapLayer].texture;
	Vec2f tScale = Vec2f(1,1);//Vec2f(1,1) / (1 << level) * (1 >> clipMapLayer);
	Vec2f tOrigin = Vec2f(0,0);//Vec2f(coordinates.x, coordinates.y) / (1 << level) * (1 >> clipMapLayer);

	shader->setUniformMatrix("cameraProjection", view->projectionMatrix() * view->modelViewMatrix());
	shader->setUniformMatrix("modelTransform", Mat4f());
	shader->setUniform1f("earthRadius", earthRadius);
	shader->setUniform1i("groundTex", 0);

	shader->setUniform2f("tOrigin", tOrigin + 0.5 / FractalNode::textureResolution);
	shader->setUniform2f("tScale", Vec2f(1,1)*(FractalNode::textureResolution-1) / FractalNode::textureResolution);
	shader->setUniform3f("eyePosition", view->camera().eye);
	shader->setUniform3f("sunDirection", graphics->getLightPosition().normalize());
	shader->setUniform1i("LCnoise",	 1); dataManager.bind("LCnoise",1);
	shader->setUniform1i("noiseTex", 2); dataManager.bind("noise",2);
	shader->setUniform1i("sand",     3); dataManager.bind("sand", 3);
	shader->setUniform1i("grass",    4); dataManager.bind("grass",4);
	shader->setUniform1i("grassDetail",	5);	dataManager.bind("grass detail",5);
		
	fractalTerrain->render(view, shader);

	

}
void Terrain::renderFractalWater(shared_ptr<GraphicsManager::View> view) const
{
	graphics->setBlendMode(GraphicsManager::PREMULTIPLIED_ALPHA);


	Vec3f eye = view->camera().eye;
	float scale = earthRadius * sqrt(1.0-pow(earthRadius/(earthRadius+eye.y),2));

	auto shader = shaders.bind("fractal ocean");
	shader->setUniformMatrix("cameraProjection", view->projectionMatrix() * view->modelViewMatrix());
	shader->setUniformMatrix("modelTransform", Mat4f(Quat4f(),Vec3f(eye.x,0,eye.z), scale));
	shader->setUniform1f("earthRadius", earthRadius);
	shader->setUniform1i("groundTex", 3);
	shader->setUniform3f("eyePosition", eye);
	shader->setUniform3f("sunDirection", graphics->getLightPosition().normalize());
	shader->setUniform2f("invScreenDims",1.0/sw, 1.0/sh);

	shader->setUniform1f("time", currentTime);

	shader->setUniform1f("invTextureScale", 1.0 / waveTextureScale);

	shader->setUniform1i("waves", 1); 
	dataManager.bind("wave lookup", 1);
	
	//shader->setUniform1i("oceanNormals", 2);
	//dataManager.bind("ocean normals", 2);
	shader->setUniform1i("sky", 0);	
	skyTexture->bind(0);

	shader->setUniform1i("waveTexture", 2);
	waveTexture->bind(2);
	shader->setUniform1fv("amplitudes", waves.size(), (float*)waves.amplitudes);
	shader->setUniform1fv("frequencies", waves.size(), (float*)waves.frequencies);
	shader->setUniform1fv("waveSpeeds", waves.size(), (float*)waves.speeds);
	shader->setUniform2fv("waveDirections", waves.size(), (Vec2f*)waves.directions);
	shader->setUniform1f("colorMult", 1.0f);
//	fractalTerrain->renderWater(view, shader);

	waterIBO->bindBuffer(waterVBO);

	graphics->setColorMask(false);
	waterIBO->drawBuffer();
	graphics->setColorMask(true);
	waterIBO->drawBuffer();

	graphics->setBlendMode(GraphicsManager::TRANSPARENCY);	

	if(wireframe)
	{
		shader->setUniform1f("colorMult", 0.0f);
		graphics->setWireFrame(true);
		waterIBO->drawBuffer();
		graphics->setWireFrame(false);
	}
}
void Terrain::renderFoliage(shared_ptr<GraphicsManager::View> view) const
{
	//for(auto i=trees.begin(); i!=trees.end(); i++)
	//{
	//	sceneManager.drawMesh(view, dataManager.getModel("tree model"), Mat4f(Quat4f(),i->location));
	//}
//	skyTexture->bind(1);
//	for(auto i = terrainPages.begin(); i != terrainPages.end(); i++)
//	{
//		(*i)->renderFoliage(view);
//	}

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
/*shared_ptr<Terrain::Page> Terrain::getPage(Vec2f position) const
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
	}*/
void Terrain::setWireframe(bool w)
{
	wireframe = w;
}
float Terrain::elevation(Vec2f v) const
{
	//TODO: implement
	return 0.0f;
}
float Terrain::elevation(float x, float z) const
{
	return elevation(Vec2f(x,z));
}
float Terrain::altitude(Vec3f v) const
{
	return v.y - elevation(v.x, v.z);
}
float Terrain::altitude(float x, float y, float z) const
{
	return altitude(Vec3f(x,y,z));
}
Vec3f Terrain::terrainNormal(Vec2f v) const
{
	//TODO: implement?
	return Vec3f(0,1,0);
}
Vec3f Terrain::terrainNormal(float x, float z) const
{
	return terrainNormal(Vec2f(x,z));
}
bool Terrain::isLand(Vec2f v) const
{
	//TODO: implement
	return true;
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
//	// lb is the corner of AABB with minmal coordinates - left bottom, rt is maximal corner
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
