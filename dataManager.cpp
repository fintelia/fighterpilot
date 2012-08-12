
#include "engine.h"
#include "png/png.h"

DataManager::~DataManager()
{
	shutdown();
}
DataManager::fontAsset* DataManager::registerFont(shared_ptr<FileManager::textFile> f) //loads a "text" .fnt file as created by Bitmap Font Generator from http://www.angelcode.com/products/bmfont/
{
	if(f->invalid())
		return nullptr;

	struct info_t
	{
		int lineHeight;
		int base;
		int width;
		int height;
	} info;
	struct fontChar
	{
		int		id,
				x, y,
				w, h,
				xOffset,
				yOffset,
				xAdvance;
		fontChar():id(0), x(0), y(0), w(0), h(0), xOffset(0), yOffset(0), xAdvance(0){}
	};

	string s;
	std::stringstream fin(f->contents);

	getline(fin,s);
	getline(fin,s);
	sscanf(s.c_str(),"common lineHeight=%d base=%d scaleW=%d scaleH=%d", &info.lineHeight, &info.base, &info.width, &info.height);

	getline(fin,s);
	char texturePath[256];
	sscanf(s.c_str(),"page id=0 file=\"%s", texturePath);
	string texPath(texturePath);
	boost::trim_right_if(texPath,boost::is_any_of(" \"\t\n"));

	int dLoc=f->filename.find_last_of("/\\");
	if(dLoc!=string::npos)	texPath = f->filename.substr(0,dLoc+1) + texPath;


	getline(fin,s);

	int numChars=0;
	sscanf(s.c_str(),"chars count=%d", &numChars);

	fontChar* fontChars = new fontChar[numChars];

	int i=0;
	while(!fin.eof() && i < numChars)
	{
		getline(fin, s);

		s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());

		sscanf(s.c_str(),"char id=%dx=%dy=%dwidth=%dheight=%dxoffset=%dyoffset=%dxadvance=%d", &fontChars[i].id, &fontChars[i].x, &fontChars[i].y, &fontChars[i].w, &fontChars[i].h, &fontChars[i].xOffset, &fontChars[i].yOffset, &fontChars[i].xAdvance);

		i++;
	}
	if(i < numChars) numChars = i;



	shared_ptr<FileManager::textureFile> texFile = fileManager.loadTextureFile(texPath);
	if(!texFile->valid())
	{
		delete [] fontChars;
		return nullptr;
	}

	fontAsset* fAsset = new fontAsset;
	fAsset->texture = graphics->genTexture2D();
	fAsset->texture->setData(texFile->width, texFile->height, (GraphicsManager::texture::Format)texFile->channels, texFile->contents);
	fAsset->type = asset::FONT;
	fAsset->height = (float)info.lineHeight;

	float invW = 1.0 / texFile->width;
	float invH = 1.0 / texFile->height;

	fontAsset::character tmpChar;
	for(auto i = 0; i < numChars; i++)
	{
		tmpChar.UV = Rect::XYWH(invW * fontChars[i].x, invH * fontChars[i].y, invW * fontChars[i].w, invH * fontChars[i].h);
		tmpChar.width = fontChars[i].w;
		tmpChar.height = fontChars[i].h;
		tmpChar.xAdvance = fontChars[i].xAdvance;
		tmpChar.xOffset = fontChars[i].xOffset;
		tmpChar.yOffset = fontChars[i].yOffset;
		fAsset->characters[fontChars[i].id] = tmpChar;
	}

	delete [] fontChars;
	return fAsset;
}
DataManager::textureAsset* DataManager::registerTexture(shared_ptr<FileManager::textureFile> f, bool tileable)
{
	if(!f || !f->valid())
		return nullptr;

	GraphicsManager::texture::Format format;
	if(f->channels == 1)		format = GraphicsManager::texture::LUMINANCE;
	else if(f->channels == 2)	format = GraphicsManager::texture::LUMINANCE_ALPHA;
	else if(f->channels == 3)	format = GraphicsManager::texture::RGB;
	else if(f->channels == 4)	format = GraphicsManager::texture::RGBA;
	else{
		debugBreak();
		return nullptr;
	}

	shared_ptr<GraphicsManager::texture2D> tex = graphics->genTexture2D();
	tex->setData(f->width, f->height, format, f->contents, tileable);

	textureAsset* a = new textureAsset;
	a->texture = tex;
	a->type = asset::TEXTURE;
	a->width = f->width;
	a->height = f->height;
	a->bpp = f->channels * 8;
	a->data = NULL;
	return a;
}
//DataManager::shaderAsset* DataManager::registerShader(shared_ptr<FileManager::textFile> vert, shared_ptr<FileManager::textFile> frag, bool use_sAspect)
//{
//	shaderAsset* a = new shaderAsset;
//	a->shader = graphics->genShader();
//	a->shader->init(vert->contents.c_str(), frag->contents.c_str());
//	a->type = asset::SHADER;
//	a->use_sAspect = use_sAspect;
//	return a;
//}
void DataManager::addTexture(string name, shared_ptr<GraphicsManager::texture> tex)
{
	if(tex)
	{
		textureAsset* a = new textureAsset;
		a->texture = tex;
		a->type = asset::TEXTURE;
		a->width = 0;//tex->width;
		a->height = 0;//f->height;
		a->bpp = 0;//f->channels * 8;
		a->data = NULL;

		assets[name] = shared_ptr<asset>(a);
	}
}
//void DataManager::addShader(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect)
//{
//	if(shader)
//	{
//		shaderAsset* a = new shaderAsset;
//		a->shader = shader;
//		a->type = asset::SHADER;
//		a->use_sAspect = use_sAspect;
//
//		assets[name] = shared_ptr<asset>(a);
//	}
//}
void DataManager::addModel(string name, string OBJfile)
{
	modelAsset* modelPtr = new modelAsset;
	modelPtr->type = asset::MODEL;
	modelPtr->mesh = sceneManager.createMesh(fileManager.loadObjFile(OBJfile));
	assets[name] = shared_ptr<asset>(modelPtr);

	//auto modelPtr = registerOBJ(OBJfile);
	//if(modelPtr)
	//{
	//	assets[name] = shared_ptr<asset>(modelPtr);
	//	//objectType t = objectTypeFromString(name);
	//	//if(t != 0)
	//	//{
	//	//	physics.setCollsionBounds(t, modelPtr->boundingSphere);
	//	//}
	//}
}
void DataManager::addFont(string name, shared_ptr<FileManager::textFile> f)
{
	auto fontPtr = registerFont(f);
	if(fontPtr) assets[name] = shared_ptr<asset>(fontPtr);
}
//DataManager::modelAsset* DataManager::registerOBJ(string filename)
//{
//	modelAsset* a = new modelAsset;
//	auto modelFile = fileManager.loadObjFile(filename);
//
//	a->type = asset::MODEL;
//	a->boundingSphere = modelFile->boundingSphere;
//	a->VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
//	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
//	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::NORMAL,		3*sizeof(float));
//	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TANGENT,		6*sizeof(float));
//	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		9*sizeof(float));
//	a->VBO->setTotalVertexSize(sizeof(normalMappedVertex3D));
//	a->VBO->setVertexData(sizeof(normalMappedVertex3D)*modelFile->vertices.size(), !modelFile->vertices.empty() ? &modelFile->vertices[0] : NULL);
//
//	modelAsset::material mat;
//	for(auto i = modelFile->materials.begin(); i != modelFile->materials.end(); i++)
//	{
//		mat.diffuse = i->diffuse;
//		mat.specular = i->specular;
//		mat.hardness = i->hardness;
//
//		auto texPtr = registerTexture(i->tex, true);
//		if(texPtr)
//		{
//			assets[i->tex->filename] = shared_ptr<asset>(texPtr);
//			mat.tex = i->tex->filename;
//		}
//
//		auto normalPtr = registerTexture(i->normalMap, true);
//		if(normalPtr)
//		{
//			assets[i->normalMap->filename] = shared_ptr<asset>(normalPtr);
//			mat.normalMap = i->normalMap->filename;
//		}
//
//		auto specularPtr = registerTexture(i->specularMap, true);
//		if(specularPtr)
//		{
//			assets[i->specularMap->filename] = shared_ptr<asset>(specularPtr);
//			mat.specularMap = i->specularMap->filename;
//		}
//
//		mat.indexBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);
//		mat.indexBuffer->setData(!i->indices.empty() ? &i->indices[0] : nullptr, i->indices.size());
//		a->materials.push_back(mat);
//	}
//	
//	return a;
//}
//void DataManager::writeErrorLog(string filename)
//{
//	shared_ptr<FileManager::textFile> file(new FileManager::textFile(filename));
//
//	string error;
//	for(auto i=assets.begin(); i!=assets.end(); i++)
//	{
//		if(i->second->type == asset::SHADER)
//		{
//			error = static_pointer_cast<shaderAsset>(i->second)->shader->getErrorStrings();
//			if(error != "")
//			{
//				file->contents += "[" + i->first + "]\n";
//				file->contents += error + "\n";
//			}
//		}
//	}
//	fileManager.writeTextFile(file, true);
//}
void DataManager::bind(string name, int textureUnit)
{
	if(assets.find(name)==assets.end())
		return;
	else if(assets[name]->type==asset::TEXTURE)
	{
		static_pointer_cast<textureAsset>(assets[name])->texture->bind(textureUnit);
	}
	else
	{
		debugBreak();
	}
	//else if(assets[name]->type==asset::SHADER)
	//{
	//	if(boundShader == name)
	//		return;

	//	static_pointer_cast<shaderAsset>(assets[name])->shader->bind();
	//	boundShader = name;
	//}
}
shared_ptr<const DataManager::fontAsset> DataManager::getFont(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::FONT)
		return nullptr;

	return static_pointer_cast<const fontAsset>(i->second);
}
//shared_ptr<const DataManager::modelAsset> DataManager::getModel(string name)
//{
//	auto i = assets.find(name);
//	if(i == assets.end() || i->second->type != asset::MODEL)
//		return nullptr;
//
//	return static_pointer_cast<const modelAsset>(i->second);
//}
shared_ptr<SceneManager::mesh> DataManager::getModel(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::MODEL)
		return nullptr;

	return static_pointer_cast<modelAsset>(i->second)->mesh;
}
shared_ptr<GraphicsManager::texture> DataManager::getTexture(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::TEXTURE)
		return nullptr;

	return static_pointer_cast<textureAsset>(i->second)->texture;
}
bool DataManager::assetLoaded(string name)
{
	return assets.find(name) != assets.end();
}

//void DataManager::setUniform1f(string name, float v0)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform1f(name, v0);
//	}
//}
//void DataManager::setUniform2f(string name, float v0, float v1)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform2f(name, v0, v1);
//	}
//}
//void DataManager::setUniform3f(string name, float v0, float v1, float v2)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform3f(name, v0, v1, v2);
//	}
//}
//void DataManager::setUniform4f(string name, float v0, float v1, float v2, float v3)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform4f(name, v0, v1, v2, v3);
//	}
//}
//void DataManager::setUniform1i(string name, int v0)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform1i(name, v0);
//	}
//}
//void DataManager::setUniform2i(string name, int v0, int v1)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform2i(name, v0, v1);
//	}
//}
//void DataManager::setUniform3i(string name, int v0, int v1, int v2)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform3i(name, v0, v1, v2);
//	}
//}
//void DataManager::setUniform4i(string name, int v0, int v1, int v2, int v3)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform4i(name, v0, v1, v2, v3);
//	}
//}
//void DataManager::setUniformMatrix(string name, const Mat3f& m)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniformMatrix(name, m);
//	}
//}
//void DataManager::setUniformMatrix(string name, const Mat4f& m)
//{
//	if(boundShader != "")
//	{
//		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniformMatrix(name, m);
//	}
//}

void DataManager::shutdown()
{
	boundShader = "";
	assets.clear();
}

shared_ptr<GraphicsManager::shader> ShaderManager::bind(string name)
{
	auto s = shaderAssets.find(name);
	if(s != shaderAssets.end())
	{
		s->second->shader->bind();
		return s->second->shader;
	}
	return nullptr;
}
shared_ptr<GraphicsManager::shader> ShaderManager::operator() (string name)
{
	auto s = shaderAssets.find(name);
	if(s != shaderAssets.end())
		return s->second->shader;
	return nullptr;
}
void ShaderManager::add(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect)
{
	if(shader)
	{
		shared_ptr<shaderAsset> a(new shaderAsset);
		a->shader = shader;
		a->use_sAspect = use_sAspect;
		shaderAssets[name] = a;
	}
}
void ShaderManager::writeErrorLog(string filename)
{
	shared_ptr<FileManager::textFile> file(new FileManager::textFile(filename));

	string error;
	for(auto i=shaderAssets.begin(); i!=shaderAssets.end(); i++)
	{
		error = i->second->shader->getErrorStrings();
		if(error != "")
		{
			file->contents += "[" + i->first + "]\n";
			file->contents += error + "\n";
		}
	}
	fileManager.writeTextFile(file, true);
}
void ShaderManager::shutdown()
{
	shaderAssets.clear();
}