
#include "engine.h"
#include "xml/tinyxml.h"

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
				assetFile tmpAssetFile;
				tmpAssetFile.type = assetFile::TEXTURE;

				c = textureElement->Attribute("name");	tmpAssetFile.name = c!=NULL ? c : "";
				c = textureElement->Attribute("file");	tmpAssetFile.filename[0] = c!=NULL ? c : "";

				const char* tileable = textureElement->Attribute("tileable");
				if(tileable != nullptr && string(tileable) == "true")
					tmpAssetFile.options.insert("tileable");

				if(tmpAssetFile.name != "" && tmpAssetFile.filename[0] != "")
				{
					const char* preload = textureElement->Attribute("preload");
					if(preload == nullptr || string(preload) != "true")
					{
						tmpAssetFile.files.push_back(fileManager.loadTextureFile(tmpAssetFile.filename[0],true));
						assetFiles.push(tmpAssetFile);
					}
					else
					{
						tmpAssetFile.files.push_back(fileManager.loadTextureFile(tmpAssetFile.filename[0],false));
						assetFilesPreload.push(tmpAssetFile);
					}
				}
				else debugBreak();

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
				assetFile tmpAssetFile;
				tmpAssetFile.type = assetFile::SHADER;

				c = shaderElement->Attribute("name");		tmpAssetFile.name = c!=nullptr ? c : "";
				c = shaderElement->Attribute("vertex");		tmpAssetFile.filename[0] = c!=nullptr ? c : "";
				c = shaderElement->Attribute("fragment");	tmpAssetFile.filename[1] = c!=nullptr ? c : "";

				const char* use_sAspect = shaderElement->Attribute("sAspect");
				if(use_sAspect != nullptr && string(use_sAspect) == "true")
					tmpAssetFile.options.insert("use_sAspect");

				if(tmpAssetFile.name != "" && tmpAssetFile.filename[0] != "" && tmpAssetFile.filename[1] != "")
				{
					const char* preload = shaderElement->Attribute("preload");
					if(preload == nullptr || string(preload) != "true")
					{
						tmpAssetFile.files.push_back(fileManager.loadTextFile(tmpAssetFile.filename[0],true));
						tmpAssetFile.files.push_back(fileManager.loadTextFile(tmpAssetFile.filename[1],true));
						assetFiles.push(tmpAssetFile);
					}
					else
					{
						tmpAssetFile.files.push_back(fileManager.loadTextFile(tmpAssetFile.filename[0],false));
						tmpAssetFile.files.push_back(fileManager.loadTextFile(tmpAssetFile.filename[1],false));
						assetFilesPreload.push(tmpAssetFile);
					}
				}
				else debugBreak();
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
				assetFile tmpAssetFile;
				tmpAssetFile.type = assetFile::MODEL;

				c = modelElement->Attribute("name");	tmpAssetFile.name = c!=nullptr ? c : "";
				c = modelElement->Attribute("file");	tmpAssetFile.filename[0] = c!=nullptr ? c : "";

				if(tmpAssetFile.name !="" && tmpAssetFile.filename[0] != "")
					assetFiles.push(tmpAssetFile);
				else
					debugBreak();

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
				assetFile tmpAssetFile;
				tmpAssetFile.type = assetFile::FONT;

				c = assetElement->Attribute("name");	tmpAssetFile.name = c!=nullptr ? c : "";
				c = assetElement->Attribute("file");	tmpAssetFile.filename[0] = c!=nullptr ? c : "";

				if(tmpAssetFile.name !="" && tmpAssetFile.filename[0] != "")
				{
					const char* preload = assetElement->Attribute("preload");
					if(preload == nullptr || string(preload) != "true")
					{
						tmpAssetFile.files.push_back(fileManager.loadTextFile(tmpAssetFile.filename[0],true));
						assetFiles.push(tmpAssetFile);
					}
					else
					{
						tmpAssetFile.files.push_back(fileManager.loadTextFile(tmpAssetFile.filename[0],false));
						assetFilesPreload.push(tmpAssetFile);
					}
				}
				else debugBreak();

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
	assetFile file;
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
		if(file.type == assetFile::TEXTURE)
		{
			if(file.files.empty()) file.files.push_back(fileManager.loadTextureFile(file.filename[0]));
			if(!file.files.front()->complete()) break;

			bool t = file.options.count("tileable") != 0;
			//registerTexture(file.name, file.filename[0], t);

			shared_ptr<FileManager::textureFile> f = static_pointer_cast<FileManager::textureFile>(file.files.front());
			auto texture = graphics->genTexture2D();
			texture->setData(f->width, f->height, ((GraphicsManager::texture::Format)f->channels), f->contents, t);
			dataManager.addTexture(file.name, texture);

			//auto texPtr = registerTexture(dynamic_pointer_cast<FileManager::textureFile>(file.files.front()), t);
			//if(texPtr) assets[file.name] = shared_ptr<asset>(texPtr);
			pop(preload);
		}
		else if(file.type == assetFile::SHADER)
		{
			if(file.files.size() == 0) file.files.push_back(fileManager.loadTextureFile(file.filename[0]));
			if(file.files.size() == 1) file.files.push_back(fileManager.loadTextureFile(file.filename[0]));
			if(!file.files[0]->complete() || !file.files[1]->complete()) break;
			
			auto shader = graphics->genShader();
			if(!shader->init(dynamic_pointer_cast<FileManager::textFile>(file.files[0])->contents.c_str(), dynamic_pointer_cast<FileManager::textFile>(file.files[1])->contents.c_str()))
			{
#ifdef _DEBUG
				messageBox(string("error in shader ") + file.name + ":\n\n" + shader->getErrorStrings());
#endif
			}
			
			if(file.options.count("use_sAspect") != 0)
			{
				shader->setUniform1f("sAspect",sAspect);
				dataManager.addShader(file.name, shader, true);
			}
			else
			{
				dataManager.addShader(file.name, shader, false);
			}
			//auto shaderPtr = registerShader(dynamic_pointer_cast<FileManager::textFile>(file.files[0]), dynamic_pointer_cast<FileManager::textFile>(file.files[1]));
			//if(shaderPtr)
			//{
			//	assets[file.name] = shared_ptr<asset>(shaderPtr);
			//	if(shaderPtr && file.options.count("use_sAspect") != 0)
			//	{
			//		auto s = assets.find(file.name);
			//		static_pointer_cast<shaderAsset>(s->second)->use_sAspect = true;
			//
			//		bind(file.name);
			//		setUniform1f("sAspect",sAspect);
			//		unbind(file.name);
			//	}
			//}
			pop(preload);
			//break;
		}
		else if(file.type == assetFile::MODEL)
		{
			dataManager.addModel(file.name, file.filename[0]);
			//auto modelPtr = registerOBJ(file.filename[0]);
			//if(modelPtr) assets[file.name] = shared_ptr<asset>(modelPtr);
			pop(preload);
			break;
		}
		else if(file.type == assetFile::FONT)
		{
			if(file.files.empty()) file.files.push_back(fileManager.loadTextFile(file.filename[0]));
			if(!file.files.front()->complete()) break;

			dataManager.addFont(file.name, dynamic_pointer_cast<FileManager::textFile>(file.files.front()));
			//auto fontPtr = registerFont(dynamic_pointer_cast<FileManager::textFile>(file.files.front()));
			//if(fontPtr) assets[file.name] = shared_ptr<asset>(fontPtr);
			pop(preload);
			break;
		}
	}while(!assetFilesPreload.empty() && !assetFiles.empty());
	
	return assetFiles.size();
}