#include <iostream>

#include "engine.h"
//#include "xml/tinyxml.h"
#include "xml/tinyxml2.h"

using namespace tinyxml2;

bool AssetLoader::loadAssetList()
{
	auto  getAttribute = [](XMLElement* element, const char* attribute)->string
	{
		const char* c = element->Attribute(attribute);
		string s = (c != NULL) ? string(c) : string("");
		return s;
	};
	auto getIntAttribute = [](XMLElement* element, const char* attribute)->int //returns 0 on failure
	{
		int i;
		return element->QueryIntAttribute(attribute, &i) == XML_SUCCESS ? i : 0;
	};

	XMLDocument doc;
	if(doc.LoadFile("media/assetList.xml"))
	{
		return false;
	}

	XMLElement* node				= nullptr;
	XMLElement* assetsNode			= nullptr;

	assetsNode = doc.FirstChildElement("assets");
	if(assetsNode == nullptr) return false;
	
////////////////////////////////////////textures//////////////////////////////////////////
	node = assetsNode->FirstChildElement("textures");
	if(node != nullptr)
	{
		XMLElement* texturesElement	= nullptr;
		XMLElement* textureElement	= nullptr;

		texturesElement = node->ToElement();
		if(texturesElement != nullptr)
		{
			node = texturesElement->FirstChildElement();
			if(node != nullptr) textureElement = node->ToElement();



			while(textureElement != nullptr)
			{
				if(strcmp(textureElement->Value(), "texture") == 0)
				{
					textureAssetFile* tmpAssetFile = new textureAssetFile;
					tmpAssetFile->name = getAttribute(textureElement, "name");
					string filename = getAttribute(textureElement, "file");
					tmpAssetFile->tileable = getAttribute(textureElement,"tileable") == "true";

					if(game->isWideScreen())
					{
						string wideFileName = getAttribute(textureElement, "fileWide");
						if(wideFileName != "")
						{
							filename = wideFileName;
						}
					}


					if(tmpAssetFile->name == "" || filename == "")
					{
						debugBreak();
						continue;
					}

					if(getAttribute(textureElement,"preload") == "true")
					{
						//tmpAssetFile->file = fileManager.loadTextureFile(filename,false);
						tmpAssetFile->filename = filename;
						assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
					}
					else
					{
						//tmpAssetFile->file = fileManager.loadTextureFile(filename,true);
						tmpAssetFile->filename = filename;
						assetFiles.push_back(shared_ptr<assetFile>(tmpAssetFile));
					}

				}
				else if(strcmp(textureElement->Value(), "texture3D") == 0)
				{
					texture3AssetFile* tmpAssetFile = new texture3AssetFile;
					tmpAssetFile->name = getAttribute(textureElement, "name");
					string filename = getAttribute(textureElement, "file");
					tmpAssetFile->tileable = getAttribute(textureElement,"tileable") == "true";

					if(tmpAssetFile->name == "" || filename == "" || textureElement->QueryIntAttribute("depth", &tmpAssetFile->depth) != XML_SUCCESS)
					{
						debugBreak();
						continue;
					}

					if(getAttribute(textureElement,"preload") == "true")
					{
						//tmpAssetFile->file = fileManager.loadTextureFile(filename,false);
						tmpAssetFile->filename = filename;
						assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
					}
					else
					{
						//tmpAssetFile->file = fileManager.loadTextureFile(filename,true);
						tmpAssetFile->filename = filename;
						assetFiles.push_back(shared_ptr<assetFile>(tmpAssetFile));
					}
				}
				else if(strcmp(textureElement->Value(), "textureCube") == 0)
				{
					textureCubeAssetFile* tmpAssetFile = new textureCubeAssetFile;
					tmpAssetFile->name = getAttribute(textureElement, "name");
					string filename = getAttribute(textureElement, "file");

					if(tmpAssetFile->name == "" || filename == "")
					{
						debugBreak();
						continue;
					}

					if(getAttribute(textureElement,"preload") == "true")
					{
						//tmpAssetFile->file = fileManager.loadTextureFile(filename,false);
						tmpAssetFile->filename = filename;
						assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
					}
					else
					{
						//tmpAssetFile->file = fileManager.loadTextureFile(filename,true);
						tmpAssetFile->filename = filename;
						assetFiles.push_back(shared_ptr<assetFile>(tmpAssetFile));
					}
				}
				textureElement = textureElement->NextSiblingElement();
			}
		}
	}
////////////////////////////////////////shaders///////////////////////////////////////////
	node = assetsNode->FirstChildElement("shaders");
	if(node != nullptr)
	{
		XMLElement* shadersElement	= nullptr;
		XMLElement* shaderElement	= nullptr;

		shadersElement = node->ToElement();
		if(shadersElement != nullptr)
		{
			node = shadersElement->FirstChildElement();
			if(node != nullptr) shaderElement = node->ToElement();


			while(shaderElement != nullptr)
			{
				shaderAssetFile* tmpAssetFile = new shaderAssetFile;
				tmpAssetFile->name = getAttribute(shaderElement, "name");
				string vertFilename = getAttribute(shaderElement, "vertex");
				string fragFilename = getAttribute(shaderElement, "fragment");
				string vert3Filename = getAttribute(shaderElement, "vertex3");	
				string frag3Filename = getAttribute(shaderElement, "geometry3");
				string geom3Filename = getAttribute(shaderElement, "fragment3");

				tmpAssetFile->use_sAspect = getAttribute(shaderElement, "sAspect") == "true";

				if(tmpAssetFile->name == "" || vertFilename == "" || fragFilename == "")
				{
					debugBreak();
					continue;
				}

				if(getAttribute(shaderElement,"preload") == "true")
				{
					tmpAssetFile->vertFile = fileManager.loadFile<FileManager::textFile>(vertFilename);
					tmpAssetFile->fragFile = fileManager.loadFile<FileManager::textFile>(fragFilename);

					tmpAssetFile->vert3File = vert3Filename != "" ? fileManager.loadFile<FileManager::textFile>(vert3Filename) : nullptr;
					tmpAssetFile->geom3File = geom3Filename != "" ? fileManager.loadFile<FileManager::textFile>(geom3Filename) : nullptr;
					tmpAssetFile->frag3File = frag3Filename != "" ? fileManager.loadFile<FileManager::textFile>(frag3Filename) : nullptr;

					assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
				}
				else
				{
					tmpAssetFile->vertFile = fileManager.loadFile<FileManager::textFile>(vertFilename,true);
					tmpAssetFile->fragFile = fileManager.loadFile<FileManager::textFile>(fragFilename,true);

					tmpAssetFile->vert3File = vert3Filename != "" ? fileManager.loadFile<FileManager::textFile>(vert3Filename, true) : nullptr;
					tmpAssetFile->geom3File = geom3Filename != "" ? fileManager.loadFile<FileManager::textFile>(geom3Filename, true) : nullptr;
					tmpAssetFile->frag3File = frag3Filename != "" ? fileManager.loadFile<FileManager::textFile>(frag3Filename, true) : nullptr;

					assetFiles.push_back(shared_ptr<assetFile>(tmpAssetFile));
				}
				shaderElement = shaderElement->NextSiblingElement();
			}
		}
	}
////////////////////////////////////////models///////////////////////////////////////////
	node = assetsNode->FirstChildElement("models");
	if(node != nullptr)
	{
		XMLElement* modelsElement		= nullptr;
		XMLElement* modelElement		= nullptr;

		modelsElement = node->ToElement();
		if(modelsElement != nullptr)
		{
			node = modelsElement->FirstChildElement();
			if(node != nullptr) modelElement = node->ToElement();

			while(modelElement != nullptr)
			{
				modelAssetFile* tmpAssetFile = new modelAssetFile;

				tmpAssetFile->name = getAttribute(modelElement, "name");
				tmpAssetFile->filename = getAttribute(modelElement, "file");

				if(tmpAssetFile->name == "" || tmpAssetFile->filename == "")
				{
					debugBreak();
					continue;
				}

				assetFiles.push_back(shared_ptr<assetFile>(tmpAssetFile));

				modelElement = modelElement->NextSiblingElement();
			}
		}
	}
///////////////////////////////////////font//////////////////////////////////////////////
	node = assetsNode->FirstChildElement("fonts");
	if(node != nullptr)
	{
		XMLElement* assetsElement		= nullptr;
		XMLElement* assetElement		= nullptr;

		assetsElement = node->ToElement();
		if(assetsElement != nullptr)
		{
			node = assetsElement->FirstChildElement();
			if(node != nullptr) assetElement = node->ToElement();


			while(assetElement != nullptr)
			{
				fontAssetFile* tmpAssetFile = new fontAssetFile;
				tmpAssetFile->name = getAttribute(assetElement, "name");
				string filename = getAttribute(assetElement, "file");

				if(tmpAssetFile->name == "" || filename == "")
				{
					debugBreak();
					continue;
				}

				if(getAttribute(assetElement,"preload") == "true")
				{
					tmpAssetFile->file = fileManager.loadFile<FileManager::textFile>(filename,false);
					assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
				}
				else
				{
					tmpAssetFile->file = fileManager.loadFile<FileManager::textFile>(filename,true);
					assetFiles.push_back(shared_ptr<assetFile>(tmpAssetFile));
				}

				assetElement = assetElement->NextSiblingElement();
			}
		}
	}

	double t = GetTime();
	assetsZipFile = fileManager.loadFile<FileManager::zipFile>("media/assets.zip");//shared_ptr<FileManager::zipFile>(new FileManager::zipFile("media/assets.zip"));
	t = GetTime()-t;
	t = GetTime();
	for(auto i=assetsZipFile->files.begin(); i!=assetsZipFile->files.end(); i++)
	{
		if(i->second->type == FileManager::MODEL_FILE)
		{
			shared_ptr<FileManager::modelFile> model = dynamic_pointer_cast<FileManager::modelFile>(i->second);
			for(auto m=model->materials.begin(); m != model->materials.end(); m++)
			{
				if(m->textureMap_filename != "")
				{
					textureAssetFile* tmpTextureFile = new textureAssetFile;
					tmpTextureFile->name = tmpTextureFile->filename = m->textureMap_filename;
					assetFiles.push_back(shared_ptr<assetFile>(tmpTextureFile));
				}
				if(m->specularMap_filename != "")
				{
					textureAssetFile* tmpTextureFile = new textureAssetFile;
					tmpTextureFile->name = tmpTextureFile->filename = m->specularMap_filename;
					assetFiles.push_back(shared_ptr<assetFile>(tmpTextureFile));
				}
				if(m->normalMap_filename != "")
				{
					textureAssetFile* tmpTextureFile = new textureAssetFile;
					tmpTextureFile->name = tmpTextureFile->filename = m->normalMap_filename;
					assetFiles.push_back(shared_ptr<assetFile>(tmpTextureFile));
				}
			}
		}
	}
	return true;
}
int AssetLoader::loadAsset()
{
	if(assetFilesPreload.empty() && assetFiles.empty())
		return 0;

	shared_ptr<assetFile> file;
	bool isPreload;
	auto pop = [this](bool p)
	{
		if(p)
			assetFilesPreload.pop_front();
		else
			assetFiles.pop_front();
	};

	do// while(!assetFilesPreload.empty() || !assetFiles.empty())
	{
		////////////////////set file////////////////////////
		file = (isPreload = !assetFilesPreload.empty()) ? assetFilesPreload.front() : assetFiles.front();
		////////////////////////////////////////////////////
		if(file->getType() == assetFile::TEXTURE)
		{
			auto textureAsset = dynamic_pointer_cast<textureAssetFile>(file);
			shared_ptr<FileManager::textureFile> texFile = loadTexture(textureAsset->filename);
			if(texFile && texFile->valid())
			{
				auto texture = graphics->genTexture2D();
				texture->setData(texFile->width, texFile->height, ((GraphicsManager::texture::Format)texFile->channels), texFile->contents, textureAsset->tileable);
				dataManager.addTexture(textureAsset->name, texture);
			}
			else
			{
				debugBreak(); //file failed to load properly
				cout << textureAsset->filename << " failed to load properly" << endl;
			}
			pop(isPreload);
		}
		else if(file->getType() == assetFile::TEXTURE_3D)
		{
			auto textureAsset = dynamic_pointer_cast<texture3AssetFile>(file);
			shared_ptr<FileManager::textureFile> texFile = loadTexture(textureAsset->filename);
			if(texFile && texFile->valid())
			{
				auto texture = graphics->genTexture3D();
				texture->setData(texFile->width, texFile->height/textureAsset->depth, textureAsset->depth, ((GraphicsManager::texture::Format)texFile->channels), texFile->contents, textureAsset->tileable);
				dataManager.addTexture(textureAsset->name, texture);
			}
			else
			{
				debugBreak(); //file failed to load properly
				cout << textureAsset->filename << " failed to load properly" << endl;
			}
			pop(isPreload);
		}
		else if(file->getType() == assetFile::TEXTURE_CUBE)
		{
			auto textureAsset = dynamic_pointer_cast<textureCubeAssetFile>(file);
			shared_ptr<FileManager::textureFile> texFile = loadTexture(textureAsset->filename);
			if(texFile && texFile->valid())
			{
				auto texture = graphics->genTextureCube();
				texture->setData(texFile->width, texFile->height/6, ((GraphicsManager::texture::Format)texFile->channels), texFile->contents);
				dataManager.addTexture(textureAsset->name, texture);
			}
			else
			{
				debugBreak(); //file failed to load properly
				cout << textureAsset->filename << " failed to load properly" << endl;
			}
			pop(isPreload);
		}
		else if(file->getType() == assetFile::SHADER)
		{
			auto shaderAsset = dynamic_pointer_cast<shaderAssetFile>(file);
			if(!shaderAsset->vertFile->complete() || !shaderAsset->fragFile->complete())
			{
				break;
			}
			else if(shaderAsset->vertFile->valid() && shaderAsset->fragFile->valid())
			{
				auto shader = graphics->genShader();

				if(graphics->hasShaderModel4() && shaderAsset->vert3File && shaderAsset->frag3File)
				{
					if(!shader->init4(shaderAsset->vert3File->contents.c_str(), shaderAsset->geom3File->contents.c_str(), shaderAsset->frag3File->contents.c_str()))
					{
#ifdef _DEBUG
						messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
#endif
					}
				}
				else
				{
					if(!shader->init(shaderAsset->vertFile->contents.c_str(), shaderAsset->fragFile->contents.c_str()))
					{
#ifdef _DEBUG
						messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
#endif
					}
				}


				if(shaderAsset->use_sAspect)
				{
					shader->setUniform1f("sAspect",sAspect);
					shaders.add(shaderAsset->name, shader, true);
				}
				else
				{
					shaders.add(shaderAsset->name, shader, false);
				}
			}
			else
			{
				debugBreak();
				if(!shaderAsset->vertFile->valid())
					cout << shaderAsset->vertFile->filename << " failed to load properly" << endl;
				if(!shaderAsset->fragFile->valid())
					cout << shaderAsset->fragFile->filename << " failed to load properly" << endl;				
			}
			pop(isPreload);
		}
		else if(file->getType() == assetFile::MODEL)
		{
			pop(isPreload);	//must pop assetFile from deque before we load it, so its textures can be safely pushed on the front
			auto modelAsset = dynamic_pointer_cast<modelAssetFile>(file);

			loadedModel l;
			l.model = loadModel(modelAsset->filename, true);
			l.name = modelAsset->name;

			if(l.model->valid())
			{
				modelsToRegister.push_back(l);
			}
			//	dataManager.addModel(modelAsset->name, model);
			debugAssert(l.model->valid());
		}
		else if(file->getType() == assetFile::COLLISION_MESH)
		{

			auto modelAsset = dynamic_pointer_cast<collisionMeshAssetFile>(file);
			shared_ptr<FileManager::modelFile> collisionMesh = loadModel(modelAsset->filename, false);
			if(collisionMesh->valid())
			{
				if(!collisionMesh->materials.empty())
				{
					vector<Vec3f> vertices;
					for(auto v=collisionMesh->vertices.begin(); v!=collisionMesh->vertices.end(); v++)
						vertices.push_back(v->position);
					collisionManager.setCollsionBounds(modelAsset->objType, collisionMesh->boundingSphere, vertices, collisionMesh->materials[0].indices);
				}
			}
			debugAssert(collisionMesh->valid());
			pop(isPreload);
		}
		else if(file->getType() == assetFile::FONT)
		{
			auto fontAsset = dynamic_pointer_cast<fontAssetFile>(file);
			if(!fontAsset->file->complete()) break;

			dataManager.addFont(fontAsset->name, fontAsset->file);
			pop(isPreload);
		}
	}while(!assetFilesPreload.empty());

	if(assetFilesPreload.empty() && assetFiles.empty())
	{
		for(auto i = modelsToRegister.begin(); i != modelsToRegister.end(); i++)
		{
			dataManager.addModel(i->name, i->model);
		}
		modelsToRegister.clear();
	}

	return assetFiles.size();
}
shared_ptr<FileManager::modelFile> AssetLoader::loadModel(string filename, bool loadTextures)
{
	//attempt to load model from assets.zip
	if(assetsZipFile->files.find(filename+".mesh") != assetsZipFile->files.end())
	{
		shared_ptr<FileManager::file> filePtr = assetsZipFile->files[filename+".mesh"];
		if(filePtr->type == FileManager::MODEL_FILE)
		{
			return dynamic_pointer_cast<FileManager::modelFile>(filePtr);
		}
	}

	//if loading from assets.zip failed, load from disk instead
	shared_ptr<FileManager::modelFile> model = fileManager.loadFile<FileManager::modelFile>(filename);
	if(model->valid())
	{
		//add model to ip file for next time
		modelsToAddToZip.push_back(model);

		if(loadTextures)
		{
			textureAssetFile* tmpTextureFile = new textureAssetFile;
			for(auto m=model->materials.begin(); m != model->materials.end(); m++)
			{
				if(m->textureMap_filename != "")
				{
					textureAssetFile* tmpTextureFile = new textureAssetFile;
					tmpTextureFile->name = tmpTextureFile->filename = m->textureMap_filename;
					assetFiles.push_front(shared_ptr<assetFile>(tmpTextureFile));
				}
				if(m->specularMap_filename != "")
				{
					textureAssetFile* tmpTextureFile = new textureAssetFile;
					tmpTextureFile->name = tmpTextureFile->filename = m->specularMap_filename;
					assetFiles.push_front(shared_ptr<assetFile>(tmpTextureFile));
				}
				if(m->normalMap_filename != "")
				{
					textureAssetFile* tmpTextureFile = new textureAssetFile;
					tmpTextureFile->name = tmpTextureFile->filename = m->normalMap_filename;
					assetFiles.push_front(shared_ptr<assetFile>(tmpTextureFile));
				}
			}
		}
	}
	return model;
}
shared_ptr<FileManager::textureFile> AssetLoader::loadTexture(string filename)
{
	//attempt to load texture from assets.zip
	if(assetsZipFile->files.find(filename+".tga") != assetsZipFile->files.end())
	{
		shared_ptr<FileManager::file> filePtr = assetsZipFile->files[filename+".tga"];
		if(filePtr->type == FileManager::TEXTURE_FILE)
		{
			return dynamic_pointer_cast<FileManager::textureFile>(filePtr);
		}
	}

	//if loading from assets.zip failed, load from disk instead
	shared_ptr<FileManager::textureFile> texture = fileManager.loadFile<FileManager::textureFile>(filename);
	texturesToAddToZip.push_back(texture);
	return texture;
}
void AssetLoader::addModel(string filename, string name)
{
	shared_ptr<modelAssetFile> asset(new modelAssetFile);
	asset->name = name;
	asset->filename = filename;
	assetFiles.push_back(asset);
}
void AssetLoader::addCollisionMesh(string filename, objectType objType)
{
	shared_ptr<collisionMeshAssetFile> asset(new collisionMeshAssetFile);
	asset->filename = filename;
	asset->objType = objType;
	assetFiles.push_back(asset);
}
void AssetLoader::saveAssetZip()
{
#ifndef _DEBUG
	bool zipChanged = false;
	for(auto i=modelsToAddToZip.begin(); i != modelsToAddToZip.end(); i++)
	{
		if((*i)->valid())
		{
			(*i)->filename += ".mesh";
			(*i)->format = FileManager::MESH;
			assetsZipFile->files[(*i)->filename] = static_pointer_cast<FileManager::file>(*i);
			zipChanged = true;
		}
	}
	for(auto i=texturesToAddToZip.begin(); i != texturesToAddToZip.end(); i++)
	{
		if((*i)->valid())
		{
			(*i)->filename += ".tga";
			(*i)->format = FileManager::TGA;
			assetsZipFile->files[(*i)->filename] = static_pointer_cast<FileManager::file>(*i);
			zipChanged = true;
		}
	}
	if(zipChanged)
	{
		fileManager.writeFile(assetsZipFile);
	}
#endif
}