
#include "engine.h"
#include "GL/glee.h"
#include <GL/glu.h>
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



	//ifstream fin(filename, ios::binary);
	//if(!fin.is_open()) return false;
	//fin.read((char*)(&info), sizeof(info));

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

	string texName = f->filename + "_TEXTURE";
	auto ptr = registerTexture(fileManager.loadTextureFile(texPath));
	if(ptr != nullptr)
	{
		assets[texName] = ptr;

		fontAsset* f = new fontAsset;
		f->id = ((textureAsset*)ptr)->id;
		f->texName = texName;
		f->type = asset::FONT;
		f->height = (float)info.lineHeight;

		float invW = 1.0 / ((textureAsset*)assets[texName])->width;
		float invH = 1.0 / ((textureAsset*)assets[texName])->height;

		fontAsset::character tmpChar;
		for(auto i = 0; i < numChars; i++)
		{
			tmpChar.UV = Rect::XYWH(invW * fontChars[i].x, invH * fontChars[i].y, invW * fontChars[i].w, invH * fontChars[i].h);
			tmpChar.width = fontChars[i].w;
			tmpChar.height = fontChars[i].h;
			tmpChar.xAdvance = fontChars[i].xAdvance;
			tmpChar.xOffset = fontChars[i].xOffset;
			tmpChar.yOffset = fontChars[i].yOffset;
			f->characters[fontChars[i].id] = tmpChar;
		}

		delete [] fontChars;
		return f;
	}
	else
	{
		delete [] fontChars;
		return nullptr;
	}
}
//bool DataManager::registerTGA(string name, string filename, bool tileable)
//{
///////////////structs///////////////
//	typedef struct
//	{
//		GLubyte header[6];			// Holds The First 6 Useful Bytes Of The File
//		GLuint bytesPerPixel;		// Number Of BYTES Per Pixel (3 Or 4)
//		GLuint imageSize;			// Amount Of Memory Needed To Hold The Image
//		GLuint type;				// The Type Of Image, GL_RGB Or GL_RGBA
//		GLuint Height;				// Height Of Image
//		GLuint Width;				// Width Of Image
//		GLuint Bpp;					// Number Of BITS Per Pixel (24 Or 32)
//	} TGA;
//	typedef struct
//	{
//		unsigned char* imageData;	// Hold All The Color Values For The Image.
//		GLuint bpp; 				// Hold The Number Of Bits Per Pixel.
//		GLuint width;				// The Width Of The Entire Image.
//		GLuint height;				// The Height Of The Entire Image.
//		GLuint texID;				// Texture ID For Use With glBindTexture.
//		GLuint type;			 	// Data Stored In * ImageData (GL_RGB Or GL_RGBA)
//	} Texture;
//
///////////////variables/////////////////////////
//	GLuint texV=0;
//	Texture texture;
//	GLubyte tgaheader[12];		// Used To Store Our File Header
//	TGA tga;					// Used To Store File Information
//
//	bool NPOT;//determined later
////////////////////code////////////////////////
//	ifstream fin(filename, ios::binary);
//    fin.read((char *)(&tgaheader), sizeof(tgaheader));
//	fin.read((char *)(&tga.header), sizeof(tga.header));
//    texture.width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
//	texture.height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
//	texture.bpp	= tga.header[4];											// Determine the bits per pixel
//	tga.Width		= texture.width;										// Copy width into local structure
//	tga.Height		= texture.height;										// Copy height into local structure
//	tga.Bpp			= texture.bpp;											// Copy BPP into local structure
//	if((texture.width <= 0) || (texture.height <= 0) || ((texture.bpp != 24) && (texture.bpp !=32)))	// Make sure all information is valid
//	{
//		fin.close();
//		//assert (0);
//		return FALSE;
//	}
//	NPOT = GLEE_ARB_texture_non_power_of_two && ((texture.width & (texture.width-1)) || (texture.height & (texture.height-1)));
//	if(texture.bpp == 24)													// If the BPP of the image is 24...
//		texture.type	= GL_RGB;											// Set Image type to GL_RGB
//	else																	// Else if its 32 BPP
//		texture.type	= GL_RGBA;											// Set image type to GL_RGBA
//
//	tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute the number of BYTES per pixel
//	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute the total amout ofmemory needed to store data
//	texture.imageData	= new GLubyte[tga.imageSize];						// Allocate that much memory
//
//
//	if(texture.imageData == NULL)											// If no space was allocated
//	{
//		fin.close();
//		//assert (0);
//		return false;
//	}
//	fin.read((char *)(texture.imageData), tga.imageSize);
//	//Byte Swapping Optimized By Steve Thomas
//	for(int cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
//	{
//		//texture.imageData[cswap] ^= texture.imageData[cswap+2] ^= texture.imageData[cswap] ^= texture.imageData[cswap+2];
//		swap(texture.imageData[cswap], texture.imageData[cswap+2]);
//	}
//
//	fin.close();
//
//	glGenTextures(1,&texV);
//    glBindTexture(GL_TEXTURE_2D, texV);
//
//    glTexImage2D(GL_TEXTURE_2D, 0, 4 , tga.Width, tga.Height, 0,texture.type, GL_UNSIGNED_BYTE, (GLvoid *)texture.imageData);
//
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tileable ? GL_REPEAT : GL_CLAMP_TO_EDGE);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tileable ? GL_REPEAT : GL_CLAMP_TO_EDGE);
//
//	if(NPOT)
//	{
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	}
//	else
//	{
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	}
//
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//
//	if(NPOT)
//		glTexImage2D(GL_TEXTURE_2D,0, texture.type, tga.Width, tga.Height,0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
//	else
//		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tga.Width, tga.Height, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
//
//    delete[] texture.imageData;
//	textureAsset* a = new textureAsset;
//	a->id = texV;
//	a->type = asset::TEXTURE;
//	a->width = tga.Width;
//	a->height = tga.Height;
//	a->bpp = tga.Bpp;
//	a->data = NULL;
//	assets[name] = a;
//	return true;
//}
DataManager::asset* DataManager::registerTexture(shared_ptr<FileManager::textureFile> f, bool tileable)
{
	if(!f->valid())
		return nullptr;

	int format;
	if(f->channels == 1)		format = GL_LUMINANCE;
	else if(f->channels == 2)	format = GL_LUMINANCE_ALPHA;
	else if(f->channels == 3)	format = GL_RGB;
	else if(f->channels == 4)	format = GL_RGBA;
	else{
		debugBreak();
		return nullptr;
	}

	bool NPOT = GLEE_ARB_texture_non_power_of_two && ((f->width & (f->width-1)) || (f->height & (f->height-1)));

	GLuint texV = 0;
	glGenTextures(1,&texV);
	glBindTexture(GL_TEXTURE_2D, texV);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if(tileable)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if(NPOT)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//any OpenGL call here can cause a "hardware does not meet minimum specifications" driver error, if multisampling is set to 16x
		glTexImage2D(GL_TEXTURE_2D, 0, f->channels, f->width, f->height,0, format, GL_UNSIGNED_BYTE, f->contents);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, f->channels, f->width, f->height, format, GL_UNSIGNED_BYTE, f->contents);
	}

	textureAsset* a = new textureAsset;
	a->id = texV;
	a->type = asset::TEXTURE;
	a->width = f->width;
	a->height = f->height;
	a->bpp = f->channels * 8;
	a->data = NULL;
	return a;
}
//bool DataManager::registerTexture(string name, string filename, bool tileable)
//{
//	//string ext=fileManager.extension(filename);
//	//if(ext.compare(".tga") == 0)		return registerTGA(name, filename, tileable);
//	//else if(ext.compare(".png") == 0)
//	//{
//		auto f = fileManager.loadTextureFile(filename);
//		return registerTexture(name, f, tileable);
//	//}
//	//else return false;
//}
//bool DataManager::registerShader(string name, string vert, string frag, bool use_sAspect)
//{
//	bool errorFlag = false;
//	GLuint v=0,f=0,p;
//	v = glCreateShader(GL_VERTEX_SHADER);
//	f = glCreateShader(GL_FRAGMENT_SHADER);
//
//	auto ff = fileManager.loadTextFile(frag);
//	auto vv = fileManager.loadTextFile(vert);
//	if(ff->invalid()  || vv->invalid()) return false;
//
//	const char* ptr = vv->contents.c_str();	glShaderSource(v, 1, (const char **)&ptr, NULL);
//	ptr = ff->contents.c_str();				glShaderSource(f, 1, (const char **)&ptr, NULL);
//
//	glCompileShader(v);
//	glCompileShader(f);
//
//	string vertErrors;
//	string fragErrors;
//	string linkErrors;
//
//	int i;//used whenever a pointer to int is required
//	glGetShaderiv(v,GL_COMPILE_STATUS,&i);
//	if(i == GL_FALSE)
//	{
//		glGetShaderiv(v,GL_INFO_LOG_LENGTH,&i);
//		char* cv=new char[i]; memset(cv,0,i);
//		glGetShaderInfoLog(v,i,&i,cv);
//		messageBox(vert + ": " + cv);
//		errorFlag = true;
//		delete[] cv;
//	}
//	glGetShaderiv(f,GL_COMPILE_STATUS,&i);
//	if(i == GL_FALSE && !errorFlag)
//	{
//		glGetShaderiv(f,GL_INFO_LOG_LENGTH,&i);
//		char* cf=new char[i]; memset(cf,0,i);
//		glGetShaderInfoLog(f,i,&i,cf);
//		messageBox(frag + ": " + cf);
//		errorFlag = true;
//		delete[] cf;
//	}
//
//
//	p = glCreateProgram();
//	glAttachShader(p,f);
//	glAttachShader(p,v);
//
//	glLinkProgram(p);
//
//	glGetProgramiv(p,GL_LINK_STATUS,&i);
//	if(i == GL_FALSE && !errorFlag)
//	{
//		glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
//		char* cl=new char[i]; memset(cl,0,i);
//		glGetProgramInfoLog(p,i,&i,cl);
//		messageBox(frag + "(link): " + cl);
//		errorFlag = true;
//		delete[] cl;
//	}
//	glUseProgram(0);
//
//	if(!errorFlag)
//	{
//		shaderAsset* a = new shaderAsset;
//		a->id = p;
//		a->type = asset::SHADER;
//		a->use_sAspect = use_sAspect;
//		assets[name] = a;
//	}
//	return !errorFlag;
//}
DataManager::asset* DataManager::registerShader(shared_ptr<FileManager::textFile> vert, shared_ptr<FileManager::textFile> frag, bool use_sAspect)
{
	bool errorFlag = false;
	GLuint v=0,f=0,p;


	//auto ff = fileManager.loadTextFile(frag);
	//auto vv = fileManager.loadTextFile(vert);
	if(frag->invalid()  || vert->invalid())
		return nullptr;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	const char* ptr = vert->contents.c_str();	glShaderSource(v, 1, (const char **)&ptr, NULL);
	ptr = frag->contents.c_str();				glShaderSource(f, 1, (const char **)&ptr, NULL);

	glCompileShader(v);
	glCompileShader(f);

	string vertErrors;
	string fragErrors;
	string linkErrors;

	int i;//used whenever a pointer to int is required
	glGetShaderiv(v,GL_COMPILE_STATUS,&i);
	if(i == GL_FALSE)
	{
		glGetShaderiv(v,GL_INFO_LOG_LENGTH,&i);
		char* cv=new char[i]; memset(cv,0,i);
		glGetShaderInfoLog(v,i,&i,cv);
		messageBox(vert->filename + ": " + cv);
		errorFlag = true;
		delete[] cv;
	}
	glGetShaderiv(f,GL_COMPILE_STATUS,&i);
	if(i == GL_FALSE && !errorFlag)
	{
		glGetShaderiv(f,GL_INFO_LOG_LENGTH,&i);
		char* cf=new char[i]; memset(cf,0,i);
		glGetShaderInfoLog(f,i,&i,cf);
		messageBox(frag->filename + ": " + cf);
		errorFlag = true;
		delete[] cf;
	}


	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);

	glLinkProgram(p);

	glGetProgramiv(p,GL_LINK_STATUS,&i);
	if(i == GL_FALSE && !errorFlag)
	{
		glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
		char* cl=new char[i]; memset(cl,0,i);
		glGetProgramInfoLog(p,i,&i,cl);
		messageBox(frag->filename + "(link): " + cl);
		errorFlag = true;
		delete[] cl;
	}
	glUseProgram(0);

	glDeleteShader(f); // we no longer need these shaders individually, although they
	glDeleteShader(v); // will not actually be deleted until the shader program is deleted

	if(!errorFlag)
	{
		shaderAsset* a = new shaderAsset;
		a->id = p;
		a->type = asset::SHADER;
		a->use_sAspect = use_sAspect;
		return a;
	}
	else
	{
		return nullptr;
	}
}
//bool DataManager::registerTerrainShader(string name, string frag)
//{
//	bool errorFlag = false;
//	static GLuint v=0;
//	const char* ptr;
//
//	if(v==0)
//	{
//		v = glCreateShader(GL_VERTEX_SHADER);
//		
//		auto vv = fileManager.loadTextFile("media/terrain.vert");
//		if(vv->invalid()) return false;
//		ptr = vv->contents.c_str();
//		glShaderSource(v, 1, (const char **)&ptr, NULL);
//		glCompileShader(v);
//
//		int i;//used whenever a pointer to int is required
//		glGetShaderiv(v,GL_COMPILE_STATUS,&i);
//		if(i == GL_FALSE)
//		{
//			glGetShaderiv(v,GL_INFO_LOG_LENGTH,&i);
//			char* cv=new char[i]; memset(cv,0,i);
//			glGetShaderInfoLog(v,i,&i,cv);
//			messageBox(string("terrain.vert: ") + cv);
//			errorFlag = true;
//			delete[] cv;
//		}
//	}
//
//	GLuint	f = glCreateShader(GL_FRAGMENT_SHADER),
//			p = 0;
//	
//	auto	ff = fileManager.loadTextFile(frag);
//	int		lf=0;
//
//	if(ff->valid())
//	{
//		char* cf=new char[512];
//		ptr = ff->contents.c_str();
//		glShaderSource(f, 1, (const char **)&ptr, NULL);
//		glCompileShader(f);
//		memset(cf,0,512);
//		glGetShaderInfoLog(f,512,&lf,cf);
//		delete[] cf;
//
//		int i;
//		glGetShaderiv(f,GL_COMPILE_STATUS,&i);
//		if(i == GL_FALSE && !errorFlag)
//		{
//			glGetShaderiv(f,GL_INFO_LOG_LENGTH,&i);
//			char* cf=new char[i]; memset(cf,0,i);
//			glGetShaderInfoLog(f,i,&i,cf);
//			messageBox(frag + ": " + cf);
//			errorFlag = true;
//			delete[] cf;
//		}
//
//		p = glCreateProgram();
//		glAttachShader(p,f);
//		glAttachShader(p,v);
//		glLinkProgram(p);
//
//		glGetProgramiv(p,GL_LINK_STATUS,&i);
//		if(i == GL_FALSE && !errorFlag)
//		{
//			glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
//			char* cl=new char[i]; memset(cl,0,i);
//			glGetProgramInfoLog(p,i,&i,cl);
//			messageBox(frag + "(link): " + cl);
//			errorFlag = true;
//			delete[] cl;
//		}
//
//	}
//	glUseProgram(0);
//
//	if(!errorFlag)
//	{
//		shaderAsset* a = new shaderAsset;
//		a->id = p;
//		a->type = asset::SHADER;
//		a->use_sAspect = false;
//		assets[name] = a;
//	}
//	return !errorFlag;
//}
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
						assets[mMtl.tex] = texPtr;
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
	//fopen_s(&fp,filename, "r");

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
	a->VBO = graphics->genVertexBuffer(GraphicsManager::vertexBuffer::STATIC, false);
	a->VBO->addPositionData(3,	0);
	a->VBO->addNormalData(3,	3*sizeof(float));
	a->VBO->addTexCoordData(2,	6*sizeof(float));
	a->VBO->setTotalVertexSize(sizeof(texturedLitVertex3D));
	a->VBO->bindBuffer();
	a->VBO->setVertexData(sizeof(texturedLitVertex3D)*vNum, VBOverts);

	//glGenBuffers(1,(GLuint*)&a->id);
	//glBindBuffer(GL_ARRAY_BUFFER, a->id);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(texturedLitVertex3D)*vNum, VBOverts, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	delete[] fs;
	delete[] vertices;
	delete[] texCoords;
	delete[] faces;
	delete[] normals;
	delete[] mtls;

	return a;
}

void DataManager::bind(string name, int textureUnit)
{
	if(assets.find(name)==assets.end())
		return;
	else if(assets[name]->type==asset::TEXTURE)
	{
		if(boundTextures[textureUnit] == name)
			return;
		else if(boundTextureIds[textureUnit] == ((textureAsset*)assets[name])->id)
		{
			boundTextures[textureUnit] = name;
			return;
		}

		glActiveTexture(GL_TEXTURE0+textureUnit);
		glBindTexture(GL_TEXTURE_2D,((textureAsset*)assets[name])->id);

		boundTextureIds[textureUnit] = ((textureAsset*)assets[name])->id;
		boundTextures[textureUnit] = name;
	}
	else if(assets[name]->type==asset::SHADER)
	{
		if(boundShader == name)
			return;
		else if(boundShaderId == ((shaderAsset*)assets[name])->id)
		{
			boundShader = name;
			return;
		}

		glUseProgram(((shaderAsset*)assets[name])->id);
		boundShaderId = ((shaderAsset*)assets[name])->id;
		boundShader = name;
	}
}
void DataManager::bindTex(int id, int textureUnit)
{
	if(boundTextureIds[textureUnit] == id)
		return;

	glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(GL_TEXTURE_2D,id);

	boundTextureIds[textureUnit] = id;
	if(id == 0)		boundTextures[textureUnit] = "noTexture";
	else			boundTextures[textureUnit] = "";
}
void DataManager::bindShader(int id)
{
	if(boundShaderId == id)
		return;

	glUseProgram(id);

	boundShaderId = id;
	if(id == 0)		boundShader = "noShader";
	else			boundShader = "";
}
void DataManager::unbind(string name)
{
	if(boundShader == name)
	{
		glUseProgram(0);
		boundShaderId = 0;
		boundShader = "noShader";
	}
	else
	{
		for(auto i = boundTextures.begin();i!=boundTextures.end();i++)
		{
			if(i->second == name)
			{

				glActiveTexture(GL_TEXTURE0+i->first);
				glBindTexture(GL_TEXTURE_2D,0);

				boundTextureIds[i->first] = 0;
				boundTextures[i->first] = "noTexture";
				return;
			}
		}
	}
}
void DataManager::unbindTextures()
{
	for(unsigned int i = 0; i < boundTextureIds.size(); i++)
	{
		if(boundTextureIds[i] != 0)
		{
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D,0);

			boundTextures[i] = "noTexture";
			boundTextureIds[i] = 0;
		}
	}
}
void DataManager::unbindShader()
{
	glUseProgram(0);
	boundShaderId = 0;
	boundShader = "";
}

bool DataManager::loadAssetList()
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
				tmpAssetFile.type = asset::TEXTURE;

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
				tmpAssetFile.type = asset::SHADER;

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
				tmpAssetFile.type = asset::MODEL;

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
				tmpAssetFile.type = asset::FONT;

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
int DataManager::loadAsset()
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
		if(file.type == asset::TEXTURE)
		{
			if(file.files.empty()) file.files.push_back(fileManager.loadTextureFile(file.filename[0]));
			if(!file.files.front()->complete()) break;

			bool t = file.options.count("tileable") != 0;
			//registerTexture(file.name, file.filename[0], t);
			auto texPtr = registerTexture(dynamic_pointer_cast<FileManager::textureFile>(file.files.front()), t);
			if(texPtr) assets[file.name] = texPtr;
			pop(preload);
		}
		else if(file.type == asset::SHADER)
		{
			if(file.files.size() == 0) file.files.push_back(fileManager.loadTextureFile(file.filename[0]));
			if(file.files.size() == 1) file.files.push_back(fileManager.loadTextureFile(file.filename[0]));
			if(!file.files[0]->complete() || !file.files[1]->complete()) break;
			auto shaderPtr = registerShader(dynamic_pointer_cast<FileManager::textFile>(file.files[0]), dynamic_pointer_cast<FileManager::textFile>(file.files[1]));
			if(shaderPtr)
			{
				assets[file.name] = shaderPtr;
				if(shaderPtr && file.options.count("use_sAspect") != 0)
				{
					auto s = assets.find(file.name);
					((shaderAsset*)(s->second))->use_sAspect = true;

					bind(file.name);
					setUniform1f("sAspect",sAspect);
					unbind(file.name);
				}
			}
			pop(preload);
			//break;
		}
		else if(file.type == asset::MODEL)
		{
			auto modelPtr = registerOBJ(file.filename[0]);
			if(modelPtr) assets[file.name] = modelPtr;
			pop(preload);
			break;
		}
		else if(file.type == asset::FONT)
		{
			if(file.files.empty()) file.files.push_back(fileManager.loadTextFile(file.filename[0]));
			if(!file.files.front()->complete()) break;

			auto fontPtr = registerFont(dynamic_pointer_cast<FileManager::textFile>(file.files.front()));
			if(fontPtr) assets[file.name] = fontPtr;
			pop(preload);
			break;
		}
	}while(!assetFilesPreload.empty() && !assetFiles.empty());
	
	return assetFiles.size();
}

int DataManager::getId(string name)
{
	auto a = assets.find(name);
	if(a != assets.end())
	{
		if(a->second->type == asset::TEXTURE)		return ((textureAsset*)a->second)->id;
		else if(a->second->type == asset::SHADER)	return ((shaderAsset*)a->second)->id;
		else if(a->second->type == asset::FONT)		return ((fontAsset*)a->second)->id;
	}
	return 0;
}
int DataManager::getId(objectType t)
{
	return getId(objectTypeString(t));
}
const DataManager::fontAsset* DataManager::getFont(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::FONT)
		return nullptr;

	return (const fontAsset*)i->second;
}
const DataManager::modelAsset* DataManager::getModel(string name)
{
	auto i = assets.find(name);
	if(i == assets.end() || i->second->type != asset::MODEL)
		return nullptr;

	return (const modelAsset*)i->second;
}
bool DataManager::assetLoaded(string name)
{
	return assets.find(name) != assets.end();
}

void DataManager::setUniform1f(string name, float v0)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform1f(((shaderAsset*)assets[boundShader])->uniforms[name],v0);
	}
}
void DataManager::setUniform2f(string name, float v0, float v1)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform2f(((shaderAsset*)assets[boundShader])->uniforms[name],v0,v1);
	}
}
void DataManager::setUniform3f(string name, float v0, float v1, float v2)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform3f(((shaderAsset*)assets[boundShader])->uniforms[name],v0,v1,v2);
	}
}
void DataManager::setUniform4f(string name, float v0, float v1, float v2, float v3)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform4f(((shaderAsset*)assets[boundShader])->uniforms[name],v0,v1,v2,v3);
	}
}
void DataManager::setUniform1i(string name, int v0)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform1i(((shaderAsset*)assets[boundShader])->uniforms[name],v0);
	}
}
void DataManager::setUniform2i(string name, int v0, int v1)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform2i(((shaderAsset*)assets[boundShader])->uniforms[name],v0,v1);
	}
}
void DataManager::setUniform3i(string name, int v0, int v1, int v2)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform3i(((shaderAsset*)assets[boundShader])->uniforms[name],v0,v1,v2);
	}
}
void DataManager::setUniform4i(string name, int v0, int v1, int v2, int v3)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniform4i(((shaderAsset*)assets[boundShader])->uniforms[name],v0,v1,v2,v3);
	}
}
void DataManager::setUniformMatrix(string name, const Mat3f& m)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniformMatrix3fv(((shaderAsset*)assets[boundShader])->uniforms[name],1,false,m.ptr());
	}
}
void DataManager::setUniformMatrix(string name, const Mat4f& m)
{
	if(boundShaderId != 0 && boundShader != "noShader")
	{
		if(((shaderAsset*)assets[boundShader])->uniforms.find(name) == ((shaderAsset*)assets[boundShader])->uniforms.end())
			((shaderAsset*)assets[boundShader])->uniforms[name] = glGetUniformLocation(boundShaderId,name.c_str());

		glUniformMatrix4fv(((shaderAsset*)assets[boundShader])->uniforms[name],1,false,m.ptr());
	}
}

void DataManager::shutdown()
{
	boundShader = "";
	boundTextures.clear();
	for(auto i = assets.begin(); i != assets.end(); i++)
	{
		if(i->second->type == asset::SHADER)			glDeleteProgram(((shaderAsset*)i->second)->id);
		else if(i->second->type == asset::MODEL)		delete ((modelAsset*)i->second)->VBO;
		else if(i->second->type == asset::TEXTURE)		glDeleteTextures(1,(const GLuint*)&((textureAsset*)i->second)->id);
	}
	assets.clear();
}
