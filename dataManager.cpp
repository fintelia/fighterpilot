
#include "main.h"

DataManager::~DataManager()
{
	shutdown();
}

int DataManager::loadTexture(string filename)
{
	string ext=filesystem::extension(filename);
	if(ext.compare(".tga") == 0)	return loadTGA(filename);
	if(ext.compare(".mmp") == 0)	return loadMMP(filename);
	if(ext.compare(".png") == 0)	return loadPNG(filename);
	return 0;
}
int DataManager::loadModel(string filename)
{
	string ext=filesystem::extension(filename);
	if(ext.compare(".obj") == 0)
	{
		int id = loadOBJ(filename);
		registerModel(filename,id);
		return id;
	}
	return 0;
}
int DataManager::loadShader(string vert, string frag)
{
	bool errorFlag = false;
	GLuint v=0,f=0,p;
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	char * ff = textFileRead((char*)frag.c_str());
	char * vv = textFileRead((char*)vert.c_str());
	if(ff == NULL || vv == NULL) return 0;

	glShaderSource(v, 1, (const char **)&vv, NULL);
	glShaderSource(f, 1, (const char **)&ff, NULL);
	glCompileShader(v);
	glCompileShader(f);
	free(ff);
	free(vv);

	string vertErrors;
	string fragErrors;
	string linkErrors;

	int i;//used whenever a pointer to int is required
	glGetShaderiv(v,GL_COMPILE_STATUS,&i);
	if(i == GL_FALSE)
	{
		glGetShaderiv(v,GL_INFO_LOG_LENGTH,&i);
		char* cv=(char*)malloc(i); memset(cv,0,i);
		glGetShaderInfoLog(v,i,&i,cv);
		messageBox(vert + ": " + cv);
		errorFlag = true;
	}
	glGetShaderiv(f,GL_COMPILE_STATUS,&i);
	if(i == GL_FALSE)
	{
		glGetShaderiv(f,GL_INFO_LOG_LENGTH,&i);
		char* cf=(char*)malloc(i); memset(cf,0,i);
		glGetShaderInfoLog(f,i,&i,cf);
		messageBox(frag + ": " + cf);
		errorFlag = true;
	}


	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);

	glLinkProgram(p);

	glGetProgramiv(p,GL_LINK_STATUS,&i);
	if(i == GL_FALSE)
	{
		glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
		char* cl=(char*)malloc(i); memset(cl,0,i);
		glGetProgramInfoLog(p,i,&i,cl);
		messageBox(frag + "(link): " + cl);
		errorFlag = true;
	}

	glUseProgram(0); 

	//if(lv != 0) MessageBoxA(NULL,(string(vert)+": "+string(cv)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
	//if(lf != 0) MessageBoxA(NULL,(string(frag)+": "+string(cf)).c_str(), "Shader Error",MB_ICONEXCLAMATION);

	return errorFlag ? 0 : p;
}
int DataManager::loadTerrainShader(string frag)
{
	bool errorFlag = false;
	static GLuint v=0;
	

	if(v==0)
	{
		v = glCreateShader(GL_VERTEX_SHADER);
		char * vv = textFileRead("media/terrain.vert");
		if(vv == NULL) return 0;
		glShaderSource(v, 1, (const char **)&vv, NULL);
		glCompileShader(v);
		free(vv);

		int i;//used whenever a pointer to int is required
		glGetShaderiv(v,GL_COMPILE_STATUS,&i);
		if(i == GL_FALSE)
		{
			glGetShaderiv(v,GL_INFO_LOG_LENGTH,&i);
			char* cv=(char*)malloc(i); memset(cv,0,i);
			glGetShaderInfoLog(v,i,&i,cv);
			messageBox(string("terrain.vert: ") + cv);
			errorFlag = true;
		}
	}

	GLuint	f = glCreateShader(GL_FRAGMENT_SHADER),
			p = 0;
	char	*ff = textFileRead((char*)frag.c_str()),
			*cf=(char*)malloc(512);
	int		lf=0;

	if(ff != NULL) 
	{
		glShaderSource(f, 1, (const char **)&ff, NULL);
		glCompileShader(f);
		free(ff);
		memset(cf,0,512);
		glGetShaderInfoLog(f,512,&lf,cf);

		int i;
		glGetShaderiv(f,GL_COMPILE_STATUS,&i);
		if(i == GL_FALSE)
		{
			glGetShaderiv(f,GL_INFO_LOG_LENGTH,&i);
			char* cf=(char*)malloc(i); memset(cf,0,i);
			glGetShaderInfoLog(f,i,&i,cf);
			messageBox(frag + ": " + cf);
			errorFlag = true;
		}

		p = glCreateProgram();
		glAttachShader(p,f);
		glAttachShader(p,v);
		glLinkProgram(p);

		glGetProgramiv(p,GL_LINK_STATUS,&i);
		if(i == GL_FALSE)
		{
			glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
			char* cl=(char*)malloc(i); memset(cl,0,i);
			glGetProgramInfoLog(p,i,&i,cl);
			messageBox(frag + "(link): " + cl);
			errorFlag = true;
		}

	}
	glUseProgram(0);

	return errorFlag ? 0 : p;
}

int DataManager::loadTGA(string filename)
{
/////////////structs///////////////
	typedef struct
	{
		GLubyte header[6];			// Holds The First 6 Useful Bytes Of The File
		GLuint bytesPerPixel;		// Number Of BYTES Per Pixel (3 Or 4)
		GLuint imageSize;			// Amount Of Memory Needed To Hold The Image
		GLuint type;				// The Type Of Image, GL_RGB Or GL_RGBA
		GLuint Height;				// Height Of Image					
		GLuint Width;				// Width Of Image				
		GLuint Bpp;					// Number Of BITS Per Pixel (24 Or 32)
	} TGA;
	typedef struct
	{
		unsigned char* imageData;	// Hold All The Color Values For The Image.
		GLuint bpp; 				// Hold The Number Of Bits Per Pixel.			
		GLuint width;				// The Width Of The Entire Image.	
		GLuint height;				// The Height Of The Entire Image.	
		GLuint texID;				// Texture ID For Use With glBindTexture.	
		GLuint type;			 	// Data Stored In * ImageData (GL_RGB Or GL_RGBA)
	} Texture;

/////////////variables/////////////////////////
	GLuint texV=0;
    int j=0; //Index variables
	
	Texture texture;
	GLubyte tgaheader[12];		// Used To Store Our File Header
	TGA tga;					// Used To Store File Information
	GLubyte uTGAcompare[12] = {0,0,2,0,0,0,0,0,0,0,0,0};// Uncompressed TGA Header

	bool NPOT;//determined later
//////////////////code////////////////////////
	ifstream fin(filename, ios::binary);
    fin.read((char *)(&tgaheader), sizeof(tgaheader));
	fin.read((char *)(&tga.header), sizeof(tga.header));
    texture.width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	texture.height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	texture.bpp	= tga.header[4];											// Determine the bits per pixel
	tga.Width		= texture.width;										// Copy width into local structure						
	tga.Height		= texture.height;										// Copy height into local structure
	tga.Bpp			= texture.bpp;											// Copy BPP into local structure
	if((texture.width <= 0) || (texture.height <= 0) || ((texture.bpp != 24) && (texture.bpp !=32)))	// Make sure all information is valid
	{
		fin.close();
		//assert (0);
		return 0;
	}
	NPOT = GLEE_ARB_texture_non_power_of_two && ((texture.width & (texture.width-1)) || (texture.height & (texture.height-1)));
	if(texture.bpp == 24)													// If the BPP of the image is 24...
		texture.type	= GL_RGB;											// Set Image type to GL_RGB
	else																	// Else if its 32 BPP
		texture.type	= GL_RGBA;											// Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute the total amout ofmemory needed to store data
	texture.imageData	= (GLubyte *)malloc(tga.imageSize);					// Allocate that much memory

	
	if(texture.imageData == NULL)											// If no space was allocated 
	{
		fin.close();
		//assert (0);
		return 0;
	}
	fin.read((char *)(texture.imageData), tga.imageSize);
	//Byte Swapping Optimized By Steve Thomas
	for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		texture.imageData[cswap] ^= texture.imageData[cswap+2] ^= texture.imageData[cswap] ^= texture.imageData[cswap+2];
	}
	
	fin.close();
	
	glGenTextures(1,&texV);
    glBindTexture(GL_TEXTURE_2D, texV);

    glTexImage2D(GL_TEXTURE_2D, 0, 4 , tga.Width, tga.Height, 0,texture.type, GL_UNSIGNED_BYTE, (GLvoid *)texture.imageData);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	if(NPOT)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	if(NPOT)
		glTexImage2D(GL_TEXTURE_2D,0, texture.type, tga.Width, tga.Height,0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tga.Width, tga.Height, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
	
    free(texture.imageData);
    return texV;
}
int DataManager::loadMMP(string filename)
{
	struct MMPheader
	{
		int size_x;
		int size_y;
		int channels;
	};
	MMPheader h;
	ifstream fin;
	fin.open(filename,ios::binary);
	if(!fin.is_open()) return 0;
	fin.read((char*)&h,sizeof(h));
	if(h.channels != 3 && h.channels != 4) return 0;

	unsigned char* imageData=(unsigned char*)malloc(h.size_x*h.size_y*h.channels);
	if(imageData==NULL) return 0;
	
	GLuint texV;
	glGenTextures(1,&texV);
    glBindTexture(GL_TEXTURE_2D, texV);

	
	int x=h.size_x, y=h.size_y, mLevel=0;
	while (true) 
	{
		fin.read((char*)imageData,x*y*h.channels);

		//set mipmap
		if(h.channels==3)
			glTexImage2D(GL_TEXTURE_2D, mLevel++, 3, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
		else if(h.channels==4)
			glTexImage2D(GL_TEXTURE_2D, mLevel++, 4, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
		if(x==1 && y==1) break;
		x=max(x>>1,1);//divide by 2
		y=max(y>>1,1);

	}
	fin.close();
	free(imageData);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return texV;
}
int DataManager::loadOBJ(string filename)
{
///////////////////////types////////////////////////////
	struct color{float r,g,b;color(float red, float green, float blue): r(red), g(green), b(blue){}};
	struct texCoord{float u,v;texCoord(float U, float V): u(U), v(V) {}texCoord(): u(0), v(0) {}};
	struct face{unsigned int v[3];unsigned int t[3];unsigned int n[3];unsigned int material;
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3, unsigned int n1, unsigned int n2, unsigned int n3) {	v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=n1;n[1]=n2;n[2]=n3;}
		face(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int t1, unsigned int t2, unsigned int t3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=t1;t[1]=t2;t[2]=t3;n[0]=0;n[1]=0;n[2]=0;}
		face(unsigned int v1, unsigned int v2, unsigned int v3) {v[0]=v1;v[1]=v2;v[2]=v3;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}
		face() {v[0]=0;v[1]=0;v[2]=0;t[0]=0;t[1]=0;t[2]=0;n[0]=0;n[1]=0;n[2]=0;}};
	struct triangle{Vec3f v1;texCoord t1;Vec3f n1;Vec3f v2;texCoord t2;Vec3f n2;Vec3f v3;texCoord t3;Vec3f n3;};
	struct mtl
	{	int tex;
		color diffuse;
		float transparency;
		string name;
		mtl():diffuse(1,1,1){}
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
	if(fopen_s(&fp,filename.c_str(), "r"))
		return 0;

	char buffer[200];
	char *token, *nextToken=NULL;
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
		token = strtok_s(buffer, " ", &nextToken);
		if(strcmp(token, "v") == 0) 	numVertices++;
		if(strcmp(token, "vt") == 0) 	numTexcoords++;
		if(strcmp(token, "f") == 0) 	numFaces++;
		if(strcmp(token, "vn") == 0)	numNormals++;
		if(strcmp(token, "mtllib") == 0)mtlFile=file + strtok_s(NULL, " ", &nextToken);
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
			mMtl.tex = dataManager.getId("white");
			mMtl.diffuse = color(1,1,1);
			mMtl.transparency = 1.0;
			mMtl.name = "";

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
				messageBox("mtl file could not be loaded");
				exit(0);
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
					erase_head(line,1);

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
					if(!mstring.compare("")!=0)
					{
						mMtl.name=mstring;
						m.insert(pair<string,mtl>(mstring,mMtl));

					}
					mMtl.tex = dataManager.getId("white");
					mMtl.diffuse = color(1,1,1);
					mMtl.transparency = 1.0;
					mMtl.name = "";
					mstring=s[1];
				}
				else if(s[0].compare(0,6,"map_Kd")==0)
				{
					if(!mstring.compare("")) continue;
					//string ext=(file+s[1]).substr((file+s[1]).find_last_of(".")+1);
					//if(ext.compare("tga")==0)
						mMtl.tex=dataManager.loadTexture(file+s[1]);
						registerTexture(mtlFile + "/" + s[1],mMtl.tex);
					//else if(ext.compare("mmp")==0)
					//	mMtl.tex=loadMMP( (char*)(file+s[1]).c_str());
				}
				else if(s[0].compare(0,2,"Kd")==0)
				{
					if(!mstring.compare("")) continue;
					try{
					float r = lexical_cast<float>(s[1]);
					float g = lexical_cast<float>(s[2]);
					float b = lexical_cast<float>(s[3]);
					mMtl.diffuse=color(r,g,b);
					}catch(...){}
				}
				else if(s[0].compare(0,2,"d")==0)
				{
					if(!mstring.compare("")) continue;
					mMtl.transparency=atof(s[1].c_str());
				}
				else if(s[0].compare(0,2,"Tr")==0)
				{
					if(!mstring.compare("")) continue;
					mMtl.transparency=1.0-atof(s[1].c_str());
				}
			}
			mMtl.name=mstring;
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
		return false;
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
		token = strtok_s(buffer, " \t", &nextToken);
		if(strcmp(token, "v") == 0)
		{
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &vertices[numVertices].x);
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &vertices[numVertices].y);
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &vertices[numVertices].z);
		
			vertices[numVertices].x = -vertices[numVertices].x;
			numVertices++;
		}
		else if(strcmp(token, "vt") == 0) 
		{
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &texCoords[numTexcoords].u);
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &texCoords[numTexcoords].v);
		
			texCoords[numTexcoords].v = 1.0f - texCoords[numTexcoords].v;
			numTexcoords++;
		}
		else if(strcmp(token, "vn") == 0) 
		{
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &normals[numNormals].x);
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &normals[numNormals].y);
			sscanf_s(strtok_s(NULL, " ", &nextToken), "%f", &normals[numNormals].z);
			numNormals++;
		}
		else if(strcmp(token, "f") == 0) 
		{
			int i, v = 0, t = 0, n = 0;
		
			for(i = 0; i<3; i++)
			{
				token = strtok_s(NULL, " \t",&nextToken);
				sscanf_s(token, "%d/%d/%d", &v, &t, &n);
		
				faces[numFaces].n[i] = n - 1;
				faces[numFaces].t[i] = t - 1;
				faces[numFaces].v[i] = v - 1;
			}
		
			Vec3f a, b;
		
			for(i = 0; i<3; i++) 
			{
				a[i] = vertices[faces[numFaces].v[0]][i]	- vertices[faces[numFaces].v[1]][i];
				b[i] = vertices[faces[numFaces].v[2]][i]	- vertices[faces[numFaces].v[1]][i];
			}
			faces[numFaces].material=cMtl;
			//normals[numNormals]=a.normalize().cross(b.normalize()).normalize();
			//numNormals++;
			numFaces++;
		}
		//else if(strcmp(token, "mtllib") == 0)  already loaded
		else if(strcmp(token, "usemtl") == 0)
		{
			string name=strtok_s(NULL, " ",&nextToken);
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
	int d = glGenLists(1);
	glNewList (d, GL_COMPILE);
	for(int pass=0;pass<=1;pass++)//pass 0 is opaque and pass 1 is translucent
	{
		if(pass==1)
		{
			glDepthMask(false);
		}
		for(int l=0; l<numMtls; l++)
		{
			glBindTexture(GL_TEXTURE_2D,mtls[l].tex);
			if(mtls[l].transparency>=0.999 && pass==0)
				glColor3f(mtls[l].diffuse.r,mtls[l].diffuse.g,mtls[l].diffuse.b);
			else if(mtls[l].transparency<0.999 && pass==1)
				glColor4f(mtls[l].diffuse.r,mtls[l].diffuse.g,mtls[l].diffuse.b,mtls[l].transparency);
			if(mtls[l].transparency>=0.999 && pass==0 || mtls[l].transparency<0.999 && pass==1)
			{
				glBegin(GL_TRIANGLES);
				for(int f=0;f<numFaces;f++)
				{
					if(faces[f].material==l)
					{
						if(mtls[faces[f].material].tex==0)
						{
							glNormal3fv(&normals[faces[f].n[0]].x);
							glVertex3fv(&vertices[faces[f].v[0]].x);

							glNormal3fv(&normals[faces[f].n[1]].x);
							glVertex3fv(&vertices[faces[f].v[1]].x);

							glNormal3fv(&normals[faces[f].n[2]].x);
							glVertex3fv(&vertices[faces[f].v[2]].x);
						}
						else
						{
					//		glNormal3fv(&normals[faces[f].n[0]].x);
							glTexCoord2fv(&texCoords[faces[f].t[0]].u);
							glVertex3fv(&vertices[	faces[f].v[0]].x);

					//		glNormal3fv(&normals[	faces[f].n[1]].x);
							glTexCoord2fv(&texCoords[faces[f].t[1]].u);
							glVertex3fv(&vertices[	faces[f].v[1]].x);
							
					//		glNormal3fv(&normals[	faces[f].n[2]].x);
							glTexCoord2fv(&texCoords[faces[f].t[2]].u);
							glVertex3fv(&vertices[	faces[f].v[2]].x);
						}
					}
				}
				glEnd();
			}
		}
	}
	glDepthMask(true);
	glEndList();

	//tmpModel.vertices = vertices;
	unsigned int* fs = new unsigned int[totalFaces*3];
	for(int itt = 0;itt<totalFaces;itt++)
	{
		fs[itt*3+0] = faces[itt].v[0];
		fs[itt*3+1] = faces[itt].v[1];
		fs[itt*3+2] = faces[itt].v[2];
	}
	models[d]=new CollisionChecker::triangleList(vertices,fs,totalVerts,totalFaces);

	delete[] fs;
	delete[] vertices;
	delete[] texCoords;
	delete[] faces;
	delete[] normals;
	delete[] mtls;
	
	return d;
}
int DataManager::loadPNG(string filename)
{
	png_uint_32		i, 
					width,
					height,
					rowbytes;
	int				bit_depth,
					color_type,
					colorChannels;
	unsigned char*	image_data;
	png_bytep*		row_pointers;
	
	/* Open the PNG file. */
	FILE *infile;
	fopen_s(&infile,filename.c_str(), "rb");
	
	if (!infile) {
		return 0;
	}
	
	unsigned char sig[8];
	/* Check for the 8-byte signature */
	fread(sig, 1, 8, infile);
	if (!png_check_sig((unsigned char *) sig, 8)) {
		fclose(infile);
		return 0;
	}
	/* 
	 * Set up the PNG structs 
	 */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(infile);
		return 0; /* out of memory */
	}
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(infile);
		return 0; /* out of memory */
	}
	
	png_infop end_ptr = png_create_info_struct(png_ptr);
	if (!end_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(infile);
		return 0; /* out of memory */
	}
	
	/*
	 * block to handle libpng errors, 
	 * then check whether the PNG file had a bKGD chunk
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		fclose(infile);
		return 0;
	}
	
	/*
	 * takes our file stream pointer (infile) and 
	 * stores it in the png_ptr struct for later use.
	 */
	png_init_io(png_ptr, infile);
	
	/*
	 * lets libpng know that we already checked the 8 
	 * signature bytes, so it should not expect to find 
	 * them at the current file pointer location
	 */
	png_set_sig_bytes(png_ptr, 8);
	
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	
	
	if (color_type == PNG_COLOR_TYPE_PALETTE)											png_set_expand(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)								png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))								png_set_expand(png_ptr);
	if (bit_depth == 16)																png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)	png_set_gray_to_rgb(png_ptr);
	
	/* snipped out the color type code, see source pngLoad.c */
	/* Update the png info struct.*/
	png_read_update_info(png_ptr, info_ptr);
	
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	colorChannels = (int)png_get_channels(png_ptr, info_ptr);
	
	if ((image_data = (unsigned char*)malloc(rowbytes*height)) == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 0;
	}
	if ((row_pointers = (png_bytep*)malloc(height*sizeof(png_bytep))) == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 0;
	}	
	for (i = 0;  i < height;  i++)
		row_pointers[i] = image_data + i*rowbytes;
	
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(infile);
	
	
	int format;
	if(colorChannels == 1)		format = GL_LUMINANCE;
	else if(colorChannels == 2)	format = GL_LUMINANCE_ALPHA;
	else if(colorChannels == 3) format = GL_RGB;
	else if(colorChannels == 4) format = GL_RGBA;
	
	bool NPOT = GLEE_ARB_texture_non_power_of_two && ((width & (width-1)) || (height & (height-1)));
	
	GLuint texV;
	glGenTextures(1,&texV);
	glBindTexture(GL_TEXTURE_2D, texV);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if(NPOT)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0, colorChannels, width, height,0, format, GL_UNSIGNED_BYTE, image_data);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, colorChannels, width, height, format, GL_UNSIGNED_BYTE, image_data);
	}
	
	free(image_data);
	free(row_pointers);
	return texV;
}
int DataManager::loadShader(string filename){return 0;}

void DataManager::bind(string name, int textureUnit)
{
	if(assets.find(name)==assets.end())
		return;
	else if(assets[name]->type==asset::TEXTURE)
	{
		if(boundTextures[textureUnit] == name)
			return;
		else if(boundTextureIds[textureUnit] == assets[name]->id)
		{
			boundTextures[textureUnit] = name;
			return;
		}

		if(activeTextureUnit != textureUnit)
		{
			glActiveTexture(GL_TEXTURE0+textureUnit);
			activeTextureUnit = textureUnit;
		}

		glBindTexture(GL_TEXTURE_2D,assets[name]->id);
		boundTextureIds[textureUnit] = assets[name]->id;
		boundTextures[textureUnit] = name;
	}
	else if(assets[name]->type==asset::SHADER)
	{
		//if(boundShader == name)
		//	return;
		//else if(boundShaderId == assets[name].id)
		//{
		//	boundShader = name;
		//	return;
		//}

		glUseProgram(assets[name]->id);
		boundShaderId = assets[name]->id;
		boundShader = name;
	}
}
void DataManager::bindTex(int id, int textureUnit)
{
	if(boundTextureIds[textureUnit] == id)
		return;

	if(activeTextureUnit != textureUnit)
	{
		glActiveTexture(GL_TEXTURE0+textureUnit);
		activeTextureUnit = textureUnit;
	}

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
				if(activeTextureUnit != i->first)
				{
					glActiveTexture(GL_TEXTURE0+i->first);
					activeTextureUnit = i->first;
				}
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
	if(boundTextureIds[activeTextureUnit] != 0)
	{
		glBindTexture(GL_TEXTURE_2D,0);
		boundTextureIds[activeTextureUnit] = 0;
		boundTextures[activeTextureUnit] = "noTexture";
	}

	for(unsigned int i = 0; i < boundTextureIds.size(); i++)
	{
		if(boundTextureIds[i] != 0)
		{
			if(activeTextureUnit != i)
			{
				glActiveTexture(GL_TEXTURE0+i);
				activeTextureUnit = i;
			}
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
void DataManager::draw(string name)
{
	if(assets.find(name)==assets.end() || assets[name]->type != asset::MODEL)
		return;
	if(activeTextureUnit != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		activeTextureUnit = 0;
	}
	bind("noTexture");
	bind("model");
	setUniform1i("tex",0);
	glCallList(assets[name]->id);
	unbind("model");
	glColor3f(1,1,1);
}
void DataManager::drawCustomShader(string name)
{
	if(assets.find(name)==assets.end() || assets[name]->type != asset::MODEL)
		return;
	if(activeTextureUnit != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		activeTextureUnit = 0;
	}
	bind("noTexture");
	glCallList(assets[name]->id);
	glColor3f(1,1,1);
}
void DataManager::draw(objectType p)
{
	if(p==F12)		draw("f12");
	if(p==F16)		draw("f16");
	if(p==F18)		draw("f18");
	if(p==F22)		draw("f22");
	if(p==UAV)		draw("UAV");
	if(p==B2)		draw("B2");

	if(p==MISSILE1)	draw("missile1");
	if(p==MISSILE2)	draw("missile2");
	if(p==MISSILE3)	draw("missile3");
	if(p==MISSILE4)	draw("missile4");
}
bool DataManager::loadAssetList()
{


	TiXmlDocument doc("media/assetList.xml");
	if(!doc.LoadFile())
	{
		return false;
	}

	const char* c;
	TiXmlNode* node					= NULL;
	TiXmlNode* assetsNode			= NULL;

	assetsNode = doc.FirstChild("assets");
	if(assetsNode == NULL) return false;

////////////////////////////////////////textures//////////////////////////////////////////
	node = assetsNode->FirstChild("textures");
	if(node != NULL)
	{
		TiXmlElement* texturesElement	= NULL;
		TiXmlElement* textureElement	= NULL;

		texturesElement = node->ToElement();
		if(texturesElement != NULL)
		{
			node = texturesElement->FirstChildElement();
			if(node != NULL) textureElement = node->ToElement();

			textureFile tmpTextureFile;
			while(textureElement != NULL)
			{
				c = textureElement->Attribute("name");	tmpTextureFile.name = c!=NULL ? c : "";
				c = textureElement->Attribute("file");	tmpTextureFile.filename = c!=NULL ? c : "";

				if(tmpTextureFile.name != "" && tmpTextureFile.filename != "")
				{
					const char* preload = textureElement->Attribute("preload");
					if(preload == NULL || string(preload) != "true")
						textureFiles.push(tmpTextureFile);
					else
						textureFilesPreload.push(tmpTextureFile);
				}
				else debugBreak();

				textureElement = textureElement->NextSiblingElement();
			}
		}
	}
////////////////////////////////////////shaders///////////////////////////////////////////
	node = assetsNode->FirstChild("shaders");
	if(node != NULL)
	{
		TiXmlElement* shadersElement	= NULL;
		TiXmlElement* shaderElement		= NULL;

		shadersElement = node->ToElement();
		if(shadersElement != NULL)
		{
			node = shadersElement->FirstChildElement();
			if(node != NULL) shaderElement = node->ToElement();

			shaderFile tmpShaderFile;
			while(shaderElement != NULL)
			{
				c = shaderElement->Attribute("name");		tmpShaderFile.name = c!=NULL ? c : "";
				c = shaderElement->Attribute("vertex");		tmpShaderFile.vertexShaderFile = c!=NULL ? c : "";
				c = shaderElement->Attribute("fragment");	tmpShaderFile.fragmentShaderFile = c!=NULL ? c : "";

				const char* use_sAspect = shaderElement->Attribute("sAspect");
				tmpShaderFile.use_sAspect = (use_sAspect != NULL && string(use_sAspect) == "true");

				if(tmpShaderFile.name != "" && tmpShaderFile.vertexShaderFile != "" && tmpShaderFile.fragmentShaderFile != "")
				{
					const char* preload = shaderElement->Attribute("preload");
					if(preload == NULL || string(preload) != "true")
						shaderFiles.push(tmpShaderFile);
					else
						shaderFilesPreload.push(tmpShaderFile);
				}
				else debugBreak();
				shaderElement = shaderElement->NextSiblingElement();
			}
		}
	}
////////////////////////////////////////models///////////////////////////////////////////
	node = assetsNode->FirstChild("models");
	if(node != NULL)
	{
		TiXmlElement* modelsElement		= NULL;
		TiXmlElement* modelElement		= NULL;

		modelsElement = node->ToElement();
		if(modelsElement != NULL)
		{
			node = modelsElement->FirstChildElement();
			if(node != NULL) modelElement = node->ToElement();

			modelFile tmpModelFile;
			while(modelElement != NULL)
			{
				c = modelElement->Attribute("name");	tmpModelFile.name = c!=NULL ? c : "";
				c = modelElement->Attribute("file");	tmpModelFile.filename = c!=NULL ? c : "";
				
				if(tmpModelFile.name !="" && tmpModelFile.filename != "")
					modelFiles.push(tmpModelFile);
				else
					debugBreak();

				modelElement = modelElement->NextSiblingElement();
			}
		}
	}

	return true;
}
void DataManager::preloadAssets()
{
	while(!textureFilesPreload.empty())
	{
		textureFile i = textureFilesPreload.front();
		textureFilesPreload.pop();

		registerAsset(i.name, i.filename);
	}

	while(!shaderFilesPreload.empty())
	{
		shaderFile i = shaderFilesPreload.front();
		shaderFilesPreload.pop();

		registerShader(i.name, i.vertexShaderFile, i.fragmentShaderFile);
	}
}
int DataManager::loadAsset()
{
	if(!textureFiles.empty())
	{
		textureFile i = textureFiles.front();
		textureFiles.pop();

		registerAsset(i.name, i.filename);

		return textureFiles.size() + shaderFiles.size() + modelFiles.size();
	}
	else if(!shaderFiles.empty())
	{
		shaderFile i = shaderFiles.front();
		shaderFiles.pop();

		registerShader(i.name, i.vertexShaderFile, i.fragmentShaderFile);

		if(i.use_sAspect)
		{
			auto s = assets.find(i.name);
			if(s != assets.end())
				((shaderAsset*)(s->second))->use_sAspect = true;

			bind(i.name);
			setUniform1f("sAspect",sAspect);
			unbind(i.name);
		}

		return shaderFiles.size() + modelFiles.size();
	}
	else if(!modelFiles.empty())
	{
		modelFile i = modelFiles.front();
		modelFiles.pop();

		registerAsset(i.name, i.filename);

		return modelFiles.size();
	}
	return 0;
}
//int DataManager::registerAssets()
//{
	////static ofstream cout("loading.txt");

	//static int callNum=0;
	//callNum++;
	//int n=1;

	////cout << "loading asset #" + lexical_cast<string>(callNum) + "... ";

	//if(callNum==n++)	textManager->init("media/ascii");//needed for error messages
	//if(callNum==n++)	registerAsset("dialog box",			"media/dialog box.png");

	//if(callNum==n++)	registerTexture("noTexture",		0);
	//if(callNum==n++)	registerShader("noShader",			0);	
	//if(callNum==n++)	registerAsset("white",				"media/white.png");	
	//if(callNum==n++)	registerAsset("grass",				"media/grass.png");
	//if(callNum==n++)	registerAsset("rock",				"media/rock.png");
	//if(callNum==n++)	registerAsset("sand",				"media/sand.png");
	//if(callNum==n++)	registerAsset("snow",				"media/snow.png");
	//if(callNum==n++)	registerAsset("LCnoise",			"media/LCnoise.png");
	//if(callNum==n++)	registerAsset("radarTex",			"media/radar2.png");
	//if(callNum==n++)	registerAsset("particle",			"media/particle4.png");	
	//if(callNum==n++)	registerAsset("radar plane",		"media/plane radar2.png");
	//if(callNum==n++)	registerAsset("hardNoise",			"media/hardNoiseDOT3.png");
	////if(callNum==n++)	registerAsset("explosion core",		"media/explosion/core.png");
	//if(callNum==n++)	registerAsset("explosion fireball",	"media/explosion/fireball4.png");
	////if(callNum==n++)	registerAsset("explosion flash",	"media/explosion/flash.png");
	//if(callNum==n++)	registerAsset("explosion smoke",	"media/explosion/smoke5.png");
	//if(callNum==n++)	registerAsset("cockpit",			"media/cockpit.png");
	//if(callNum==n++)	registerAsset("health bar",			"media/health bar.png");
	//if(callNum==n++)	registerAsset("tilt",				"media/tilt.png");
	//if(callNum==n++)	registerAsset("targeter",			"media/targeter.png");
	//if(callNum==n++)	registerAsset("missile smoke",		"media/particle8.png");
	//if(callNum==n++)	registerAsset("key",				"media/key.png");
	//if(callNum==n++)	registerAsset("next level",			"media/nextLevel.png");
	//if(callNum==n++)	registerAsset("radar frame",		"media/radar_frame.png");
	//if(callNum==n++)	registerAsset("cockpit square",		"media/cockpit square.png");
	//if(callNum==n++)	registerAsset("button",				"media/button.png");
	//if(callNum==n++)	registerAsset("file viewer",		"media/file viewer.png");
	//if(callNum==n++)	registerAsset("entry bar",			"media/entry bar.png");
	//if(callNum==n++)	registerAsset("target ring",		"media/target ring.png");
	//if(callNum==n++)	registerAsset("smoke",				"media/particles/smoke.png");
	//if(callNum==n++)	registerAsset("fire",				"media/fire.png");
	//if(callNum==n++)	registerAsset("hex grid",			"media/hexGrid.png");
	//if(callNum==n++)	registerAsset("bullet",				"media/bullet.png");

	//if(callNum==n++)	registerAsset("glow",				"media/glow.png");
	//if(callNum==n++)	registerAsset("cursor",				"media/cursor.png");
	//if(callNum==n++)	registerAsset("layers",				"media/layers.png");
	//if(callNum==n++)	registerAsset("check",				"media/check.png");
	//if(callNum==n++)	registerAsset("check box",			"media/check box.png");
	//if(callNum==n++)	registerAsset("slider",				"media/slider.png");
	//if(callNum==n++)	registerAsset("slider bar",			"media/slider bar.png");

	////if(callNum==n++)	registerAsset("tree top",			"media/tree/top.png");
	////if(callNum==n++)	registerAsset("tree right",			"media/tree/right.png");
	////if(callNum==n++)	registerAsset("tree front",			"media/tree/front.png");

	////if(callNum==n++)	registerAsset("menu background",	"media/menu/menu background2.tga"); registered earlier in loading
	//if(callNum==n++)	registerAsset("menu start",			"media/menu/start.png");
	//if(callNum==n++)	registerAsset("menu slot",			"media/menu/slot.png");
	//if(callNum==n++)	registerAsset("menu mode choices",	"media/menu/mode choices.png");
	//if(callNum==n++)	registerAsset("menu pictures",		"media/menu/choice pics.png");

	//if(callNum==n++)	registerAsset("menu in game",		"media/menu/in game.png");
	//if(callNum==n++)	registerAsset("menu in game select","media/menu/in game select.png");

	//if(callNum==n++)	registerShader("grass terrain",		"media/toon.vert","media/toon.frag");
	//if(callNum==n++)	registerShader("radar",				"media/radar.vert","media/radar.frag");
	//if(callNum==n++)	registerShader("radar2",			"media/radar2.vert","media/radar2.frag");
	//if(callNum==n++)	registerShader("radar plane shader","media/radarPlane.vert","media/radarPlane.frag");
	////if(callNum==n++)	registerShader("water",				"media/water.vert","media/water.frag");
	//if(callNum==n++)	registerShader("sea floor",			"media/sea floor.vert","media/sea floor.frag");
	//if(callNum==n++)	registerShader("horizon",			"media/horizon.vert","media/horizon.frag");//was water2
	//if(callNum==n++)	registerShader("horizon2",			"media/horizon2.vert","media/horizon2.frag");//was water2
	//if(callNum==n++)	registerShader("island terrain",	"media/island.vert","media/island.frag");
	//if(callNum==n++)	registerShader("rock terrain",		"media/rock.vert","media/rock.frag");
	//if(callNum==n++)	registerShader("health",			"media/health.vert","media/health.frag");
	//if(callNum==n++)	registerShader("ocean",				"media/ocean.vert","media/ocean.frag");
	//if(callNum==n++)	registerShader("partical shader",	"media/smoke.vert","media/smoke.frag");
	//if(callNum==n++)	registerShader("model",				"media/model.vert","media/model.frag");
	//if(callNum==n++)	registerShader("ortho",				"media/ortho.vert","media/ortho.frag");
	//if(callNum==n++)	registerShader("hex grid shader",	"media/hex grid.vert","media/hex grid.frag");
	//if(callNum==n++)	registerShader("sky shader",		"media/sky.vert","media/sky.frag");
	////if(callNum==n++)	registerAsset("island new terrain",	"media/terrain.frag");
	//if(callNum==n++)	registerAsset("island new terrain",	"media/grass.frag");
	//if(callNum==n++)	registerAsset("grass new terrain",	"media/grass2.frag");
	//if(callNum==n++)	registerAsset("snow terrain",		"media/snow.frag");

	//if(callNum==n++)	registerAsset("sky dome",			"media/dome.obj");//was 'dome4.obj'
	//if(callNum==n++)	registerAsset("cylinder",			"media/cylinder.obj");
	//if(callNum==n++)	registerAsset("f16",				"media/f16.obj");
	//if(callNum==n++)	registerAsset("f18",				"media/f18hornet.obj");
	//if(callNum==n++)	registerAsset("f22",				"media/f22.obj");
	//if(callNum==n++)	registerAsset("UAV",				"media/UAV.obj");
	//if(callNum==n++)	registerAsset("B2",					"media/B2.obj");
	//if(callNum==n++)	registerAsset("AA gun",				"media/AAgun.obj");

	//if(callNum==n++)	registerAsset("missile1",			"media/m1(center).obj");
	//if(callNum==n++)	registerAsset("missile2",			"media/m2(center).obj");
	//if(callNum==n++)	registerAsset("missile3",			"media/m3(center).obj");
	//if(callNum==n++)	registerAsset("missile4",			"media/m4(center).obj");

	////if(callNum==n++)	registerAsset("AA gun",				"media/AAGun_Mobile_01.obj"); <---- WE HAVE NO MODEL YET!!!
	////				.					.
	////				.					.
	////				.					.
	//if(callNum==n++)	settings.load("media/modelData.txt");
	////cout << "loading complete" << endl;

	//return (n-1)-callNum;//number left


//	return 0;
//}
void DataManager::registerAsset(string name, string filename)
{//shaders must be registered by hand right now
	if(assets.find(name) != assets.end()) 
		messageBox(string("name clash: ") + name);

	string ext=filesystem::extension(filename);
	if(ext.compare(".tga") == 0)		registerTexture(name,loadTGA(filename));
	else if(ext.compare(".mmp") == 0)	registerTexture(name,loadMMP(filename));
	else if(ext.compare(".png") == 0)	registerTexture(name,loadPNG(filename));
	else if(ext.compare(".obj") == 0)	registerModel(name,loadOBJ(filename));
	else if(ext.compare(".frag") == 0)	registerShader(name,loadTerrainShader(filename));
}
void DataManager::registerTexture(string name, int id)
{
	asset* a = new asset;
	a->id = id;
	a->type = asset::TEXTURE;

	assets[name] = a;
}
void DataManager::registerShader(string name, int id)
{
	shaderAsset* a = new shaderAsset;
	a->id = id;
	a->type = asset::SHADER;
	a->use_sAspect = false;

	assets[name] = a;
}
void DataManager::registerShader(string name, string vert, string frag)
{
	shaderAsset* a = new shaderAsset;
	a->id = loadShader(vert,frag);
	a->type = asset::SHADER;
	a->use_sAspect = false;

	assets[name] = a;
}
void DataManager::registerModel(string name, int id)
{
	asset* a = new asset;
	a->id = id;
	a->type = asset::MODEL;
	assets[name] = a;
}
int DataManager::getId(string name)
{
	if(assets.find(name)==assets.end())
		return 0;
	return assets[name]->id;
}
int DataManager::getId(objectType t)
{
	if(t==F12)	return getId("f12");
	if(t==F16)	return getId("f16");
	if(t==F18)	return getId("f18");
	if(t==F22)	return getId("f22");
	if(t==UAV)	return getId("UAV");
	if(t==B2)	return getId("B2");

	if(t==MISSILE1) return getId("missile1");
	if(t==MISSILE2) return getId("missile2");
	if(t==MISSILE3) return getId("missile3");
	if(t==MISSILE4) return getId("missile4");
	return 0;
}
CollisionChecker::triangleList* DataManager::getModel(objectType type)
{
	auto i = models.find(getId(type));
	return i != models.end() ? i->second : NULL;
}
char* DataManager::textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) 
	{
		fopen_s(&fp,fn,"rt");

		if (fp != NULL) 
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				memset(content,0,sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
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

void DataManager::shutdown()
{
	boundShader = "";
	boundTextures.clear();
	for(auto i = assets.begin(); i != assets.end(); i++)
	{
		if(i->second->type == asset::SHADER)			glDeleteProgram(i->second->id);
		else if(i->second->type == asset::MODEL)		glDeleteLists(i->second->id,1);
		else if(i->second->type == asset::TEXTURE)	glDeleteTextures(1,(const GLuint*)&i->second->id);
	}
	assets.clear();
	for(auto i = models.begin(); i!=models.end();i++)
	{
		i->second->~triangleList();
	}
	models.clear();
}