
#include "png\png.h"
#include "main.h"


DataManager::~DataManager()
{
	for(map<int,CollisionChecker::triangleList>::iterator i = models.begin(); i!=models.end();i++)
	{
		i->second.~triangleList();
	}
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
	if(ext.compare(".obj") == 0)	return loadOBJ(filename);
	return 0;
}
int DataManager::loadShader(string vert, string frag)
{
	GLuint v=0,f=0,p;
	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	char * ff = textFileRead((char*)frag.c_str());
	char * vv = textFileRead((char*)vert.c_str());
	if(ff == NULL || vv == NULL) return 0;

	glShaderSource(v, 1, (const char **)&vv, NULL);
	glShaderSource(f, 1, (const char **)&ff, NULL);
	glCompileShader(v);
	glCompileShader(f);
	free(ff);
	free(vv);

	char* cv=(char*)malloc(512); memset(cv,0,512);
	char* cf=(char*)malloc(512); memset(cf,0,512);
	int lv,lf;
	glGetShaderInfoLog(v,512,&lv,cv);
	glGetShaderInfoLog(f,512,&lf,cf);

	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);

	glLinkProgram(p);
	glUseProgram(0); 

	if(lv != 0)		messageBox(string(vert)+": "+string(cv));
	if(lf != 0) 	messageBox(string(frag)+": "+string(cf));
	//if(lv != 0) MessageBoxA(NULL,(string(vert)+": "+string(cv)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
	//if(lf != 0) MessageBoxA(NULL,(string(frag)+": "+string(cf)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
	return p;
}
int DataManager::loadTerrainShader(string frag)
{
	static GLuint v=0;
	

	if(v==0)
	{
		v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		char * vv = textFileRead("media/terrain.vert");
		if(vv == NULL) return 0;
		glShaderSource(v, 1, (const char **)&vv, NULL);
		glCompileShader(v);
		free(vv);
		int lv;
		char* cv=(char*)malloc(512); memset(cv,0,512);
		glGetShaderInfoLog(v,512,&lv,cv);
		if(lv != 0){
			//MessageBoxA(NULL,(string("media/terrain.vert")+"f: "+string(cv)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
			messageBox(string("media/terrain.vert")+"f: "+string(cv));
			return 0;
		}
	}

	GLuint	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB),
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

		p = glCreateProgram();
		glAttachShader(p,f);
		glAttachShader(p,v);
		glLinkProgram(p);
	}
	glUseProgram(0);

	if(lf != 0){
		//MessageBoxA(NULL,(string(frag)+": "+string(cf)).c_str(), "Shader Error",MB_ICONEXCLAMATION);
		messageBox(string(frag)+": "+string(cf));
		return 0;
	}
	return p;
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
	NPOT = GLEE_ARB_texture_non_power_of_two && (texture.width & (texture.width-1)) && (texture.height & (texture.height-1));
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
	struct mtl{int tex;color diffuse;float transparency;string name;mtl(): tex(0), diffuse(1,1,1), transparency(1.0){}};
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
					if(mstring.compare("")!=0)
					{
						mMtl.name=mstring;
						m.insert(pair<string,mtl>(mstring,mMtl));
					}
					mMtl=mtl();
					mstring=s[1];
				}
				else if(s[0].compare(0,6,"map_Kd")==0)
				{
					assert(mstring.compare(""));
					//string ext=(file+s[1]).substr((file+s[1]).find_last_of(".")+1);
					//if(ext.compare("tga")==0)
						mMtl.tex=dataManager.loadTexture(file+s[1]);
					//else if(ext.compare("mmp")==0)
					//	mMtl.tex=loadMMP( (char*)(file+s[1]).c_str());
				}
				else if(s[0].compare(0,2,"Kd")==0)
				{
					assert(mstring.compare(""));
					mMtl.diffuse=color(atof(s[1].c_str()),atof(s[2].c_str()),atof(s[3].c_str()));
				}
				else if(s[0].compare(0,2,"d")==0 || s[0].compare(0,2,"Tr")==0)
				{
					assert(mstring.compare(""));
					mMtl.transparency=atof(s[1].c_str());
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
	int d = glGenLists (1);
	glNewList (d, GL_COMPILE);
	for(int pass=0;pass<=1;pass++)//pass 0 is opaque and pass 1 is translucent
	{
		if(pass==1)
		{
			glEnable(GL_BLEND);
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
							glNormal3fv(&normals[faces[f].n[0]].x);
							glTexCoord2fv(&texCoords[faces[f].t[0]].u);
							glVertex3fv(&vertices[	faces[f].v[0]].x);

							glNormal3fv(&normals[	faces[f].n[1]].x);
							glTexCoord2fv(&texCoords[faces[f].t[1]].u);
							glVertex3fv(&vertices[	faces[f].v[1]].x);
							
							glNormal3fv(&normals[	faces[f].n[2]].x);
							glTexCoord2fv(&texCoords[faces[f].t[2]].u);
							glVertex3fv(&vertices[	faces[f].v[2]].x);
						}
					}
				}
				glEnd();
			}
		}
	}
	glDisable(GL_BLEND);
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
	CollisionChecker::triangleList tmpModel(vertices,fs,totalVerts,totalFaces);
	models[d]=tmpModel;

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
	for (i = 0;  i < height;  ++i)
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

	bool NPOT = GLEE_ARB_texture_non_power_of_two && !((width & (width-1)) && (height & (height-1)));
	glError()

	GLuint texV;
	glGenTextures(1,&texV);glError();
    glBindTexture(GL_TEXTURE_2D, texV);glError();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);glError();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);glError();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);glError();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	glError();
	if(NPOT)	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);glError();
	

	if(NPOT)	glTexImage2D(GL_TEXTURE_2D,0, colorChannels, width, height,0, format, GL_UNSIGNED_BYTE, image_data);
	else		gluBuild2DMipmaps(GL_TEXTURE_2D, colorChannels, width, height, format, GL_UNSIGNED_BYTE, image_data);glError();
	
    free(image_data);
	free(row_pointers);
    return texV;
}
int DataManager::loadShader(string filename){return 0;}

void DataManager::bind(string name, int textureUnit)
{
	if(assets.find(name)==assets.end())
		return;
	else if(assets[name].type==asset::TEXTURE)
	{
		glActiveTexture(GL_TEXTURE0+textureUnit);
		glBindTexture(GL_TEXTURE_2D,assets[name].id);
		boundTextures[textureUnit] = name;
	}
	else if(assets[name].type==asset::SHADER)
	{
		glUseProgram(assets[name].id);
		boundShader = name;
	}
}
void DataManager::bindTex(int id, int textureUnit)
{
	glActiveTexture(GL_TEXTURE0+textureUnit);
	glBindTexture(GL_TEXTURE_2D,id);
	boundTextures[textureUnit] = "";
}
void DataManager::bindShader(int id)
{
	glUseProgram(id);
	boundShader = "";
}
void DataManager::unbind(string name)
{
	if(boundShader.compare(name) == 0)
	{
		glUseProgram(0);
		boundShader = "";
	}
	else
	{
		for(map<int,string>::iterator i = boundTextures.begin();i!=boundTextures.end();i++)
		{
			if(i->second.compare(name) == 0)
			{
				glActiveTexture(GL_TEXTURE0+i->first);
				glBindTexture(GL_TEXTURE_2D,0);
				return;
			}
		}
	}
}
void DataManager::unbindTextures()
{
	for(map<int,string>::iterator i = boundTextures.begin();i!=boundTextures.end();i++)
	{
		glActiveTexture(GL_TEXTURE0+i->first);
		glBindTexture(GL_TEXTURE_2D,0);
	}
	boundTextures.end();
}
void DataManager::unbindShader()
{
	glUseProgram(0);
	boundShader = "";
}
void DataManager::draw(string name)
{
	if(assets.find(name)==assets.end() || assets[name].type != asset::MODEL)
		return;
	glCallList(assets[name].id);
}
void DataManager::draw(planeType p)
{
	if(p==F12)	draw("f12");
	if(p==F16)	draw("f16");
	if(p==F18)	draw("f18");
	if(p==F22)	draw("f22");
	if(p==UAV)	draw("UAV");
	if(p==B2)	draw("B2");
}
int DataManager::registerAssets()
{
	static int callNum=0;
	callNum++;
	int n=1;

	if(callNum==n++)	registerTexture("noTexture",		0);
	if(callNum==n++)	registerShader("noShader",			0);
	if(callNum==n++)	registerAsset("grass",				"media/grass.png");
	if(callNum==n++)	registerAsset("rock",				"media/rock.png");
	if(callNum==n++)	registerAsset("sand",				"media/sand.png");
	if(callNum==n++)	registerAsset("snow",				"media/snow.png");
	if(callNum==n++)	registerAsset("LCnoise",			"media/LCnoise.png");
	//if(callNum==n++)	registerAsset("aimer",				"media/aimer.tga");
	//if(callNum==n++)	registerAsset("dialFront",			"media/dial front.tga");
	//if(callNum==n++)	registerAsset("dialBAck",			"media/dial back.tga");	
	//if(callNum==n++)	registerAsset("dialSpeed",			"media/speed2.tga");
	//if(callNum==n++)	registerAsset("dialAltitude",		"media/altitude.tga");
	//if(callNum==n++)	registerAsset("needle",				"media/needle2.tga");	
	if(callNum==n++)	registerAsset("radarTex",			"media/radar2.png");
	if(callNum==n++)	registerAsset("particle",			"media/particle4.png");	
	if(callNum==n++)	registerAsset("radar plane",		"media/plane radar2.png");
	if(callNum==n++)	registerAsset("hardNoise",			"media/hardNoiseDOT3.png");
	//if(callNum==n++)	registerAsset("explosion core",		"media/explosion/core.png");
	if(callNum==n++)	registerAsset("explosion fireball",	"media/explosion/fireball4.png");
	//if(callNum==n++)	registerAsset("explosion flash",	"media/explosion/flash.png");
	if(callNum==n++)	registerAsset("explosion smoke",	"media/explosion/smoke5.png");
	if(callNum==n++)	registerAsset("cockpit",			"media/cockpit.png");
	if(callNum==n++)	registerAsset("health bar",			"media/health bar.png");
	if(callNum==n++)	registerAsset("tilt",				"media/tilt.png");
	if(callNum==n++)	registerAsset("targeter",			"media/targeter.png");
	//if(callNum==n++)	registerAsset("speed",				"media/speed2.tga");
	//if(callNum==n++)	registerAsset("altitude",			"media/altitude.tga");
	if(callNum==n++)	registerAsset("missile smoke",		"media/particle8.png");
	if(callNum==n++)	registerAsset("key",				"media/key.png");
	if(callNum==n++)	registerAsset("next level",			"media/nextLevel.png");
	if(callNum==n++)	registerAsset("radar frame",		"media/radar_frame.png");
	if(callNum==n++)	registerAsset("cockpit square",		"media/cockpit square.png");
	if(callNum==n++)	registerAsset("button",				"media/button.png");
	if(callNum==n++)	registerAsset("file viewer",		"media/file viewer.png");
	if(callNum==n++)	registerAsset("entry bar",			"media/entry bar.png");
	if(callNum==n++)	registerAsset("dialog box",			"media/dialog box.png");
	if(callNum==n++)	registerAsset("glow",				"media/glow.png");
	if(callNum==n++)	registerAsset("cursor",				"media/cursor.png");
	if(callNum==n++)	registerAsset("layers",				"media/layers.png");
	if(callNum==n++)	registerAsset("check",				"media/check.png");
	if(callNum==n++)	registerAsset("check box",			"media/check box.png");

	//if(callNum==n++)	registerAsset("tree top",			"media/tree/top.png");
	//if(callNum==n++)	registerAsset("tree right",			"media/tree/right.png");
	//if(callNum==n++)	registerAsset("tree front",			"media/tree/front.png");

	//if(callNum==n++)	registerAsset("menu background",	"media/menu/menu background2.tga"); registered earlier in loading
	if(callNum==n++)	registerAsset("menu start",			"media/menu/start.png");
	if(callNum==n++)	registerAsset("menu slot",			"media/menu/slot.png");
	if(callNum==n++)	registerAsset("menu mode choices",	"media/menu/mode choices.png");
	if(callNum==n++)	registerAsset("menu pictures",		"media/menu/choice pics.png");


	if(callNum==n++)	registerShader("grass terrain",		"media/toon.vert","media/toon.frag");
	if(callNum==n++)	registerShader("radar",				"media/radar.vert","media/radar.frag");
	if(callNum==n++)	registerShader("radar2",			"media/radar2.vert","media/radar2.frag");
	if(callNum==n++)	registerShader("water",				"media/water.vert","media/water.frag");
	if(callNum==n++)	registerShader("sea floor",			"media/sea floor.vert","media/sea floor.frag");
	if(callNum==n++)	registerShader("horizon",			"media/horizon.vert","media/horizon.frag");//was water2
	if(callNum==n++)	registerShader("horizon2",			"media/horizon2.vert","media/horizon2.frag");//was water2
	if(callNum==n++)	registerShader("island terrain",	"media/island.vert","media/island.frag");
	if(callNum==n++)	registerShader("rock terrain",		"media/rock.vert","media/rock.frag");
	if(callNum==n++)	registerShader("health",			"media/health.vert","media/health.frag");
	if(callNum==n++)	registerShader("ocean",				"media/ocean.vert","media/ocean.frag");
	if(callNum==n++)	registerAsset("island new terrain",	"media/terrain.frag");
	if(callNum==n++)	registerAsset("grass new terrain",	"media/grass.frag");

	if(callNum==n++)	registerAsset("sky dome",			"media/dome4.obj");
	if(callNum==n++)	registerAsset("f16",				"media/f16.obj");
	if(callNum==n++)	registerAsset("f18",				"media/f18hornet.obj");
	if(callNum==n++)	registerAsset("f22",				"media/f22.obj");
	if(callNum==n++)	registerAsset("UAV",				"media/UAV.obj");
	if(callNum==n++)	registerAsset("B2",					"media/B2.obj");
	//				.					.
	//				.					.
	//				.					.
	if(callNum==n++)	settings.loadModelData("media/modelData.txt");
	if(callNum==n++)	textManager->init("media/ascii");

	return (n-1)-callNum;//number left
}
void DataManager::registerAsset(string name, string filename)
{//shaders must be registered by hand right now
	if(assets.find(name) != assets.end()) messageBox(string("name clash: ") + name);
	string ext=filesystem::extension(filename);
	if(ext.compare(".tga") == 0)		registerTexture(name,loadTGA(filename));
	else if(ext.compare(".mmp") == 0)	registerTexture(name,loadMMP(filename));
	else if(ext.compare(".png") == 0)	registerTexture(name,loadPNG(filename));
	else if(ext.compare(".obj") == 0)	registerModel(name,loadOBJ(filename));
	else if(ext.compare(".frag") == 0)	registerShader(name,loadTerrainShader(filename));
}
void DataManager::registerTexture(string name, int id)
{
	asset a;
	a.id = id;
	a.type = asset::TEXTURE;
	assets.insert(pair<string,asset>(name,a));
}
void DataManager::registerShader(string name, int id)
{
	asset a;
	a.id = id;
	a.type = asset::SHADER;
	assets.insert(pair<string,asset>(name,a));
}
void DataManager::registerShader(string name, string vert, string frag)
{
	asset a;
	a.id = loadShader(vert,frag);
	a.type = asset::SHADER;
	assets.insert(pair<string,asset>(name,a));
}
void DataManager::registerModel(string name, int id)
{
	asset a;
	a.id = id;
	a.type = asset::MODEL;
	assets.insert(pair<string,asset>(name,a));
}
int DataManager::getId(string name)
{
	if(assets.find(name)==assets.end())
		return 0;
	return assets[name].id;
}
int DataManager::getId(objectType t)
{
	if(t==F12)	return getId("f12");
	if(t==F16)	return getId("f16");
	if(t==F18)	return getId("f18");
	if(t==F22)	return getId("f22");
	if(t==UAV)	return getId("UAV");
	if(t==B2)	return getId("B2");
	return 0;
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