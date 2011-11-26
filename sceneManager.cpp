
#include "engine.h"
#include "GL/glee.h"
#include <GL/glu.h>

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
std::shared_ptr<SceneManager::meshInstance> SceneManager::newMeshInstance(string model, Vec3f position, Quat4f rotation)
{
	std::shared_ptr<meshInstance> ptr(new meshInstance(model,position,rotation));
	meshInstances[model].push_back(ptr);
	return ptr;
}
void SceneManager::newTemperaryMesh(string model, Vec3f position, Quat4f rotation)
{
	std::shared_ptr<meshInstance> ptr(new meshInstance(model,position,rotation));
	ptr->setTemperaryFlag(true);
	meshInstances[model].push_back(ptr);
}
void SceneManager::resetMeshInstances()
{
	meshInstances.clear();
}
void SceneManager::renderScene(meshInstancePtr firstPersonObject)
{
	for(auto meshType=meshInstances.begin(); meshType!=meshInstances.end(); meshType++)
	{
		meshType->second.erase(remove_if(meshType->second.begin(), meshType->second.end(), [](std::shared_ptr<meshInstance> p)->bool{return p->deleteFlag();}), meshType->second.end());
	}

	dataManager.bind("model");
	dataManager.setUniform1i("tex",0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	Angle ang;

	Mat4f cameraProjectionMat = graphics->getView().projectionMat * graphics->getView().modelViewMat;
	dataManager.setUniformMatrix("cameraProjection",cameraProjectionMat);

	for(int pass = 0; pass <= 1; pass++)
	{
		for(auto meshType=meshInstances.begin(); meshType!=meshInstances.end(); meshType++)
		{
			if(meshType->second.empty())
				continue;

			auto modelPtr = dataManager.getModel(meshType->first);
			if(modelPtr == nullptr)
				continue;

			glBindBuffer(GL_ARRAY_BUFFER, modelPtr->id);
			glVertexPointer(3,	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)0);
			glNormalPointer(	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(3*sizeof(float)));
			glTexCoordPointer(2,GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(6*sizeof(float)));

			for(auto material = modelPtr->materials.begin(); material != modelPtr->materials.end(); material++)
			{
				dataManager.bind(material->tex == "" ? "white" : material->tex);
				glColor4f(material->color.r,material->color.g,material->color.b, material->color.a);

				if((material->color.a > 0.999 && pass == 0) || (material->color.a <= 0.999 && pass == 1))
				{
					for(auto instance = (*meshType).second.begin(); instance != (*meshType).second.end(); instance++)
					{
						if((*instance) == firstPersonObject)
							continue;

						if((*instance)->renderFlag() && graphics->sphereInFrustum(modelPtr->boundingSphere + (*instance)->position))
						{
							dataManager.setUniformMatrix("modelTransform", Mat4f((*instance)->rotation,(*instance)->position));
							glDrawArrays(GL_TRIANGLES, material->indicesOffset, material->numIndices);

						}
						(*instance)->setRenderFlag(false);
						if((*instance)->temperaryFlag())
						{
							(*instance)->setDeleteFlag(true);
						}
					}
				}

			}
		}
		if(pass == 0) glDepthMask(false); // set to false after the first pass
		if(pass == 1) glDepthMask(true);  // then reset to true after the second
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	dataManager.unbind("model");
	dataManager.unbindTextures();
	glColor3f(1,1,1);
}
