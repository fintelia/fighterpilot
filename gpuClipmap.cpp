
#include "engine.h"

Terrain::GpuClipMap::GpuClipMap(float sLength, unsigned int resolution,
                                unsigned int num_layers, 
                                const vector<unique_ptr<float[]>>& pinnedLayers):
    sideLength(sLength),
    layerResolution(resolution),
    numPinnedLayers(pinnedLayers.size()),
    resolution(layerResolution << (num_layers-1)),
    blockStep(4),
    meshResolution(layerResolution/(2*blockStep)-1),
    blockResolution((meshResolution - 1) * blockStep + 1),
    layers(num_layers)
{
    assert(num_layers > 0);
    assert(num_layers >= pinnedLayers.size());

    for(unsigned int i = 0; i < num_layers; i++){
        if(i < numPinnedLayers){
            float* ptr = pinnedLayers[i].get();
            layers[i].heights = graphics->genTexture2D();
            layers[i].heights->setData(layerResolution, layerResolution,
                                  GraphicsManager::texture::R32F, true,
                                  false, (unsigned char*)ptr);
            layers[i].normals = graphics->genTexture2D();
            layers[i].normals->setData(layerResolution, layerResolution,
                                       GraphicsManager::texture::RGB, true,
                                       false, nullptr);
            layers[i].textureCenter.x = layerResolution/2;
            layers[i].textureCenter.y = layerResolution/2;
            generateAuxiliaryMaps(i);
        } else {
            layers[i].heights = graphics->genTexture2D();
            layers[i].heights->setData(layerResolution, layerResolution,
                                       GraphicsManager::texture::R32F, true,
                                       false, nullptr);
            layers[i].normals = graphics->genTexture2D();
            layers[i].normals->setData(layerResolution, layerResolution,
                                       GraphicsManager::texture::RGB, true,
                                       false, nullptr);

            layers[i].center = Vec2i(0,0);
            layers[i].targetCenter = Vec2i(0,0);
            synthesizeHeightmap(i);
            generateAuxiliaryMaps(i);
        }
        layers[i].textureCenter = Vec2u(layerResolution/2, layerResolution/2);
    }

    size_t vboDataSize = 2 * meshResolution * meshResolution;
    unique_ptr<float[]> vboData{new float[vboDataSize]};

    for(unsigned int x = 0; x < meshResolution; x++){
        for(unsigned int y = 0; y < meshResolution; y++){
            vboData[2*(x + meshResolution*y)] = x;
            vboData[2*(x + meshResolution*y) + 1] = y;
        }
    }
	clipMapVBO = graphics->genVertexBuffer(
        GraphicsManager::vertexBuffer::STATIC);
	clipMapVBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION2, 0);
	clipMapVBO->setTotalVertexSize(sizeof(float)*2);
	clipMapVBO->setVertexData(sizeof(float)*vboDataSize, vboData.get());


    // size is an over estimate, but is easier than computing the actual value,
    // and the memory is freed at the end of this function.
    size_t iboDataSize = 6*((meshResolution-1)*(meshResolution-1)) * 2;
    unique_ptr<uint32_t[]> iboData{new uint32_t[iboDataSize]};
    size_t index = 0;

    // Supports using negative values to index from the other side of the grid
    auto addTriangle = [&](int ax, int ay, int bx, int by, int cx, int cy){
        auto mod = [&](int x) -> uint32_t {
            return (x + meshResolution) % meshResolution;
        };

        iboData[index++] = mod(ax) + mod(ay) * meshResolution;
        iboData[index++] = mod(bx) + mod(by) * meshResolution;
        iboData[index++] = mod(cx) + mod(cy) * meshResolution;
    };
    
    // ring
    unsigned int ringSize = (meshResolution - 1) / 4;
    for(int p = 0; p < 2; p++){
        for(int i = 0; i < (meshResolution-1)/2; i++){
            addTriangle(2*i,0,   2*i+2,0,  2*i+1,1);  // y = 0
            addTriangle(2*i,-1,  2*i+2,-1, 2*i+1,-2); // y = blockResolution -1
            addTriangle(0,2*i,   0,2*i+2,  1,2*i+1);  // x = 0
            addTriangle(-1,2*i,  -1,2*i+2, -2,2*i+1); // x = blockResolution - 1

            if(i > 0){
                addTriangle(2*i,0,   2*i,1,   2*i+1,1);
                addTriangle(2*i,-1,  2*i,-2,  2*i+1,-2);
                addTriangle(0,2*i,   1,2*i,   1,2*i+1);
                addTriangle(-1,2*i,  -2,2*i,  -2,2*i+1);
            }
            if(i < (meshResolution-1)/2 - 1){
                addTriangle(2*i+2,0,   2*i+2,1,   2*i+1,1);
                addTriangle(2*i+2,-1,  2*i+2,-2,  2*i+1,-2);
                addTriangle(0,2*i+2,   1,2*i+2,   1,2*i+1);
                addTriangle(-1,2*i+2,  -2,2*i+2,  -2,2*i+1);
            }
        }
        for(int y = 1; y < meshResolution-2; y++){
            for(int x = 1; x < meshResolution-2; x++){
                if((x < ringSize+1 || x >= meshResolution-ringSize-1) ||
                   (y < ringSize+1 || y >= meshResolution-ringSize-1)){
                    if(p == 0){
                        addTriangle(x,y,      x+1,y+1,   x+1,y);
                        addTriangle(x,y,      x+1,y+1,   x,y+1);
                    }else{
                        addTriangle(x,y,      x,y+1,     x+1,y);
                        addTriangle(x+1,y,    x+1,y+1,   x,y+1);
                    }
                }
            }
        }

        if(p == 0){
            numRingIndices = index;
        }
    }
    // center
    for(int x = ringSize+1; x < meshResolution-ringSize-1; x++){
        for(int y = ringSize+1; y < meshResolution-ringSize-1; y++){
            addTriangle(x,y,      x,y+1,     x+1,y);
            addTriangle(x+1,y,    x+1,y+1,   x,y+1);
        }
    }
    numCenterIndices = index - numRingIndices;
    clipMapIBO = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
    clipMapIBO->setData(iboData.get(), GraphicsManager::TRIANGLES, index);
}

void Terrain::GpuClipMap::synthesizeHeightmap(unsigned int layer)
{
    unique_ptr<GraphicsManager::timer> t = graphics->genTimer();
    t->start();
    debugAssert(layer > 0);

    float layerScale = 1 << (layers.size()-1-layer);
    
    auto shader = shaders.bind("clipmap synthesize");
	shader->setUniform1i("parent_heightmap", 0);
    shader->setUniform1i("parent_normalmap", 1);
    shader->setUniform1i("layerScale", layerScale);
    shader->setUniform1f("texelSize", (sideLength*layerScale) / (resolution-1));
    shader->setUniform1i("resolution", resolution);
    shader->setUniform1i("layerResolution", layerResolution);
    shader->setUniform2i("parent_center", (Vec2i)layers[layer-1].center);
    shader->setUniform2i("center", (Vec2i)layers[layer].center);
    
    layers[layer-1].heights->bind(0);
    layers[layer-1].normals->bind(1);

    graphics->startRenderToTexture(layers[layer].heights, 0, nullptr, 0, true);
    graphics->setBlendMode(GraphicsManager::REPLACE);
	graphics->drawOverlay(Rect::XYXY(-1, -1, 1, 1));
    graphics->setBlendMode(GraphicsManager::TRANSPARENCY);
	graphics->endRenderToTexture();
    t->stop();
    layers[layer].heights->generateMipmaps();
}
void Terrain::GpuClipMap::generateAuxiliaryMaps(unsigned int layer)
{
    auto shader = shaders.bind("clipmap auxmapgen");
    shader->setUniform1f("invTexelSize", (resolution-1)
                         / (sideLength * (1<<(layers.size()-1-layer))));
	shader->setUniform1i("heightmap", 0);
    layers[layer].heights->bind(0);
    
    graphics->startRenderToTexture(layers[layer].normals, 0, nullptr, 0, true);
	graphics->drawOverlay(Rect::XYXY(-1, -1, 1, 1));
	graphics->endRenderToTexture();
    layers[layer].normals->generateMipmaps();
}
void Terrain::GpuClipMap::regenLayer(unsigned int layer)
{
    layers[layer].center = layers[layer].targetCenter;
    synthesizeHeightmap(layer);
    generateAuxiliaryMaps(layer);
}
void Terrain::GpuClipMap::centerClipMap(Vec2f center)
{
    unsigned int num_layers = layers.size();

    // TODO fix these bounds?
    int bound = (layerResolution/2 - blockResolution/2 - 2*blockStep)
        << (num_layers - numPinnedLayers);

    Vec2f ncenter = center/sideLength+0.5;
    Vec2i icenter{(int)(ncenter.x*resolution), (int)(ncenter.y*resolution)};

    icenter -= resolution/2;
    icenter.x = clamp(icenter.x, -bound, bound);
    icenter.y = clamp(icenter.y, -bound, bound);
    
    for(unsigned int i = 0; i < layers.size(); i++){
        auto& layer = layers[i];

        // Distance between any two samples in this layer, relative to the most
        // detailed layer.
        unsigned int layerScale = 1 << (num_layers-i-1);
        
        unsigned int step = blockStep * layerScale * 2;
        layers[i].targetCenter = ((icenter+resolution) / step) * step
            - resolution;

        int dx = abs(layers[i].targetCenter.x - layer.center.x);
        int dy = abs(layers[i].targetCenter.y - layer.center.y);
        if((dx > layerResolution/8 || dy > layerResolution/8) &&
           i >= numPinnedLayers){
            regenLayer(i);
        }
    }
}
void Terrain::GpuClipMap::render(shared_ptr<GraphicsManager::View> view)
{
    auto shader = shaders.bind("clipmap terrain");
	shader->setUniformMatrix("cameraProjection",
                            view->projectionMatrix() * view->modelViewMatrix());
	shader->setUniform1f("earthRadius", earthRadius);
    shader->setUniform1i("resolution", meshResolution-1);
    shader->setUniform1f("invResolution", 1.0 / (meshResolution-1));
    shader->setUniform1i("textureStep", blockStep);
    shader->setUniform3f("eyePosition", view->camera().eye);
	shader->setUniform3f("sunDirection",
                         graphics->getLightPosition().normalize());
    shader->setUniform1f("time", world->time() / 1000.0);
    
	shader->setUniform1i("heightmap", 0);
    shader->setUniform1i("normalmap", 1);

	shader->setUniform1i("grass", 2);
    dataManager.bind("grass", 2);

    shader->setUniform1i("oceanHeights", 3);
    dataManager.bind("fft ocean heights", 3);
    shader->setUniform1i("oceanNormals", 4);
    dataManager.bind("fft ocean normals", 4);
    shader->setUniform1i("oceanNormals2", 5);
    dataManager.bind("fft ocean2 normals", 5);

    shader->setUniform1i("oceanGradient", 6);
    dataManager.bind("ocean gradient", 6);

    shader->setUniform1i("LCnoise", 7);
    dataManager.bind("LCnoise", 7);
    
    clipMapIBO->bindBuffer(clipMapVBO);
    for(int i = 0; i < layers.size(); i++){
        unsigned int layerScale = 1 << (layers.size()-i-1);
        float slength = sideLength * (blockResolution - 1) * layerScale
            / (resolution - 1);
        Vec2f worldOrigin =
            (layers[i].targetCenter - ((blockResolution-1)/2-blockStep) *
             layerScale);
        worldOrigin = (worldOrigin-0.5) * sideLength/(resolution-1);
        Vec2i textureOrigin =
            layers[i].textureCenter - ((blockResolution-1)/2-blockStep);
        textureOrigin -= (layers[i].center - layers[i].targetCenter)
            / layerScale;
        
        shader->setUniform1f("sideLength", slength);
        shader->setUniform2f("worldOrigin", worldOrigin.x, worldOrigin.y);
        shader->setUniform2i("textureOrigin", textureOrigin.x, textureOrigin.y);

        layers[i].heights->bind(0);
        layers[i].normals->bind(1);

        // TODO: factor in actual terrain height at viewer location, and make
        // max LOD decisions in update/regen phase instead of render.
        if(i < layers.size()-1 && slength+2000 > 0.4 * view->camera().eye.y){
            Vec2i scCenter = layers[i+1].targetCenter/(layerScale*blockStep)+1;
            Vec2i fs = Vec2i(((scCenter.x % 2) + 2) % 2,
                             ((scCenter.y % 2) + 2) % 2);

            shader->setUniform2i("flipAxis", fs.x, fs.y);
            clipMapIBO->drawBuffer(numRingIndices,
                                   fs.x==fs.y ? 0 : numRingIndices);
        }
        else{
            shader->setUniform2i("flipAxis", 0, 0);
            clipMapIBO->drawBuffer(numCenterIndices, numRingIndices);
            break;
        }
    }
}
