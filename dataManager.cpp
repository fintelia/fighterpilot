
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
	struct texCoord{float u,v;texCoord(float U, float V): u(U), v(V) {}texCoord(): u(0), v(0) {}};
	struct face{unsigned int v[3];unsigned int t[3];unsigned int n[3];unsigned int material;
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3, unsigned int n1, unsigned int n2, unsigned int n3) {	v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=n1;n[1]=n2;n[2]=n3;}
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=0;n[1]=0;n[2]=0;}
		face(unsigned int v1, unsigned int v2, unsigned int v3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}
		face() {v[0]=0;v[1]=0;v[2]=0;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}};
	struct triangle{Vec3f v1;texCoord t1;Vec3f n1;Vec3f v2;texCoord t2;Vec3f n2;Vec3f v3;texCoord t3;Vec3f n3;};
	struct mtl
	{
		string tex;
		string name;
		Color diffuse;
	};
////////////////////variables///////////////////////////
	unsigned int	numVertices=0,
					numTexcoords=0,
					numNormals=0,
					numFaces=0,
					numMtls=0;

	Vec3f*			vertices;
	Vec3f*			normals;
	texCoord*		texCoords;
	face*			faces;
	mtl*			mtls;

	unsigned int	totalVerts,totalFaces;

	FILE *fp;
	if((fp=fopen(filename.c_str(), "r")) == nullptr)
		return nullptr;

	char buffer[200];
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
	while(fgets(buffer, 200, fp) != NULL)
	{
		token = strtok(buffer, " ");
		if(strcmp(token, "v") == 0) 	numVertices++;
		if(strcmp(token, "vt") == 0) 	numTexcoords++;
		if(strcmp(token, "f") == 0) 	numFaces++;
		if(strcmp(token, "vn") == 0)	numNormals++;
		if(strcmp(token, "mtllib") == 0)mtlFile=file + strtok(NULL, "");
	}
	rewind(fp);
	//fclose(fp);
	if(mtlFile.size()!=0)
	{
		mtlFile=mtlFile.substr(0,mtlFile.size()-1);
///////////////////////LOAD MTL/////////////////////////////////
		{
			string mstring="";
			mtl mMtl;
			mMtl.tex = "";
			mMtl.name = "";
			mMtl.diffuse = white;

			map<string,mtl> m;

			ifstream fin;
			fin.open(mtlFile);
			string file;
			int i=mtlFile.find_last_of("/");
			if(i==string::npos)
				file.assign("");
			else
				file=mtlFile.substr(0,i+1);
			if(!fin.is_open())
			{
				messageBox(mtlFile + " could not be loaded");
				return nullptr;
			}
			while (!fin.eof())
			{
				string line;
				char l[256];
				string s[4];
				int i=0, h=0;

				fin.getline(l,256);
				line.assign(l);
				if(line.empty())
					continue;

				if(line[0] == '\t')
					boost::erase_head(line,1);

				h=line.find(" ");
				for(int n=0;n<4;n++)
				{
					s[n].assign(line.substr(i,h-i));
					i=h+1;
					if(i>(signed int)line.size())
						i=line.size();
					h=line.find(" ",h+2);
					if(h==string::npos)
						h=line.size();
				}

				//if(h!=string::npos)
				//{
				//	s[0].assign(line.substr(line.find_first_not_of("	 "),h));
				//	s[1].assign(line.substr(h+1,line.size()-h-1));
				//}
				if(s[0].compare("newmtl")==0)
				{
					if(mstring != "")
					{
						m.insert(pair<string,mtl>(mstring,mMtl));
					}
					mMtl.tex = "";
					mMtl.diffuse = white;
					mMtl.name = s[1];
					mstring=s[1];
				}
				else if(s[0].compare(0,6,"map_Kd")==0)
				{
					if(mstring == "") continue;

					mMtl.tex=file + line.substr(line.find_first_of(' ')+1,line.npos);
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
				else if(s[0].compare(0,2,"Kd")==0)
				{
					if(mstring == "") continue;
					try{
						float r = lexical_cast<float>(s[1]);
						float g = lexical_cast<float>(s[2]);
						float b = lexical_cast<float>(s[3]);
						mMtl.diffuse=Color(r,g,b,mMtl.diffuse.a);
					}catch(...){}
				}
				else if(s[0].compare(0,2,"d")==0)
				{
					if(mstring == "") continue;
					mMtl.diffuse.a=atof(s[1].c_str());

				}
				else if(s[0].compare(0,2,"Tr")==0)
				{
					if(mstring == "") continue;
					mMtl.diffuse.a=1.0-atof(s[1].c_str());
				}
			}
			m.insert(pair<string,mtl>(mstring,mMtl));
			mtl_map = m;
		}
///////////////////////LOAD MTL END/////////////////////////
	}

	try
	{
		vertices	= new Vec3f[numVertices];
		texCoords	= new texCoord[numTexcoords];
		faces		= new face[numFaces];
		normals		= new Vec3f[numNormals];
		mtls		= new mtl[mtl_map.size()];
	}
	catch(...)
	{
		fclose(fp);
		return nullptr;
	}

	for(map<string,mtl>::iterator itt=mtl_map.begin();itt!=mtl_map.end();itt++)
		mtls[numMtls++]=itt->second;

	totalVerts = numVertices;
	totalFaces = numFaces;

	numVertices=0;
	numTexcoords=0;
	numNormals=0;
	numFaces=0;

	int cMtl=-1;
	while(fgets(buffer, 200, fp) != NULL)
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
			sscanf(strtok(NULL, " "), "%f", &texCoords[numTexcoords].u);
			sscanf(strtok(NULL, " "), "%f", &texCoords[numTexcoords].v);

			texCoords[numTexcoords].v = 1.0f - texCoords[numTexcoords].v;
			numTexcoords++;
		}
		else if(strcmp(token, "vn") == 0)
		{
			sscanf(strtok(NULL, " "), "%f", &normals[numNormals].x);
			sscanf(strtok(NULL, " "), "%f", &normals[numNormals].y);
			sscanf(strtok(NULL, " "), "%f", &normals[numNormals].z);
			numNormals++;
		}
		else if(strcmp(token, "f") == 0)
		{
			int i, v = 0, t = 0, n = 0;

			for(i = 0; i<3; i++)
			{
				token = strtok(NULL, " \t");
				sscanf(token, "%d/%d/%d", &v, &t, &n);

				faces[numFaces].n[i] = n;
				faces[numFaces].t[i] = t;
				faces[numFaces].v[i] = v;
			}

			//Vec3f a, b;

			//for(i = 0; i<3; i++)
			//{
			//	a[i] = vertices[faces[numFaces].v[0]][i]	- vertices[faces[numFaces].v[1]][i];
			//	b[i] = vertices[faces[numFaces].v[2]][i]	- vertices[faces[numFaces].v[1]][i];
			//}
			faces[numFaces].material=cMtl;
			//normals[numNormals]=a.normalize().cross(b.normalize()).normalize();
			//numNormals++;
			numFaces++;
		}
		//else if(strcmp(token, "mtllib") == 0)  already loaded
		else if(strcmp(token, "usemtl") == 0)
		{
			string name=strtok(NULL, " ");
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

	texturedLitVertex3D* VBOverts = new texturedLitVertex3D[totalFaces*3];
	unsigned int lNum=0, vNum = 0;
	Vec3f faceNormal;
	for(int m=0; m<numMtls; m++)
	{
		for(int i=0; i < totalFaces; i++)
		{
			if(faces[i].material == m)
			{
				if(faces[i].v[0] == 0 || faces[i].v[1] == 0 || faces[i].v[2] == 0) continue;
				VBOverts[vNum+0].position = vertices[faces[i].v[0]-1];
				VBOverts[vNum+1].position = vertices[faces[i].v[1]-1];
				VBOverts[vNum+2].position = vertices[faces[i].v[2]-1];

				if(faces[i].n[0] == 0 || faces[i].n[1] == 0 || faces[i].n[2] == 0) //so we only have to generate the face normal once
					faceNormal = (VBOverts[vNum+1].position-VBOverts[vNum+0].position).cross(VBOverts[vNum+3].position-VBOverts[vNum+0].position);
				VBOverts[vNum+0].normal	= faces[i].n[0]!=0 ? normals[faces[i].n[0]-1] : faceNormal;
				VBOverts[vNum+1].normal	= faces[i].n[0]!=0 ? normals[faces[i].n[1]-1] : faceNormal;
				VBOverts[vNum+2].normal	= faces[i].n[0]!=0 ? normals[faces[i].n[2]-1] : faceNormal;


				if(faces[i].t[0] != 0)	VBOverts[vNum+0].UV = Vec2f(texCoords[faces[i].t[0]-1].u,	texCoords[faces[i].t[0]-1].v);
				if(faces[i].t[1] != 0)	VBOverts[vNum+1].UV = Vec2f(texCoords[faces[i].t[1]-1].u,	texCoords[faces[i].t[1]-1].v);
				if(faces[i].t[2] != 0)	VBOverts[vNum+2].UV = Vec2f(texCoords[faces[i].t[2]-1].u,	texCoords[faces[i].t[2]-1].v);

				vNum+=3;
			}
		}
		modelAsset::material mat;
		mat.color = mtls[m].diffuse;
		mat.tex = mtls[m].tex;
		mat.numIndices = vNum - lNum;
		mat.indicesOffset = lNum;
		a->materials.push_back(mat);
		lNum=vNum;
	}
	a->VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC);
	a->VBO->addPositionData(3,	0);
	a->VBO->addNormalData(3,	3*sizeof(float));
	a->VBO->addTexCoordData(2,	6*sizeof(float));
	a->VBO->setTotalVertexSize(sizeof(texturedLitVertex3D));
	a->VBO->setVertexData(sizeof(texturedLitVertex3D)*vNum, VBOverts);

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
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform1f(name, v0);
}
void DataManager::setUniform2f(string name, float v0, float v1)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform2f(name, v0, v1);
}
void DataManager::setUniform3f(string name, float v0, float v1, float v2)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform3f(name, v0, v1, v2);
}
void DataManager::setUniform4f(string name, float v0, float v1, float v2, float v3)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform4f(name, v0, v1, v2, v3);
}
void DataManager::setUniform1i(string name, int v0)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform1i(name, v0);
}
void DataManager::setUniform2i(string name, int v0, int v1)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform2i(name, v0, v1);
}
void DataManager::setUniform3i(string name, int v0, int v1, int v2)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform3i(name, v0, v1, v2);
}
void DataManager::setUniform4i(string name, int v0, int v1, int v2, int v3)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniform4i(name, v0, v1, v2, v3);
}
void DataManager::setUniformMatrix(string name, const Mat3f& m)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniformMatrix(name, m);
}
void DataManager::setUniformMatrix(string name, const Mat4f& m)
{
	static_pointer_cast<shaderAsset>(assets[boundShader])->shader->setUniformMatrix(name, m);
}

void DataManager::shutdown()
{
	boundShader = "";
	assets.clear();
}
