#include <iostream>

#include "engine.h"
//#include "xml/tinyxml.h"
#include "xml/tinyxml2.h"

using namespace tinyxml2; //makes the code much more readable...

void AssetLoader::setTextureCompression(bool compression)
{
	useTextureCompression = compression;
}

bool AssetLoader::loadAssetList()
{
	auto  getAttribute = [](XMLElement* element, const char* attribute)->string
	{
		const char* c = element->Attribute(attribute);
		string s = (c != NULL) ? string(c) : string("");
		return s;
	};
// 	auto getIntAttribute = [](XMLElement* element, const char* attribute)->int //returns 0 on failure
// 	{
// 		int i;
// 		return element->QueryIntAttribute(attribute, &i) == XML_SUCCESS ? i : 0;
// 	};

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
					tmpAssetFile->compress = useTextureCompression;
					
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

				//load file names
				tmpAssetFile->name = getAttribute(shaderElement, "name");
				string vertFilename = getAttribute(shaderElement, "vertex");
				string fragFilename = getAttribute(shaderElement, "fragment");
				string vert3Filename = getAttribute(shaderElement, "vertex3");	
				string geom3Filename = getAttribute(shaderElement, "geometry3");
				string frag3Filename = getAttribute(shaderElement, "fragment3");

				string vert4Filename = getAttribute(shaderElement, "vertex4");	
				string geom4Filename = getAttribute(shaderElement, "geometry4");
				string frag4Filename = getAttribute(shaderElement, "fragment4");
				string tessC4Filename = getAttribute(shaderElement, "tessellationControl4");
				string tessE4Filename = getAttribute(shaderElement, "tessellationEval4");
				//sAspect
				tmpAssetFile->use_sAspect = getAttribute(shaderElement, "sAspect") == "true";


				///transform feedback varyings
				tmpAssetFile->feedbackTransformVaryingsStr = getAttribute(shaderElement, "feedbackTransformVaryings");
				int pos = tmpAssetFile->feedbackTransformVaryingsStr.find_first_not_of(';');
				if(pos != tmpAssetFile->feedbackTransformVaryingsStr.npos)
				{
					vector<int> positions;
					positions.push_back(0);
					while((pos = tmpAssetFile->feedbackTransformVaryingsStr.find(';', positions.back())) != tmpAssetFile->feedbackTransformVaryingsStr.npos)
					{
						if(tmpAssetFile->feedbackTransformVaryingsStr.size() > pos+1)
							positions.push_back(pos+1);
						tmpAssetFile->feedbackTransformVaryingsStr[pos] = '\0';
					}

					const char* c_str = tmpAssetFile->feedbackTransformVaryingsStr.c_str();
					for(auto i = positions.begin(); i != positions.end(); i++)
					{
						tmpAssetFile->feedbackTransformVaryings.push_back(c_str + *i);
					}
				}

				//check for possible errors
		//		if(tmpAssetFile->name == "" || (vertFilename == "" && vert3Filename == "") || (fragFilename == "" && frag3Filename == "" && tmpAssetFile->feedbackTransformVaryings.empty()))
		//		{
		//			debugBreak();
		//			continue;
		//		}

				//load files
				if(getAttribute(shaderElement,"preload") == "true")
				{
					tmpAssetFile->vertFile = vertFilename != "" ? fileManager.loadFile<FileManager::textFile>(vertFilename) : nullptr;
					tmpAssetFile->fragFile = fragFilename != "" ? fileManager.loadFile<FileManager::textFile>(fragFilename) : nullptr;

					tmpAssetFile->vert3File = vert3Filename != "" ? fileManager.loadFile<FileManager::textFile>(vert3Filename) : nullptr;
					tmpAssetFile->geom3File = geom3Filename != "" ? fileManager.loadFile<FileManager::textFile>(geom3Filename) : nullptr;
					tmpAssetFile->frag3File = frag3Filename != "" ? fileManager.loadFile<FileManager::textFile>(frag3Filename) : nullptr;

					tmpAssetFile->vert4File = vert4Filename != "" ? fileManager.loadFile<FileManager::textFile>(vert4Filename) : nullptr;
					tmpAssetFile->geom4File = geom4Filename != "" ? fileManager.loadFile<FileManager::textFile>(geom4Filename) : nullptr;
					tmpAssetFile->frag4File = frag4Filename != "" ? fileManager.loadFile<FileManager::textFile>(frag4Filename) : nullptr;
					tmpAssetFile->tessC4File = tessC4Filename != "" ? fileManager.loadFile<FileManager::textFile>(tessC4Filename) : nullptr;
					tmpAssetFile->tessE4File = tessE4Filename != "" ? fileManager.loadFile<FileManager::textFile>(tessE4Filename) : nullptr;

					assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
				}
				else
				{
					tmpAssetFile->vertFile = vertFilename != "" ? fileManager.loadFile<FileManager::textFile>(vertFilename,true) : nullptr;
					tmpAssetFile->fragFile = fragFilename != "" ? fileManager.loadFile<FileManager::textFile>(fragFilename,true) : nullptr;

					tmpAssetFile->vert3File = vert3Filename != "" ? fileManager.loadFile<FileManager::textFile>(vert3Filename, true) : nullptr;
					tmpAssetFile->geom3File = geom3Filename != "" ? fileManager.loadFile<FileManager::textFile>(geom3Filename, true) : nullptr;
					tmpAssetFile->frag3File = frag3Filename != "" ? fileManager.loadFile<FileManager::textFile>(frag3Filename, true) : nullptr;

					tmpAssetFile->vert4File = vert4Filename != "" ? fileManager.loadFile<FileManager::textFile>(vert4Filename) : nullptr;
					tmpAssetFile->geom4File = geom4Filename != "" ? fileManager.loadFile<FileManager::textFile>(geom4Filename) : nullptr;
					tmpAssetFile->frag4File = frag4Filename != "" ? fileManager.loadFile<FileManager::textFile>(frag4Filename) : nullptr;
					tmpAssetFile->tessC4File = tessC4Filename != "" ? fileManager.loadFile<FileManager::textFile>(tessC4Filename) : nullptr;
					tmpAssetFile->tessE4File = tessE4Filename != "" ? fileManager.loadFile<FileManager::textFile>(tessE4Filename) : nullptr;

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
//        cout << file->name << endl;
		////////////////////////////////////////////////////
		if(file->getType() == assetFile::TEXTURE)
		{
			auto textureAsset = dynamic_pointer_cast<textureAssetFile>(file);
			shared_ptr<FileManager::textureFile> texFile = loadTexture(textureAsset->filename);
			if(texFile && texFile->valid())
			{
				auto texture = graphics->genTexture2D();
				texture->setData(texFile->width, texFile->height, ((GraphicsManager::texture::Format)texFile->channels), textureAsset->tileable, textureAsset->compress, texFile->contents);
				
		//		if(shaders.shaderExists("blur mipmaps"))
		//			graphics->generateCustomMipmaps(texture, shaders("blur mipmaps"));
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

			if(	(shaderAsset->vertFile		&& !shaderAsset->vertFile->complete()) || 
				(shaderAsset->fragFile		&& !shaderAsset->fragFile->complete()) ||
				(shaderAsset->vert3File		&& !shaderAsset->vert3File->complete()) ||
				(shaderAsset->geom3File		&& !shaderAsset->geom3File->complete()) ||
				(shaderAsset->frag3File		&& !shaderAsset->frag3File->complete()) ||
				(shaderAsset->vert4File		&& !shaderAsset->vert4File->complete()) ||
				(shaderAsset->geom4File		&& !shaderAsset->geom4File->complete()) ||
				(shaderAsset->tessC4File	&& !shaderAsset->tessC4File->complete()) ||
				(shaderAsset->tessE4File	&& !shaderAsset->tessE4File->complete()) ||
				(shaderAsset->frag4File		&& !shaderAsset->frag4File->complete()))
			{
				break; //wait for all shader files to finish loading
			}

			if(shaderAsset->vertFile	&& !shaderAsset->vertFile->complete()){		messageBox(shaderAsset->vertFile->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->fragFile	&& !shaderAsset->fragFile->complete()){		messageBox(shaderAsset->fragFile->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->vert3File	&& !shaderAsset->vert3File->complete()){	messageBox(shaderAsset->vert3File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->geom3File	&& !shaderAsset->geom3File->complete()){	messageBox(shaderAsset->geom3File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->frag3File	&& !shaderAsset->frag3File->complete()){	messageBox(shaderAsset->frag3File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->vert4File	&& !shaderAsset->vert4File->complete()){	messageBox(shaderAsset->vert4File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->geom4File	&& !shaderAsset->geom4File->complete()){	messageBox(shaderAsset->geom4File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->tessC4File	&& !shaderAsset->tessC4File->complete()){	messageBox(shaderAsset->tessC4File->filename + " failed to load properly.");	pop(isPreload); break; }
			if(shaderAsset->tessE4File	&& !shaderAsset->tessE4File->complete()){	messageBox(shaderAsset->tessE4File->filename + " failed to load properly.");	pop(isPreload); break; }
			if(shaderAsset->frag4File	&& !shaderAsset->frag4File->complete()){	messageBox(shaderAsset->frag4File->filename + " failed to load properly.");		pop(isPreload); break; }


			auto shader = graphics->genShader();

			if(graphics->hasShaderModel4() && shaderAsset->vert3File && !shaderAsset->feedbackTransformVaryings.empty())
			{
				if(!shader->init4(shaderAsset->vert3File->contents.c_str(), shaderAsset->geom3File ? shaderAsset->geom3File->contents.c_str() : nullptr, nullptr, shaderAsset->feedbackTransformVaryings))
				{
#ifdef _DEBUG
//					messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
#endif
				}
			}
			else if(graphics->hasShaderModel5() && shaderAsset->vert4File /*&& shaderAsset->frag3File*/)
			{
				if(!shader->init5(shaderAsset->vert4File->contents.c_str(), shaderAsset->geom4File ? shaderAsset->geom4File->contents.c_str() : nullptr,  shaderAsset->tessC4File ? shaderAsset->tessC4File->contents.c_str() : nullptr,  shaderAsset->tessE4File ? shaderAsset->tessE4File->contents.c_str() : nullptr, shaderAsset->frag4File ? shaderAsset->frag4File->contents.c_str() : nullptr))
				{
#ifdef _DEBUG
//					messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
#endif
				}
			}
			else if(graphics->hasShaderModel4() && shaderAsset->vert3File /*&& shaderAsset->frag3File*/)
			{
				if(!shader->init4(shaderAsset->vert3File->contents.c_str(), shaderAsset->geom3File ? shaderAsset->geom3File->contents.c_str() : nullptr, shaderAsset->frag3File ? shaderAsset->frag3File->contents.c_str() : nullptr))
				{
#ifdef _DEBUG
//					messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
#endif
				}
			}
			else if(shaderAsset->vertFile && shaderAsset->fragFile)
			{
				if(!shader->init(shaderAsset->vertFile->contents.c_str(), shaderAsset->fragFile->contents.c_str()))
				{
#ifdef _DEBUG
//					messageBox(string("error in shader ") + shaderAsset->name + ":\n\n" + shader->getErrorStrings());
#endif
				}
			}
			else
			{
				pop(isPreload);
				break;
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
			auto collisionMeshAsset = dynamic_pointer_cast<collisionMeshAssetFile>(file);
			auto collisionMeshFile = loadModel(collisionMeshAsset->filename, false);

			dataManager.addCollisionBounds(collisionMeshAsset->filename, collisionMeshFile);

			debugAssert(collisionMeshFile->valid());
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

	//Add loaded models to dataManager (we couldn't do this earlier since their textures may not have been loaded)
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
		//add model to zip file for next time
		modelsToAddToZip.push_back(model);

		if(loadTextures)
		{
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
