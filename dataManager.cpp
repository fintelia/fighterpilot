
#include "engine.h"
#include "png/png.h"
#include "xml/tinyxml.h"
DataManager::~DataManager()
{
	shutdown();
}
DataManager::asset* DataManager::registerFont(shared_ptr<FileManager::textFile> f) //loads a "text" .fnt file as created by Bitmap Font Generator from http://www.angelcode.com/products/bmfont/
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
	stringstream fin(f->contents);

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
DataManager::asset* DataManager::registerTexture(shared_ptr<FileManager::textureFile> f, bool tileable)
{
	if(!f->valid())
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
DataManager::asset* DataManager::registerShader(shared_ptr<FileManager::textFile> vert, shared_ptr<FileManager::textFile> frag, bool use_sAspect)
{
	shaderAsset* a = new shaderAsset;
	a->shader = graphics->genShader();
	a->shader->init(vert->contents.c_str(), frag->contents.c_str());
	a->type = asset::SHADER;
	a->use_sAspect = use_sAspect;
	return a;
}
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
void DataManager::addShader(string name, shared_ptr<GraphicsManager::shader> shader, bool use_sAspect)
{
	if(shader)
	{
		shaderAsset* a = new shaderAsset;
		a->shader = shader;
		a->type = asset::SHADER;
		a->use_sAspect = use_sAspect;

		assets[name] = shared_ptr<asset>(a);
	}
}
void DataManager::addModel(string name, string OBJfile)
{
	auto modelPtr = registerOBJ(OBJfile);
	if(modelPtr) assets[name] = shared_ptr<asset>(modelPtr);
}
void DataManager::addFont(string name, shared_ptr<FileManager::textFile> f)
{
	auto fontPtr = registerFont(f);
	if(fontPtr) assets[name] = shared_ptr<asset>(fontPtr);
}
DataManager::asset* DataManager::registerOBJ(string filename)
{
	///////////////////////types////////////////////////////
//	struct color{float r,g,b;color(float red, float green, float blue): r(red), g(green), b(blue){}};
//	struct texCoord{float u,v;texCoord(float U, float V): u(U), v(V) {}texCoord(): u(0), v(0) {}};
	struct face{unsigned int v[3];unsigned int t[3];unsigned int n[3];unsigned int material;unsigned int combinedVertices[3];
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3, unsigned int n1, unsigned int n2, unsigned int n3) {	v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=n1;n[1]=n2;n[2]=n3;}
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=0;n[1]=0;n[2]=0;}
		face(unsigned int v1, unsigned int v2, unsigned int v3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}
		face() {v[0]=0;v[1]=0;v[2]=0;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}};
	struct triangle
	{
		Vec3f v1;
		Vec2f t1;
		Vec3f n1;

		Vec3f v2;
		Vec2f t2;
		Vec3f n2;

		Vec3f v3;
		Vec2f t3;
		Vec3f n3;
	};
	struct mtl
	{
		string tex;
		string specularMap;
		string normalMap;
		string name;
		Color4 diffuse;
		Color3 specular;
		float hardness;
	};
	struct vertexIndices
	{
		unsigned int position;
		unsigned int texCoord;
		unsigned int normal;
		bool operator< (const vertexIndices& v)const{return v.position != position ? v.position < position : (v.texCoord != texCoord ? v.texCoord < texCoord : v.normal < normal);}
		vertexIndices(unsigned int p, unsigned int t, unsigned int n): position(p), texCoord(t), normal(n){}
	};
	struct combinedFace
	{
		unsigned int vertices[3];
		unsigned int material;
	};
////////////////////variables///////////////////////////
	unsigned int	numVertices=0,
					numTexcoords=0,
					numNormals=0,
					numFaces=0,
					numMtls=0;

	Vec3f*			vertices;
	Vec3f*			normals;
	Vec2f*			texCoords;
	face*			faces;
	mtl*			mtls;

	unsigned int	totalVerts,totalFaces;

	vector<normalMappedVertex3D> combinedVertices;
	map<vertexIndices, unsigned int> indexMap;

	FILE *fp;
	if((fp=fopen(filename.c_str(), "r")) == nullptr)
		return nullptr;

	char buffer[256];
	char *token;
	///////
	map<string,mtl>	mtl_map;
	string file(filename);
	int i=file.find_last_of("/");
	if(i==string::npos)
		file.assign("");
	else
		file=file.substr(0,i+1);
	string mtlFile;
	//////
	while(fgets(buffer, 256, fp) != NULL)
	{
		token = strtok(buffer, " ");
		if(strcmp(token, "v") == 0) 	numVertices++;
		if(strcmp(token, "vt") == 0) 	numTexcoords++;
		if(strcmp(token, "f") == 0) 	numFaces++;
		if(strcmp(token, "vn") == 0)	numNormals++;
		if(strcmp(token, "mtllib") == 0)mtlFile=file + strtok(NULL, "\r\n");
	}
	rewind(fp);
	//fclose(fp);

	if(mtlFile.size()!=0)
	{
/////////////////////////new mtl////////////////////////////
		FILE *mtlFilePtr;
		if((mtlFilePtr=fopen(mtlFile.c_str(), "r")) != nullptr)
		{
			enum State{SEARCHING_FOR_NEWMTL, READING_MTL}state=SEARCHING_FOR_NEWMTL;
			mtl mMtl;

			while(fgets(buffer, 256, mtlFilePtr) != NULL)
			{
				token = strtok(buffer, " \t");
				if(strcmp(token, "newmtl") == 0)
				{
					if(state == READING_MTL)
					{
						mtl_map[mMtl.name] = mMtl;
					}
					mMtl.tex = "";
					mMtl.name = strtok(NULL, " #\r\n");
					mMtl.diffuse = white;
					mMtl.specular = black;
					mMtl.hardness = 40.0;
					state = READING_MTL;
				}
				else if(strcmp(token, "map_Kd") == 0 && state == READING_MTL)
				{
					mMtl.tex = file + strtok(NULL, "#\r\n");
					auto texPtr = registerTexture(fileManager.loadTextureFile(mMtl.tex), true);
					if(texPtr)
					{
						assets[mMtl.tex] = shared_ptr<asset>(texPtr);
					}
					else
					{
						mMtl.tex = "";
					}
				}
				else if(strcmp(token, "map_Ns") == 0 && state == READING_MTL)
				{
					mMtl.specularMap = file + strtok(NULL, "#\r\n");
					auto texPtr = registerTexture(fileManager.loadTextureFile(mMtl.specularMap), true);
					if(texPtr)
					{
						assets[mMtl.specularMap] = shared_ptr<asset>(texPtr);
					}
					else
					{
						mMtl.specularMap = "";
					}
				}
				else if(strcmp(token, "map_bump") == 0 && state == READING_MTL)
				{
					mMtl.normalMap = file + strtok(NULL, "#\r\n");
					auto texPtr = registerTexture(fileManager.loadTextureFile(mMtl.normalMap), true);
					if(texPtr)
					{
						assets[mMtl.normalMap] = shared_ptr<asset>(texPtr);
					}
					else
					{
						mMtl.normalMap = "";
					}
				}
				else if(strcmp(token, "Kd") == 0 && state == READING_MTL)
				{
					float r,g,b;
					if(sscanf(strtok(NULL, "#\r\n"), "%f%f%f", &r,&g,&b) == 3)
					{
						mMtl.diffuse = Color4(r,g,b,mMtl.diffuse.a);
					}
				}
				else if(strcmp(token, "Ks") == 0 && state == READING_MTL)
				{
					float r,g,b;
					if(sscanf(strtok(NULL, "#\r\n"), "%f%f%f", &r,&g,&b) == 3)
					{
						mMtl.specular = Color3(r,g,b);
					}
				}
				else if(strcmp(token, "Ns") == 0 && state == READING_MTL)
				{
					sscanf(strtok(NULL, "#\r\n"), "%f", &mMtl.hardness);
				}
				else if(strcmp(token, "d") == 0 && state == READING_MTL)
				{
					sscanf(strtok(NULL, "#\r\n"), "%f", &mMtl.diffuse.a);
				}
				else if(strcmp(token, "Tr") == 0 && state == READING_MTL)
				{
					float Tr;
					if(sscanf(strtok(NULL, "#\r\n"), "%f", &Tr))
					{
						mMtl.diffuse.a = 1.0 - Tr;
					}
				}
			}

			if(state == READING_MTL)
			{
				mtl_map[mMtl.name] = mMtl;
			}
			fclose(mtlFilePtr);
		}
////////////////////////new mtl end////////////////////////////
	}

	vertices	= new Vec3f[numVertices];
	texCoords	= new Vec2f[numTexcoords];
	faces		= new face[numFaces];
	normals		= new Vec3f[numNormals];
	mtls		= new mtl[mtl_map.size()];

	for(map<string,mtl>::iterator itt=mtl_map.begin();itt!=mtl_map.end();itt++)
		mtls[numMtls++]=itt->second;

	totalVerts = numVertices;
	totalFaces = numFaces;

	numVertices=0;
	numTexcoords=0;
	numNormals=0;
	numFaces=0;

	int cMtl=-1;
	while(fgets(buffer, 256, fp) != NULL)
	{
		token = strtok(buffer, " \t");
		if(strcmp(token, "v") == 0)
		{
			sscanf(strtok(NULL, " "), "%f", &vertices[numVertices].x);
			sscanf(strtok(NULL, " "), "%f", &vertices[numVertices].y);
			sscanf(strtok(NULL, " "), "%f", &vertices[numVertices].z);
			vertices[numVertices].x = -vertices[numVertices].x;
			numVertices++;
		}
		else if(strcmp(token, "vt") == 0)
		{
			sscanf(strtok(NULL, " "), "%f", &texCoords[numTexcoords].x);
			sscanf(strtok(NULL, " "), "%f", &texCoords[numTexcoords].y);

			texCoords[numTexcoords].y = 1.0f - texCoords[numTexcoords].y;
			numTexcoords++;
		}
		else if(strcmp(token, "vn") == 0)
		{
			sscanf(strtok(NULL, " "), "%f", &normals[numNormals].x);
			sscanf(strtok(NULL, " "), "%f", &normals[numNormals].y);
			sscanf(strtok(NULL, " "), "%f", &normals[numNormals].z);
			normals[numNormals] = normals[numNormals].normalize();
			numNormals++;
		}
		else if(strcmp(token, "f") == 0)
		{
			int i, v = 0, t = 0, n = 0;

			for(i = 0; i<3; i++) //should check for faces that do not include both a vertex, normal and position
			{
				token = strtok(NULL, " \t");
				sscanf(token, "%d/%d/%d", &v, &t, &n);

				faces[numFaces].n[i] = n;
				faces[numFaces].t[i] = t;
				faces[numFaces].v[i] = v;
			}
			faces[numFaces].material=cMtl;
			numFaces++;
		}
		else if(strcmp(token, "usemtl") == 0)
		{
			string name=strtok(NULL, " \r");
			if(name.size()!=0)
			{
				name=name.substr(0,name.size()-1);
				for(int l=0; l<numMtls; l++)
				{
					if(mtls[l].name.compare(name)==0)
					{
						cMtl=l;
						break;
					}
				}
			}
		}
	}
	fclose(fp);

////////////////////////////////////////////////bounding sphere///////////////////////////////////////////////////
	Vec3f center;
	float minx=0, maxx=0, miny=0, maxy=0, minz=0, maxz=0, radiusSquared=0;
	if(numVertices >= 1)
	{
		minx = maxx = vertices[0].x;
		miny = maxy = vertices[0].y;
		minz = maxz = vertices[0].z;
		for(i=1;i<numVertices;i++)
		{
			if(vertices[i].x<minx) minx=vertices[i].x;
			if(vertices[i].y<miny) miny=vertices[i].y;
			if(vertices[i].z<minz) minz=vertices[i].z;
			if(vertices[i].x>maxx) maxx=vertices[i].x;
			if(vertices[i].y>maxy) maxy=vertices[i].y;
			if(vertices[i].z>maxz) maxz=vertices[i].z;
		}
		center = Vec3f((minx+maxx)/2,((miny+maxy)/2),(minz+maxz)/2);
		radiusSquared = center.distanceSquared(vertices[0]);
		for(i=1;i<numVertices;i++)
		{
			if(center.distanceSquared(vertices[i]) > radiusSquared)
			{
				radiusSquared = center.distanceSquared(vertices[i]);
			}
		}
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned int* fs = new unsigned int[totalFaces*3];
	for(int itt = 0;itt<totalFaces;itt++)
	{
		fs[itt*3+0] = faces[itt].v[0]-1;
		fs[itt*3+1] = faces[itt].v[1]-1;
		fs[itt*3+2] = faces[itt].v[2]-1;
	}
	modelAsset* a = new modelAsset;
	a->type = asset::MODEL;
	a->trl = std::shared_ptr<CollisionChecker::triangleList>(new CollisionChecker::triangleList(vertices,fs,totalVerts,totalFaces));
	a->boundingSphere = Sphere<float>(center, sqrt(radiusSquared));
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	map<vertexIndices, unsigned int>::iterator vertexIndicesItt;
	float inv;
	normalMappedVertex3D tmpVertex;
	Vec3f faceNormal(0,1,0), avgFaceNormal, faceTangent(0,0,1);
	for(int i=0; i < totalFaces; i++)
	{
		if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0)
		{
			faceNormal = (vertices[faces[i].v[1]-1]-vertices[faces[i].v[0]-1]).cross(vertices[faces[i].v[2]-1]-vertices[faces[i].v[0]-1]);


		}
		if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0 && faces[i].t[0] != 0 && faces[i].t[1] != 0 && faces[i].t[2] != 0)
		{
			inv = (texCoords[faces[i].t[0]-1].x * texCoords[faces[i].t[1]-1].y - texCoords[faces[i].t[1]-1].x * texCoords[faces[i].t[0]-1].y);
			faceTangent = abs(inv) < 0.001 ? Vec3f(1,0,0).cross(faceNormal) : (vertices[faces[i].v[0]-1] * texCoords[faces[i].t[1]-1].y - vertices[faces[i].v[1]-1] * texCoords[faces[i].t[0]-1].y) / inv;
		}

		if(faces[i].v[0] != 0 && faces[i].v[1] != 0 && faces[i].v[2] != 0 && faces[i].n[0] != 0 && faces[i].n[1] != 0 && faces[i].n[2] != 0)
		{
			avgFaceNormal = normals[faces[i].n[0]-1] + normals[faces[i].n[1]-1] + normals[faces[i].n[2]-1];
			if(faceNormal.dot(faceNormal) < 0.0)// correct triangle winding order
			{
				swap(faces[i].v[0], faces[i].v[1]);
				swap(faces[i].t[0], faces[i].t[1]);
				swap(faces[i].n[0], faces[i].n[1]);
			}
		}
		for(int j = 0; j < 3; j++)
		{
			vertexIndicesItt = indexMap.find(vertexIndices(faces[i].v[j], faces[i].t[j], faces[i].n[j]));
			if(vertexIndicesItt == indexMap.end())
			{
				indexMap[vertexIndices(faces[i].v[j], faces[i].t[j], faces[i].n[j])] = combinedVertices.size();
				faces[i].combinedVertices[j] = combinedVertices.size();

				tmpVertex.position = (faces[i].v[j] != 0) ? vertices[faces[i].v[j]-1] : Vec3f();
				tmpVertex.normal = (faces[i].n[j] != 0) ? normals[faces[i].n[j]-1] : faceNormal;
				tmpVertex.UV = (faces[i].t[j] != 0) ? texCoords[faces[i].t[j]-1] : Vec2f();
				tmpVertex.tangent = Vec3f(0,0,0);
				combinedVertices.push_back(tmpVertex);
			}
			else
			{
				faces[i].combinedVertices[j] = vertexIndicesItt->second;
			}
		}
		combinedVertices[faces[i].combinedVertices[0]].tangent += faceTangent;
		combinedVertices[faces[i].combinedVertices[1]].tangent += faceTangent;
		combinedVertices[faces[i].combinedVertices[2]].tangent += faceTangent;
	}


	//unsigned int* indexBuffer = new unsigned int[totalFaces*3];
	//unsigned int lNum=0, vNum = 0;
	for(int m=0; m<numMtls; m++)
	{
		modelAsset::material mat;
		mat.diffuse = mtls[m].diffuse;
		mat.specular = mtls[m].specular;
		mat.hardness = mtls[m].hardness;
		mat.specularMap = mtls[m].specularMap;
		mat.normalMap = mtls[m].normalMap;
		mat.tex = mtls[m].tex;
		mat.indexBuffer = graphics->genIndexBuffer(GraphicsManager::indexBuffer::STATIC);

		if(totalFaces*3 <= USHRT_MAX)
		{
			vector<unsigned short> indexBuffer;
			for(int i=0; i < totalFaces; i++)
			{
				if(faces[i].material == m)
				{
					indexBuffer.push_back(faces[i].combinedVertices[0]);
					indexBuffer.push_back(faces[i].combinedVertices[1]);
					indexBuffer.push_back(faces[i].combinedVertices[2]);
				}
			}
			mat.indexBuffer->setData(!indexBuffer.empty() ? &indexBuffer[0] : nullptr, indexBuffer.size());
		}
		else
		{
			vector<unsigned int> indexBuffer;
			for(int i=0; i < totalFaces; i++)
			{
				if(faces[i].material == m)
				{
					indexBuffer.push_back(faces[i].combinedVertices[0]);
					indexBuffer.push_back(faces[i].combinedVertices[1]);
					indexBuffer.push_back(faces[i].combinedVertices[2]);
				}
			}
			mat.indexBuffer->setData(!indexBuffer.empty() ? &indexBuffer[0] : nullptr, indexBuffer.size());
		}
		a->materials.push_back(mat);
	}
	a->VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::POSITION3,	0);
	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::NORMAL,		3*sizeof(float));
	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TANGENT,		6*sizeof(float));
	a->VBO->addVertexAttribute(GraphicsManager::vertexBuffer::TEXCOORD,		9*sizeof(float));
	a->VBO->setTotalVertexSize(sizeof(normalMappedVertex3D));
	a->VBO->setVertexData(sizeof(normalMappedVertex3D)*combinedVertices.size(), !combinedVertices.empty() ? &combinedVertices[0] : NULL);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	delete[] fs;
	delete[] vertices;
	delete[] texCoords;
	delete[] faces;
	delete[] normals;
	delete[] mtls;
	return a;
}
void DataManager::writeErrorLog(string filename)
{
	shared_ptr<FileManager::textFile> file(new FileManager::textFile(filename));

	string error;
	for(auto i=assets.begin(); i!=assets.end(); i++)
	{
		if(i->second->type == asset::SHADER)
		{
			error = static_pointer_cast<shaderAsset>(i->second)->shader->getErrorStrings();
			if(error != "")
			{
				file->contents += "[" + i->first + "]\n";
				file->contents += error + "\n";
			}
		}
	}
	fileManager.writeTextFile(file, true);
}
void DataManager::bind(string name, int textureUnit)
{
	if(assets.find(name)==assets.end())
		return;
	else if(assets[name]->type==asset::TEXTURE)
	{
		static_pointer_cast<textureAsset>(assets[name])->texture->bind(textureUnit);
	}
	else if(assets[name]->type==asset::SHADER)
	{
		if(boundShader == name)
			return;

		static_pointer_cast<shaderAsset>(assets[name])->shader->bind();
		boundShader = name;
	}
}
shared_ptr<const DataManager::fontAsset> DataManager::getFont(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::FONT)
		return nullptr;

	return static_pointer_cast<const fontAsset>(i->second);
}
shared_ptr<const DataManager::modelAsset> DataManager::getModel(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::MODEL)
		return nullptr;

	return static_pointer_cast<const modelAsset>(i->second);
}
bool DataManager::assetLoaded(string name)
{
	return assets.find(name) != assets.end();
}

void DataManager::setUniform1f(string name, float v0)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform1f(name, v0);
	}
}
void DataManager::setUniform2f(string name, float v0, float v1)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform2f(name, v0, v1);
	}
}
void DataManager::setUniform3f(string name, float v0, float v1, float v2)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform3f(name, v0, v1, v2);
	}
}
void DataManager::setUniform4f(string name, float v0, float v1, float v2, float v3)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform4f(name, v0, v1, v2, v3);
	}
}
void DataManager::setUniform1i(string name, int v0)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform1i(name, v0);
	}
}
void DataManager::setUniform2i(string name, int v0, int v1)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform2i(name, v0, v1);
	}
}
void DataManager::setUniform3i(string name, int v0, int v1, int v2)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform3i(name, v0, v1, v2);
	}
}
void DataManager::setUniform4i(string name, int v0, int v1, int v2, int v3)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform4i(name, v0, v1, v2, v3);
	}
}
void DataManager::setUniformMatrix(string name, const Mat3f& m)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniformMatrix(name, m);
	}
}
void DataManager::setUniformMatrix(string name, const Mat4f& m)
{
	if(boundShader != "")
	{
		static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniformMatrix(name, m);
	}
}

void DataManager::shutdown()
{
	boundShader = "";
	assets.clear();
}
