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
    XMLElement* shadersElement = nullptr;
        
	if(node != nullptr && (shadersElement = node->ToElement()) != nullptr)
	{
        node = shadersElement->FirstChildElement();

        XMLElement* shaderElement = node ? node->ToElement() : nullptr;
        while(shaderElement != nullptr)
        {
            if(strcmp(shaderElement->Value(), "header") == 0)
            {
                shaderHeaderFile* tmpAssetFile = new shaderHeaderFile;
                string filename = getAttribute(shaderElement, "file");
                tmpAssetFile->name = getAttribute(shaderElement, "name");
                tmpAssetFile->header =
                    fileManager.loadFile<FileManager::textFile>(filename);

                assetFilesPreload.push_back(shared_ptr<assetFile>(tmpAssetFile));
            }
            else if(strcmp(shaderElement->Value(), "shader") == 0)
            {
                
                shaderAssetFile* tmpAssetFile = new shaderAssetFile;

                //load file names
                tmpAssetFile->name = getAttribute(shaderElement, "name");
                tmpAssetFile->use_sAspect =
                    getAttribute(shaderElement, "sAspect") == "true";

                ///transform feedback varyings
                auto& ftVec = tmpAssetFile->transformFeedbackVaryings;
                string feedbackTransformVaryings =
                    getAttribute(shaderElement, "feedbackTransformVaryings");
                boost::split(ftVec, feedbackTransformVaryings,
                             boost::is_any_of(";"));
                ftVec.erase(std::remove_if(ftVec.begin(), ftVec.end(),
                                           [](string s){return s.empty();}),
                            ftVec.end());
                
                //load files
                bool preload = (getAttribute(shaderElement,"preload") == "true");
                auto load_file = [&](shared_ptr<FileManager::textFile>& file,
                                     const char* attrib_name) {
                    string filename = getAttribute(shaderElement, attrib_name);
                    if(filename != "") {
                        file = fileManager.loadFile<FileManager::textFile>(
                            filename, preload);
                    }
                };

                load_file(tmpAssetFile->vertFile, "vertex");
                load_file(tmpAssetFile->fragFile, "fragment");
                load_file(tmpAssetFile->vert3File, "vertex3");
                load_file(tmpAssetFile->geom3File, "geometry3");
                load_file(tmpAssetFile->frag3File, "fragment3");
                load_file(tmpAssetFile->vert4File, "vertex4");
                load_file(tmpAssetFile->geom4File, "geometry4");
                load_file(tmpAssetFile->tessC4File, "tessellationControl4");
                load_file(tmpAssetFile->tessE4File, "tessellationEval4");
                load_file(tmpAssetFile->frag4File, "fragment4");

                auto asset_ptr = shared_ptr<assetFile>(tmpAssetFile);
                if(preload)
                    assetFilesPreload.push_back(asset_ptr);
                else
                    assetFiles.push_back(asset_ptr);
            }
            shaderElement = shaderElement->NextSiblingElement();
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
        else if(file->getType() == assetFile::SHADER_HEADER)
        {
            auto shaderHeader = dynamic_pointer_cast<shaderHeaderFile>(file);
            if(shaderHeader->header->complete()) {
                GraphicsManager::shader::loadHeader(shaderHeader->name,
                                                shaderHeader->header->contents);
                pop(isPreload);
            }
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

			if(shaderAsset->vertFile	&& !shaderAsset->vertFile->complete()){
                messageBox(shaderAsset->vertFile->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->fragFile	&& !shaderAsset->fragFile->complete()){
                messageBox(shaderAsset->fragFile->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->vert3File	&& !shaderAsset->vert3File->complete()){
                messageBox(shaderAsset->vert3File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->geom3File	&& !shaderAsset->geom3File->complete()){
                messageBox(shaderAsset->geom3File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->frag3File	&& !shaderAsset->frag3File->complete()){
                messageBox(shaderAsset->frag3File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->vert4File	&& !shaderAsset->vert4File->complete()){
                messageBox(shaderAsset->vert4File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->geom4File	&& !shaderAsset->geom4File->complete()){
                messageBox(shaderAsset->geom4File->filename + " failed to load properly.");		pop(isPreload); break; }
			if(shaderAsset->tessC4File	&& !shaderAsset->tessC4File->complete()){
                messageBox(shaderAsset->tessC4File->filename + " failed to load properly.");	pop(isPreload); break; }
			if(shaderAsset->tessE4File	&& !shaderAsset->tessE4File->complete()){
                messageBox(shaderAsset->tessE4File->filename + " failed to load properly.");	pop(isPreload); break; }
			if(shaderAsset->frag4File	&& !shaderAsset->frag4File->complete()){
                messageBox(shaderAsset->frag4File->filename + " failed to load properly.");		pop(isPreload); break; }


			auto shader = graphics->genShader();

			if(graphics->hasShaderModel4() && shaderAsset->vert3File && !shaderAsset->transformFeedbackVaryings.empty())
			{
                vector<const char*> transformFeedbackVaryings;
                for(auto& v : shaderAsset->transformFeedbackVaryings){
                    if(!v.empty())
                        transformFeedbackVaryings.push_back(v.c_str());
                }
                
				shader->init4(shaderAsset->vert3File->contents.c_str(),
                              shaderAsset->geom3File ? shaderAsset->geom3File->contents.c_str() : nullptr,
                              nullptr,
                              transformFeedbackVaryings);
			}
			else if(graphics->hasShaderModel5() && shaderAsset->vert4File /*&& shaderAsset->frag3File*/)
			{
				shader->init5(shaderAsset->vert4File->contents.c_str(),
                              shaderAsset->geom4File ? shaderAsset->geom4File->contents.c_str() : nullptr,
                              shaderAsset->tessC4File ? shaderAsset->tessC4File->contents.c_str() : nullptr,
                              shaderAsset->tessE4File ? shaderAsset->tessE4File->contents.c_str() : nullptr,
                              shaderAsset->frag4File ? shaderAsset->frag4File->contents.c_str() : nullptr);
			}
			else if(graphics->hasShaderModel4() && shaderAsset->vert3File /*&& shaderAsset->frag3File*/)
			{
				shader->init4(shaderAsset->vert3File->contents.c_str(),
                              shaderAsset->geom3File ? shaderAsset->geom3File->contents.c_str() : nullptr,
                              shaderAsset->frag3File ? shaderAsset->frag3File->contents.c_str() : nullptr);

			}
			else if(shaderAsset->vertFile && shaderAsset->fragFile)
			{
				shader->init(shaderAsset->vertFile->contents.c_str(), shaderAsset->fragFile->contents.c_str());
			}
			else
			{
				pop(isPreload);
				break;
			}

			if(shaderAsset->use_sAspect)
			{
				shader->bind();
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
