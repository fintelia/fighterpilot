
#include "engine.h"
#include "xml/tinyxml.h"

string getAttribute(TiXmlElement* element, const char* attribute)
{
	const char* c = element->Attribute(attribute);
	string s = (c != NULL) ? string(c) : string("");
	return s;
}
int getIntAttribute(TiXmlElement* element, const char* attribute)//returns 0 on failure
{
	int i;
	return element->QueryIntAttribute(attribute, &i) == TIXML_SUCCESS ? i : 0;
}
bool AssetLoader::loadAssetList()
{
	TiXmlDocument doc("media/assetList.xml");
	if(!doc.LoadFile())
	{
		return false;
	}

	const char* c;
	TiXmlNode* node					= nullptr;
	TiXmlNode* assetsNode			= nullptr;

	assetsNode = doc.FirstChild("assets");
	if(assetsNode == nullptr) return false;

////////////////////////////////////////textures//////////////////////////////////////////
	node = assetsNode->FirstChild("textures");
	if(node != nullptr)
	{
		TiXmlElement* texturesElement	= nullptr;
		TiXmlElement* textureElement	= nullptr;

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

					if(tmpAssetFile->name == "" || filename == "" || textureElement->QueryIntAttribute("depth", &tmpAssetFile->depth) != TIXML_SUCCESS)
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
	node = assetsNode->FirstChild("shaders");
	if(node != nullptr)
	{
		TiXmlElement* shadersElement	= nullptr;
		TiXmlElement* shaderElement		= nullptr;

		shadersElement = node->ToElement();
		if(shadersElement != nullptr)
		{
			node = shadersElement->FirstChildElement();
			if(node != nullptr) shaderElement = node->ToElement();


			while(shaderElement != nullptr)
			{
				shaderAssetFile* tmpAssetFile = new shaderAssetFile;
				tmpAssetFile->name = getAttribute(shaderElement, "name");
				tmpAssetFile->positionComponents = getIntAttribute(shaderElement, "Position");
				tmpAssetFile->texCoordComponents = getIntAttribute(shaderElement, "TexCoord");
				tmpAssetFile->normalComponents = getIntAttribute(shaderElement, "Normal");
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
	node = assetsNode->FirstChild("models");
	if(node != nullptr)
	{
		TiXmlElement* modelsElement		= nullptr;
		TiXmlElement* modelElement		= nullptr;

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
	node = assetsNode->FirstChild("fonts");
	if(node != nullptr)
	{
		TiXmlElement* assetsElement		= nullptr;
		TiXmlElement* assetElement		= nullptr;

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

	bool loadedAsset=false;
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
			if(!textureAsset->file->complete()) break;

			auto texture = graphics->genTexture2D();
			texture->setData(textureAsset->file->width, textureAsset->file->height, ((GraphicsManager::texture::Format)textureAsset->file->channels), textureAsset->file->contents, textureAsset->tileable);
			dataManager.addTexture(textureAsset->name, texture);

			pop(preload);
		}
		else if(file->getType() == assetFile::TEXTURE_3D)
		{
			auto textureAsset = dynamic_pointer_cast<texture3AssetFile>(file);
			if(!textureAsset->file->complete()) break;
			auto texture = graphics->genTexture3D();
			texture->setData(textureAsset->file->width, textureAsset->file->height/textureAsset->depth, textureAsset->depth, ((GraphicsManager::texture::Format)textureAsset->file->channels), textureAsset->file->contents, textureAsset->tileable);
			dataManager.addTexture(textureAsset->name, texture);

			pop(preload);
		}
		else if(file->getType() == assetFile::SHADER)
		{
			auto shaderAsset = dynamic_pointer_cast<shaderAssetFile>(file);
			if(!shaderAsset->vertFile->complete() || !shaderAsset->fragFile->complete()) break;
			
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
				dataManager.addShader(shaderAsset->name, shader, true);
			}
			else
			{
				dataManager.addShader(shaderAsset->name, shader, false);
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