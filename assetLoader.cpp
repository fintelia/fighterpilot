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
						tmpAssetFile->file = fileManager.loadTextureFile(filename,false);
						assetFilesPreload.push(shared_ptr<assetFile>(tmpAssetFile));
					}
					else
					{
						tmpAssetFile->file = fileManager.loadTextureFile(filename,true);
						assetFiles.push(shared_ptr<assetFile>(tmpAssetFile));
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
						tmpAssetFile->file = fileManager.loadTextureFile(filename,false);
						assetFilesPreload.push(shared_ptr<assetFile>(tmpAssetFile));
					}
					else
					{
						tmpAssetFile->file = fileManager.loadTextureFile(filename,true);
						assetFiles.push(shared_ptr<assetFile>(tmpAssetFile));
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
						tmpAssetFile->file = fileManager.loadTextureFile(filename,false);
						assetFilesPreload.push(shared_ptr<assetFile>(tmpAssetFile));
					}
					else
					{
						tmpAssetFile->file = fileManager.loadTextureFile(filename,true);
						assetFiles.push(shared_ptr<assetFile>(tmpAssetFile));
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
				tmpAssetFile->use_sAspect = getAttribute(shaderElement, "sAspect") == "true";

				if(tmpAssetFile->name == "" || vertFilename == "" || fragFilename == "")
				{
					debugBreak();
					continue;
				}

				if(getAttribute(shaderElement,"preload") == "true")
				{
					tmpAssetFile->vertFile = fileManager.loadTextFile(vertFilename,false);
					tmpAssetFile->fragFile = fileManager.loadTextFile(fragFilename,false);
					assetFilesPreload.push(shared_ptr<assetFile>(tmpAssetFile));
				}
				else
				{
					tmpAssetFile->vertFile = fileManager.loadTextFile(vertFilename,true);
					tmpAssetFile->fragFile = fileManager.loadTextFile(fragFilename,true);
					assetFiles.push(shared_ptr<assetFile>(tmpAssetFile));
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

				assetFiles.push(shared_ptr<assetFile>(tmpAssetFile));

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
					tmpAssetFile->file = fileManager.loadTextFile(filename,false);
					assetFilesPreload.push(shared_ptr<assetFile>(tmpAssetFile));
				}
				else
				{
					tmpAssetFile->file = fileManager.loadTextFile(filename,true);
					assetFiles.push(shared_ptr<assetFile>(tmpAssetFile));
				}

				assetElement = assetElement->NextSiblingElement();
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
	bool preload;
	auto pop = [this](bool p)
	{
		if(p)
			assetFilesPreload.pop();
		else assetFiles.pop();
	};

	do
	{
		////////////////////set file////////////////////////
		file = (preload = !assetFilesPreload.empty()) ? assetFilesPreload.front() : assetFiles.front();
		////////////////////////////////////////////////////
		if(file->getType() == assetFile::TEXTURE)
		{
			auto textureAsset = dynamic_pointer_cast<textureAssetFile>(file);
			if(!textureAsset->file->complete()) //if the file is not loaded yet break out of the loop
			{
				break;
			}
			else if(textureAsset->file->valid()) //if the file loaded properly, initialize the texture
			{
				auto texture = graphics->genTexture2D();
				texture->setData(textureAsset->file->width, textureAsset->file->height, ((GraphicsManager::texture::Format)textureAsset->file->channels), textureAsset->file->contents, textureAsset->tileable);
				dataManager.addTexture(textureAsset->name, texture);
			}
			else
			{
				debugBreak(); //file failed to load properly
				cout << textureAsset->file->filename << " failed to load properly" << endl;
			}
			pop(preload);
		}
		else if(file->getType() == assetFile::TEXTURE_3D)
		{
			auto textureAsset = dynamic_pointer_cast<texture3AssetFile>(file);
			if(!textureAsset->file->complete()) //if the file is not loaded yet break out of the loop
			{
				break;
			}
			else if(textureAsset->file->valid()) //if the file loaded properly, initialize the texture
			{
				auto texture = graphics->genTexture3D();
				texture->setData(textureAsset->file->width, textureAsset->file->height/textureAsset->depth, textureAsset->depth, ((GraphicsManager::texture::Format)textureAsset->file->channels), textureAsset->file->contents, textureAsset->tileable);
				dataManager.addTexture(textureAsset->name, texture);
			}
			else
			{
				debugBreak(); //file failed to load properly
				cout << textureAsset->file->filename << " failed to load properly" << endl;
			}
			pop(preload);
		}
		else if(file->getType() == assetFile::TEXTURE_CUBE)
		{
			auto textureAsset = dynamic_pointer_cast<textureCubeAssetFile>(file);
			if(!textureAsset->file->complete()) //if the file is not loaded yet break out of the loop
			{
				break;
			}
			else if(textureAsset->file->valid()) //if the file loaded properly, initialize the texture
			{
				auto texture = graphics->genTextureCube();
				texture->setData(textureAsset->file->width, textureAsset->file->height/6, ((GraphicsManager::texture::Format)textureAsset->file->channels), textureAsset->file->contents);
				dataManager.addTexture(textureAsset->name, texture);
			}
			else
			{
				debugBreak(); //file failed to load properly
				cout << textureAsset->file->filename << " failed to load properly" << endl;
			}
			pop(preload);
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

				if(!shader->init(shaderAsset->vertFile->contents.c_str(), shaderAsset->fragFile->contents.c_str()))
				{
	#ifdef _DEBUG
					messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
	#endif
				}

				if(shaderAsset->use_sAspect)
				{
					shader->setUniform1f("sAspect",sAspect);
					shaders.add(shaderAsset->name, shader, true);
					//dataManager.addShader(shaderAsset->name, shader, true);
				}
				else
				{
					shaders.add(shaderAsset->name, shader, false);
					//dataManager.addShader(shaderAsset->name, shader, false);
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
			pop(preload);
		}
		else if(file->getType() == assetFile::MODEL)
		{
			auto modelAsset = dynamic_pointer_cast<modelAssetFile>(file);
			dataManager.addModel(modelAsset->name, modelAsset->filename);
			pop(preload);

			break;
		}
		else if(file->getType() == assetFile::FONT)
		{
			auto fontAsset = dynamic_pointer_cast<fontAssetFile>(file);
			if(!fontAsset->file->complete()) break;

			dataManager.addFont(fontAsset->name, fontAsset->file);
			pop(preload);
		}
	}while(!assetFilesPreload.empty() && !assetFiles.empty());

	return assetFiles.size();
}
