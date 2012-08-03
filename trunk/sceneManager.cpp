
#include "engine.h"

void SceneManager::meshInstance::update(const Vec3f& pos,bool render)
{
	position=pos;
	setRenderFlag(render);
}
void SceneManager::meshInstance::update(const Vec3f& pos, const Quat4f& rot,bool render)
{
	position=pos;
	rotation=rot;
	setRenderFlag(render);
}
void SceneManager::meshInstance::setRenderFlag(bool b)
{
	if(b)	flags |=  0x01;
	else	flags &= ~0x01;
}
void SceneManager::meshInstance::setDeleteFlag(bool b)
{
	if(b)	flags |=  0x02;
	else	flags &= ~0x02;
}
void SceneManager::meshInstance::setTemperaryFlag(bool b)
{
	if(b)	flags |=  0x04;
	else	flags &= ~0x04;
}
shared_ptr<SceneManager::meshInstance> SceneManager::newMeshInstance(string model, Vec3f position, Quat4f rotation)
{
	shared_ptr<meshInstance> ptr(new meshInstance(model,position,rotation));
	meshInstances[model].push_back(ptr);
	return ptr;
}
void SceneManager::newTemperaryMesh(string model, Vec3f position, Quat4f rotation)
{
	shared_ptr<meshInstance> ptr(new meshInstance(model,position,rotation));
	ptr->setTemperaryFlag(true);
	meshInstances[model].push_back(ptr);
}
void SceneManager::resetMeshInstances()
{
	meshInstances.clear();
}
void SceneManager::renderScene(shared_ptr<GraphicsManager::View> view, meshInstancePtr firstPersonObject)
{
	for(auto meshType=meshInstances.begin(); meshType!=meshInstances.end(); meshType++)
	{
		meshType->second.erase(std::remove_if(meshType->second.begin(), meshType->second.end(), [](shared_ptr<meshInstance> p)->bool{return p->deleteFlag();}), meshType->second.end());
	}

	dataManager.bind("model");
	dataManager.setUniform1i("tex",0);
	dataManager.setUniform1i("specularMap",1);
	dataManager.setUniform1i("normalMap",2);
	dataManager.setUniform3f("lightPosition", graphics->getLightPosition());
	dataManager.setUniformMatrix("cameraProjection",view->projectionMatrix() * view->modelViewMatrix());
	for(int pass = 0; pass <= 1; pass++)
	{
		for(auto meshType=meshInstances.begin(); meshType!=meshInstances.end(); meshType++)
		{
			if(meshType->second.empty())
				continue;

			auto modelPtr = dataManager.getModel(meshType->first);
			if(modelPtr == nullptr)
				continue;

			for(auto material = modelPtr->materials.begin(); material != modelPtr->materials.end(); material++)
			{
				if((material->diffuse.a > 0.999 && pass == 0) || (material->diffuse.a <= 0.999 && pass == 1))
				{
					dataManager.bind(material->tex == "" ? "white" : material->tex);
					dataManager.bind(material->specularMap == "" ? "white" : material->specularMap,1);
					dataManager.bind(material->normalMap == "" ? "default normals" : material->normalMap,2);
					dataManager.setUniform4f("diffuse", material->diffuse);
					dataManager.setUniform3f("specular", material->specular);
					dataManager.setUniform1f("hardness", material->hardness);
 
					for(auto instance = (*meshType).second.begin(); instance != (*meshType).second.end(); instance++)
					{
						if((*instance) != firstPersonObject && (*instance)->renderFlag() /*&& view->sphereInFrustum(modelPtr->boundingSphere + (*instance)->position)*/)
						{
							dataManager.setUniformMatrix("modelTransform", Mat4f((*instance)->rotation,(*instance)->position));
							material->indexBuffer->drawBuffer(GraphicsManager::TRIANGLES, modelPtr->VBO);
						}
					}
				}
			}
		}
		
		if(pass == 0) graphics->setDepthMask(false); // set to false after the first pass
		if(pass == 1) graphics->setDepthMask(true);  // then reset to true after the second
	}
}
void SceneManager::endRender()
{
	for(auto meshType=meshInstances.begin(); meshType!=meshInstances.end(); meshType++)
	{
		for(auto instance = (*meshType).second.begin(); instance != (*meshType).second.end(); instance++)
		{
			(*instance)->setRenderFlag(false);
			if((*instance)->temperaryFlag())
			{
				(*instance)->setDeleteFlag(true);
			}
		}
	}
}