
#include "engine.h"

void SceneManager::meshInstance::update(const Mat4f& transformation,bool render)
{
	modelTransform = transformation;
	setRenderFlag(render);
}
bool SceneManager::meshInstance::renderFlag()
{
	return (flags & 0x01) != 0;
}
void SceneManager::meshInstance::setRenderFlag(bool b)
{
	if(b)	flags |=  0x01;
	else	flags &= ~0x01;
}
Sphere<float> SceneManager::meshInstance::getBoundingSphere()
{
	return (!meshPtr.expired()) ? meshPtr.lock()->boundingSphere  : Sphere<float>(Vec3f(0,0,0), 0);
}
shared_ptr<SceneManager::meshInstance> SceneManager::newMeshInstance(shared_ptr<mesh> meshPtr, Mat4f transformation)
{
	if(meshPtr)
	{
		shared_ptr<meshInstance> ptr(new meshInstance(meshPtr,transformation));
		nMeshInstances[meshPtr->meshID].push_back(ptr);
		return ptr;
	}
	return shared_ptr<meshInstance>();
}
shared_ptr<SceneManager::meshInstance> SceneManager::newChildMeshInstance(shared_ptr<mesh> meshPtr, weak_ptr<meshInstance> parent, Mat4f transformation)
{
	if(meshPtr)
	{
		shared_ptr<meshInstance> ptr(new meshInstance(meshPtr,transformation,parent));
		nMeshInstances[meshPtr->meshID].push_back(ptr);
		return ptr;
	}
	return shared_ptr<meshInstance>();
}
shared_ptr<SceneManager::mesh> SceneManager::createMesh(shared_ptr<FileManager::modelFile> modelFile)
{
	if(modelFile && modelFile->valid())
	{
		shared_ptr<mesh> nMesh(new mesh);
		nMesh->boundingSphere = modelFile->boundingSphere;
		nMesh->VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
		nMesh->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
		nMesh->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::NORMAL,		3*sizeof(float));
		nMesh->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TANGENT,		6*sizeof(float));
		nMesh->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::BITANGENT,	9*sizeof(float));
		nMesh->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		12*sizeof(float));
		nMesh->VBO->setTotalVertexSize(sizeof(normalMappedVertex3D));
		nMesh->VBO->setVertexData(sizeof(normalMappedVertex3D)*modelFile->vertices.size(), !modelFile->vertices.empty() ? &modelFile->vertices[0] : NULL);

		mesh::material mat;
		for(auto i = modelFile->materials.begin(); i != modelFile->materials.end(); i++)
		{
			mat.diffuse = i->diffuse;
			mat.specular = i->specular;
			mat.hardness = i->hardness;

			mat.tex			= dataManager.getTexture(i->textureMap_filename);
			mat.normalMap	= dataManager.getTexture(i->normalMap_filename);
			mat.specularMap	= dataManager.getTexture(i->specularMap_filename);
			//if(i->tex)			debugAssert(i->tex->valid());
			//if(i->normalMap)	debugAssert(i->normalMap->valid());
			//if(i->specularMap)	debugAssert(i->specularMap->valid());

			//if(i->tex && i->tex->valid())
			//{
			//	mat.tex = graphics->genTexture2D();
			//	mat.tex->setData(i->tex->width, i->tex->height, (GraphicsManager::texture::Format)i->tex->channels, i->tex->contents,true);
			//}
			//else
			//{
			//	mat.tex = static_pointer_cast<GraphicsManager::texture2D>(dataManager.getTexture("white"));
			//}
			//if(i->normalMap && i->normalMap->valid())
			//{
			//	mat.normalMap = graphics->genTexture2D();
			//	mat.normalMap->setData(i->normalMap->width, i->normalMap->height, (GraphicsManager::texture::Format)i->normalMap->channels, i->normalMap->contents,true);
			//}
			//else
			//{
			//	mat.normalMap = static_pointer_cast<GraphicsManager::texture2D>(dataManager.getTexture("default normals"));
			//}
			//if(i->specularMap && i->specularMap->valid())
			//{
			//	mat.specularMap = graphics->genTexture2D();
			//	mat.specularMap->setData(i->specularMap->width, i->specularMap->height, (GraphicsManager::texture::Format)i->specularMap->channels, i->specularMap->contents,true);
			//}
			//else
			//{
			//	mat.specularMap = static_pointer_cast<GraphicsManager::texture2D>(dataManager.getTexture("white"));
			//}
			mat.indexBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
			mat.indexBuffer->setData(!i->indices.empty() ? &i->indices[0] : nullptr, GraphicsManager::TRIANGLES, i->indices.size());
			nMesh->materials.push_back(mat);
		}
		nMesh->meshID = meshes.size();
		meshes.push_back(nMesh);
		return nMesh;
	}
	return shared_ptr<mesh>();
}
//void SceneManager::newTemperaryMesh(string model, Vec3f position, Quat4f rotation)
//{
//	shared_ptr<meshInstance> ptr(new meshInstance(model,position,rotation));
//	ptr->setTemperaryFlag(true);
//	meshInstances[model].push_back(ptr);
//}
void SceneManager::reset()
{
	lights.clear();
	nMeshInstances.clear();
}
void SceneManager::renderScene(shared_ptr<GraphicsManager::View> view, meshInstancePtr firstPersonObject)
{
	for(auto meshType=nMeshInstances.begin(); meshType!=nMeshInstances.end(); meshType++)
	{
		meshType->second.erase(std::remove_if(meshType->second.begin(), meshType->second.end(), [](weak_ptr<meshInstance> p)->bool{return p.expired();}), meshType->second.end());
	}

	shared_ptr<meshInstance> i;
	
	auto model = shaders.bind("model");
	model->setUniform1i("tex",0);
	model->setUniform1i("specularMap",1);
	model->setUniform1i("normalMap",2);	
	model->setUniform3f("eyePosition", view->camera().eye);
	model->setUniform3f("lightPosition", graphics->getLightPosition());
	model->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());

	for(auto meshType=nMeshInstances.begin(); meshType!=nMeshInstances.end(); meshType++)
	{
		if(meshType->second.empty())
			continue;

		auto meshPtr = meshes[meshType->first];
		if(!meshPtr)
			continue;

		if(!meshPtr->materials.empty())
			meshPtr->VBO->bindBuffer();
		for(auto material = meshPtr->materials.begin(); material != meshPtr->materials.end(); material++)
		{
			if(/*(*/material->diffuse.a > 0.999/* && pass == 0) || (material->diffuse.a <= 0.999 && pass == 1)*/)
			{
				if(material->tex)
					material->tex->bind(0);
				else
					dataManager.bind("white", 0);

				if(material->specularMap)
					material->specularMap->bind(1);
				else
					dataManager.bind("white", 1);

				if(material->specularMap)
					material->normalMap->bind(2);
				else
					dataManager.bind("default normals", 2);
				model->setUniform4f("diffuse", material->diffuse);
				model->setUniform3f("specular", material->specular);
				model->setUniform1f("hardness", material->hardness);
 
				if(!meshType->second.empty())
					material->indexBuffer->bindBuffer();
				for(auto instance = meshType->second.begin(); instance != meshType->second.end(); instance++)
				{
					i = instance->lock();
					if(i != firstPersonObject && (i->parent.expired() || i->parent.lock() != firstPersonObject) && i->renderFlag() /*&& view->sphereInFrustum(modelPtr->boundingSphere + (*instance)->position)*/)
					{
						model->setUniformMatrix("modelTransform", i->modelTransform);
						material->indexBuffer->drawBuffer();
					}
					i.reset();
				}
			}
		}
	}
}
void SceneManager::renderSceneTransparency(shared_ptr<GraphicsManager::View> view, meshInstancePtr firstPersonObject)
{
	for(auto meshType=nMeshInstances.begin(); meshType!=nMeshInstances.end(); meshType++)
	{
		meshType->second.erase(std::remove_if(meshType->second.begin(), meshType->second.end(), [](weak_ptr<meshInstance> p)->bool{return p.expired();}), meshType->second.end());
	}

	shared_ptr<meshInstance> i;
	
	graphics->setDepthMask(false);

	auto model = shaders.bind("model");
	model->setUniform1i("tex",0);
	model->setUniform1i("specularMap",1);
	model->setUniform1i("normalMap",2);
	model->setUniform3f("eyePosition", view->camera().eye);
	model->setUniform3f("lightPosition", graphics->getLightPosition());
	model->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	for(auto meshType=nMeshInstances.begin(); meshType!=nMeshInstances.end(); meshType++)
	{
		if(meshType->second.empty())
			continue;

		//auto modelPtr = dataManager.getModel(meshType->first);
		//if(modelPtr == nullptr)
		//	continue;

		auto meshPtr = meshes[meshType->first];
		if(!meshPtr)
			continue;

		if(!meshPtr->materials.empty())
			meshPtr->VBO->bindBuffer();
		for(auto material = meshPtr->materials.begin(); material != meshPtr->materials.end(); material++)
		{
			if(material->diffuse.a < 0.999)
			{
				if(material->tex)
					material->tex->bind(0);
				else
					dataManager.bind("white", 0);

				if(material->specularMap)
					material->specularMap->bind(1);
				else
					dataManager.bind("white", 1);

				if(material->specularMap)
					material->normalMap->bind(2);
				else
					dataManager.bind("default normals", 2);
				model->setUniform4f("diffuse", material->diffuse);
				model->setUniform3f("specular", material->specular);
				model->setUniform1f("hardness", material->hardness);
 
				if(!meshType->second.empty())
					material->indexBuffer->bindBuffer();
				for(auto instance = meshType->second.begin(); instance != meshType->second.end(); instance++)
				{
					i = instance->lock();
					if(i != firstPersonObject && (i->parent.expired() || i->parent.lock() != firstPersonObject) && i->renderFlag() /*&& view->sphereInFrustum(modelPtr->boundingSphere + (*instance)->position)*/)
					{
						model->setUniformMatrix("modelTransform", i->modelTransform);
						material->indexBuffer->drawBuffer();
					}
					i.reset();
				}
			}
		}
	}
	graphics->setDepthMask(true);
	//if(pass == 0) graphics->setDepthMask(false); // set to false after the first pass
	//if(pass == 1) graphics->setDepthMask(true);  // then reset to true after the second
}
void SceneManager::endRender()
{
	for(auto meshType=nMeshInstances.begin(); meshType!=nMeshInstances.end(); meshType++)
	{
		for(auto instance = (*meshType).second.begin(); instance != (*meshType).second.end(); instance++)
		{
			if(!instance->expired())
			{
				instance->lock()->setRenderFlag(false);
			}
		}
	}
}
void SceneManager::bindLights(shared_ptr<GraphicsManager::shader> shader)
{
	const int MAX_LIGHTS = 4;

	lights.erase(std::remove_if(lights.begin(),lights.end(), [](weak_ptr<pointLight> p)->bool{return p.expired();}),lights.end());

	Vec3f lightPositions[MAX_LIGHTS];
	Color3 lightColors[MAX_LIGHTS];
	float invLightStrengths[MAX_LIGHTS];
	int numLights = min(lights.size(), MAX_LIGHTS);

	for(int i=0; i<max(MAX_LIGHTS,lights.size()); i++)
	{
		if(i < numLights)
		{
			debugAssert(!lights[i].expired());
			lightPositions[i] = lights[i].lock()->position;
			invLightStrengths[i] = lights[i].lock()->strength > 0.0 ? 1.0 / lights[i].lock()->strength : 0.0;
			lightColors[i] = lights[i].lock()->color;
		}
		else if(i < MAX_LIGHTS) // if we have space for extra lights...
		{
			lightPositions[i] = Vec3f(0,0,0);
			invLightStrengths[i] = 0.0;
			lightColors[i] = black;
		}
		else // if we already has MAX_LIGHTS number of lights...
		{
			debugAssert(!lights[i].expired());
			float a = abs(world.altitude(lights[i].lock()->position));
			for(int j=0;j<MAX_LIGHTS;j++)
			{
				if(abs(world.altitude(lightPositions[j])) > a)
				{
					lightPositions[j] = lights[i].lock()->position;
					invLightStrengths[j] = lights[i].lock()->strength > 0.0 ? 1.0 / lights[i].lock()->strength : 0.0;
					lightColors[j] = lights[i].lock()->color;
				}
			}
		}
	}

	shader->setUniform3f("sunPosition", graphics->getLightPosition());

	//shader->setUniform1i("numLights", numLights);
	shader->setUniform3fv("lightPositions", MAX_LIGHTS, lightPositions);
	shader->setUniform1fv("invLightStrengths", MAX_LIGHTS, invLightStrengths);
	shader->setUniform3fv("lightColors", MAX_LIGHTS, lightColors);
}
void SceneManager::drawMesh(shared_ptr<GraphicsManager::View> view, shared_ptr<mesh> meshPtr, Mat4f transformation)
{
	if(!view || !meshPtr)
	{
	//	debugBreak();
		return;
	}

	auto model = shaders.bind("model");
	model->setUniform1i("tex",0);
	model->setUniform1i("specularMap",1);
	model->setUniform1i("normalMap",2);
	model->setUniform1i("numLights", 1);
	model->setUniform3f("eyePosition", view->camera().eye);
	model->setUniform3f("lightPosition", graphics->getLightPosition());
	model->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());

	meshPtr->VBO->bindBuffer();
	for(int pass = 0; pass <= 1; pass++)
	{
		for(auto material = meshPtr->materials.begin(); material != meshPtr->materials.end(); material++)
		{
			material->indexBuffer->bindBuffer();
			if((material->diffuse.a > 0.999 && pass == 0) || (material->diffuse.a <= 0.999 && pass == 1))
			{
				if(material->tex)			material->tex->bind(0);
				if(material->specularMap)	material->specularMap->bind(1);
				if(material->normalMap)		material->normalMap->bind(2);
				model->setUniform4f("diffuse", material->diffuse);
				model->setUniform3f("specular", material->specular);
				model->setUniform1f("hardness", material->hardness);
 
				model->setUniformMatrix("modelTransform", transformation);
				material->indexBuffer->drawBuffer();
			}
		}
		
		if(pass == 0) graphics->setDepthMask(false); // set to false after the first pass
		if(pass == 1) graphics->setDepthMask(true);  // then reset to true after the second
	}
}
void SceneManager::drawMesh(shared_ptr<GraphicsManager::View> view, shared_ptr<mesh> meshPtr, Mat4f transformation, shared_ptr<GraphicsManager::shader> shader)
{
	if(!view || !meshPtr)
	{
		debugBreak();
		return;
	}
	shader->bind();
	shader->setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());

	meshPtr->VBO->bindBuffer();
	for(auto material = meshPtr->materials.begin(); material != meshPtr->materials.end(); material++)
	{
		shader->setUniformMatrix("modelTransform", transformation);
		material->indexBuffer->bindBuffer();
		material->indexBuffer->drawBuffer();
	}
}
shared_ptr<SceneManager::pointLight> SceneManager::genPointLight()
{
	shared_ptr<pointLight> l(new pointLight);
	lights.push_back(l);
	return l;
}