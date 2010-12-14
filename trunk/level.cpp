
#include "main.h"

void LevelFile::load(string filename)
{
	if(info != NULL) delete info;
	if(objects != NULL) delete objects;
	if(heights != NULL) delete heights;

	ifstream fin(filename, ios::binary);
	fin.read((char*)&header,sizeof(Header));
	if(header.magicNumber == 0x454c49465f4c564c && header.version == 1)
	{
		info = new Info;
		fin.read((char*)info,sizeof(Info));
		objects = new Object[info->numObjects];
		for(int i=0;i<info->numObjects;i++)
			fin.read((char*)&objects[i],sizeof(Object));
		heights = new float[info->mapResolution.x*info->mapResolution.y];
		fin.read((char*)heights,info->mapResolution.x*info->mapResolution.y*sizeof(float));
	}
	fin.close();
}
void LevelFile::save(string filename)
{
	ofstream fout(filename, ios::binary);
	fout.write((char*)&header,sizeof(Header));
	if(header.version == 1 && info!=NULL && heights!=NULL && (objects!=NULL || info->numObjects==0))
	{
		fout.write((char*)info,sizeof(Info));
		for(int i=0;i<info->numObjects;i++)
			fout.write((char*)&objects[i],sizeof(Object));
		fout.write((char*)heights,info->mapResolution.x*info->mapResolution.y*sizeof(float));
	}
	fout.close();
}
LevelFile::LevelFile(): info(NULL), heights(NULL)
{
	header.magicNumber = 0x454c49465f4c564c;
	header.version = 0;
}
LevelFile::LevelFile(string filename): info(NULL), heights(NULL)
{
	header.magicNumber = 0x454c49465f4c564c;
	header.version = 0;
	load(filename);
}
//_______________________________________________________________________________________________________________________________________________________________
float Level::heightmapBase::getRasterHeight(unsigned int x,unsigned int z) const
{
	return heights[clamp(x,0,getResolutionX()-1) + clamp(z,0,getResolutionZ()-1)*getResolutionX()];
}
float Level::heightmapBase::getHeight(float x, float z) const
{	//    A------D		.-----x+
	//	  |      |		|
	//    |      |		|
	//    B------C		z+

	x -= position.x;
	z -= position.z;
	x *= resolution.x/size.x;
	z *= resolution.y/size.z;

	if(x-floor(x)+z-floor(z)<1.0)
	{
		float A = position.y+size.y*heights[clamp((int)floor(x),0,getResolutionX()-1) + clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX()];
		float B = position.y+size.y*heights[clamp((int)floor(x),0,getResolutionX()-1) + clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX()];
		float D = position.y+size.y*heights[clamp((int)floor(x)+1,0,getResolutionX()-1) + clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX()];
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x));
	}
	else
	{
		float B = position.y+size.y*heights[clamp((int)floor(x),0,getResolutionX()-1) + clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX()];
		float C = position.y+size.y*heights[clamp((int)floor(x)+1,0,getResolutionX()-1) + clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX()];
		float D = position.y+size.y*heights[clamp((int)floor(x)+1,0,getResolutionX()-1) + clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX()];
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}

Vec3f Level::heightmapBase::getRasterNormal(unsigned int h, unsigned int k) const
{
	unsigned int x=h; unsigned int z=k;
	float Cy = (z < getResolutionZ()-1)	? ((float)heights[x	+(z+1)*getResolutionX()	]	- heights[x+z*getResolutionX()])/255.0  : 0.0f;
	float Ay = (x < getResolutionX()-1)	? ((float)heights[(x+1)+z*getResolutionX()	]	- heights[x+z*getResolutionX()])/255.0  : 0.0f;
	float Dy = (  z > 0	)				? ((float)heights[x	+(z-1)*getResolutionX()	]	- heights[x+z*getResolutionX()])/255.0  : 0.0f;
	float By = (  x > 0	)				? ((float)heights[(x-1)+z*getResolutionX()	]	- heights[x+z*getResolutionX()])/255.0  : 0.0f;
	//if(z >= getResolutionZ()-1) Cy=Dy;
	//if(x >= getResolutionX()-1) Ay=By;
	//if(z <= 0) Dy = Cy;
	//if(x <= 0) By = Ay;
	return Vec3f(Cy - Ay, 3.0 / 15.0, Dy - By).normalize();	
}
Vec3f Level::heightmapBase::getNormal(float x, float z) const
{	//    A------D		.-----x+
	//	  |      |		|
	//    |      |		|
	//    B------C		z+

	x -= position.x;
	z -= position.z;
	x *= resolution.x/size.x;
	z *= resolution.y/size.z;

	if(x-floor(x)+z-floor(z)<1.0)
	{
		Vec3f A = getRasterNormal(clamp((int)floor(x),0,getResolutionX()-1), clamp((int)floor(z),0,getResolutionZ()-1));
		Vec3f B = getRasterNormal(clamp((int)floor(x),0,getResolutionX()-1), clamp((int)floor(z)+1,0,getResolutionZ()-1));
		Vec3f D = getRasterNormal(clamp((int)floor(x)+1,0,getResolutionX()-1), clamp((int)floor(z),0,getResolutionZ()-1));
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x)).normalize();
	}
	else
	{
		Vec3f B = getRasterNormal(clamp((int)floor(x),0,getResolutionX()-1), clamp((int)floor(z)+1,0,getResolutionZ()-1));
		Vec3f C = getRasterNormal(clamp((int)floor(x)+1,0,getResolutionX()-1), clamp((int)floor(z)+1,0,getResolutionZ()-1));
		Vec3f D = getRasterNormal(clamp((int)floor(x)+1,0,getResolutionX()-1), clamp((int)floor(z),0,getResolutionZ()-1));
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z))).normalize();
	}
}
Level::heightmapBase::heightmapBase(Vec2u Resolution): resolution(Resolution), position(0,0,0), size(Resolution.x,1,Resolution.y), heights(NULL)
{
	heights = new float[resolution.x*resolution.y];

}
Level::heightmapBase::heightmapBase(Vec2u Resolution, float* hts): resolution(Resolution), position(0,0,0), size(Resolution.x,1,Resolution.y), heights(NULL)
{
	heights = new float[resolution.x*resolution.y];
	memcpy(heights,hts,resolution.x*resolution.y*sizeof(float));
}
void Level::heightmapGL::init()
{
	glGenTextures(1,(GLuint*)&groundTex);
	groundValues = (unsigned char*)malloc(uPowerOfTwo(resolution.x)*uPowerOfTwo(resolution.y)*sizeof(unsigned char)*4);
	memset(groundValues,0,uPowerOfTwo(resolution.x)*uPowerOfTwo(resolution.y)*sizeof(unsigned char)*4);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4 , uPowerOfTwo(resolution.x), uPowerOfTwo(resolution.y), 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	setTex();
	//if(!dynamic)
	//{
	//	delete groundValues;
	//	groundValues = NULL;
	//}
}
void Level::heightmapGL::setTex() const
{	//////////////////////////////////
	//		B		//		-z		//
	//	C		A	//	-x		+x	//
	//		D		//		+z		//
	//////////////////////////////////
	int x,z;

	Vec3f n;
	Vec2f bSize((float)(resolution.x)/uPowerOfTwo(resolution.x),(float)(resolution.y)/uPowerOfTwo(resolution.y));

	for(x=0; x < uPowerOfTwo(resolution.x); x += 1)
	{
		for(z=0; z< uPowerOfTwo(resolution.y); z += 1)
		{
			n = getNormal(bSize.x*x,bSize.y*z);
			//if(x > 0.99)					n += getNormal(bSize.x*x-1,	bSize.y*z) * 0.5;
			//if(x < getResolutionX()-0.99)	n += getNormal(bSize.x*x+1,	bSize.y*z) * 0.5;
			//if(z > 0.99)					n += getNormal(bSize.x*x,	bSize.y*z-1) * 0.5;
			//if(z < getResolutionZ()-0.99)	n += getNormal(bSize.x*x,	bSize.y*z+1) * 0.5;
			if(n.magnitudeSquared() < 0.001)n = Vec3f(0.0,1.0,0.0);
			n = n.normalize();

			groundValues[(x+z*uPowerOfTwo(resolution.x))*4+0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x+z*uPowerOfTwo(resolution.x))*4+1] = (unsigned char)(n.y*255.0);
			groundValues[(x+z*uPowerOfTwo(resolution.x))*4+2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x+z*uPowerOfTwo(resolution.x))*4+3] = (unsigned char)(255.0*(heights[x+z*getResolutionX()]-minHeight)/(maxHeight-minHeight));//(255.0*(getHeight(bSize.x*x,bSize.y*z)/size.y-minHeight)/(maxHeight-minHeight));
		}
	}
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, uPowerOfTwo(resolution.x), uPowerOfTwo(resolution.y), GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
}
void Level::heightmapGL::createList() const
{
	dispList = glGenLists(1);
	glNewList(dispList,GL_COMPILE);
	for(unsigned int z=0;z<resolution.y-1;z++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int x=0;x<resolution.x;x++)
		{
			glVertex3f(x,heights[x+z*resolution.x],z);
			glVertex3f(x,heights[x+(z+1)*resolution.x],z+1);
		}
		glEnd();
	}
	glEndList();
}
void Level::heightmapGL::render() const
{
	if(!valid)
	{
		glDeleteLists(dispList,1);
		createList();
		setMinMaxHeights();
		setTex();
		valid=true;
	}
	if(shader != NULL)
	{
		glUseProgram(shader);
		dataManager.bind("sand",0);
		dataManager.bind("grass",1);
		dataManager.bind("rock",2);
		dataManager.bind("snow",3);
		dataManager.bind("LCnoise",4);
		dataManager.bindTex(groundTex,5);

		glUniform1f(glGetUniformLocation(shader, "maxHeight"),	maxHeight);
		glUniform1f(glGetUniformLocation(shader, "minHeight"),	minHeight + (maxHeight-minHeight)/3);
		glUniform1f(glGetUniformLocation(shader, "XZscale"),	resolution.x*size.x);
		glUniform1f(glGetUniformLocation(shader, "time"),		gameTime());
		glUniform1i(glGetUniformLocation(shader, "sand"),		0);
		glUniform1i(glGetUniformLocation(shader, "grass"),		1);
		glUniform1i(glGetUniformLocation(shader, "rock"),		2);
		glUniform1i(glGetUniformLocation(shader, "snow"),		3);
		glUniform1i(glGetUniformLocation(shader, "LCnoise"),	4);
		glUniform1i(glGetUniformLocation(shader, "groundTex"),	5);

		glPushMatrix();
		glTranslatef(position.x,position.y,position.z);
		glScalef(size.x,size.y,size.z);
		glCallList(dispList);
		glPopMatrix();

		dataManager.bindTex(0,6);
		dataManager.bindTex(0,5);
		dataManager.bindTex(0,4);
		dataManager.bindTex(0,3);
		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		glUseProgram(0);
	}
	//glDisable(GL_LIGHTING);
	//glEnable(GL_BLEND);	
	//glPushMatrix();
	//glColor4f(0.1,0.3,1.0,0.6);
	//glTranslatef(position.x,position.y,position.z);
	//glScalef(size.x,size.y,size.z);
	//for(unsigned int z=0;z<resolution.y-1;z++)
	//{
	//	glBegin(GL_TRIANGLE_STRIP);
	//	for(unsigned int x=0;x<resolution.x;x++)
	//	{
	//		glVertex3f(x,heights[x+z*resolution.x],z);
	//		glVertex3f(x,heights[x+(z+1)*resolution.x],z+1);
	//	}
	//	glEnd();
	//}
	//glColor4f(0.3,0.5,1.0,0.8);
	//glBegin(GL_LINES);
	//for(int x=0;x<resolution.x; x+=4)
	//{
	//	for(int y=0;y<resolution.y-1; y+=1)
	//	{
	//		glVertex3f(x,getRasterHeight(x,y),y);
	//		glVertex3f(x,getRasterHeight(x,y+1),y+1);
	//	}
	//}
	//for(int y=0;y<resolution.y; y+=4)
	//{
	//	for(int x=0;x<resolution.x-1; x+=1)
	//	{
	//		glVertex3f(x,getRasterHeight(x,y),y);
	//		glVertex3f(x+1,getRasterHeight(x+1,y),y);
	//	}
	//}
	//glEnd();
	//glPopMatrix();
}
void Level::heightmapBase::setMinMaxHeights() const
{
	if(heights == NULL || getResolutionX()==0 || getResolutionZ()==0) return;
	maxHeight=heights[0]+0.001;
	minHeight=heights[0];
	for(int x=0;x<getResolutionX();x++)
	{
		for(int z=0;z<getResolutionZ();z++)
		{
			if(heights[x+z*getResolutionX()]>maxHeight) maxHeight=heights[x+z*getResolutionX()];
			if(heights[x+z*getResolutionX()]<minHeight) minHeight=heights[x+z*getResolutionX()];
		}
	}
}
//__________________________________________________________________________________________________________________________________________________________//

Level::Level(LevelFile file)
{
	mGround = new heightmapGL(file.info->mapResolution,file.heights);
	mGround->setSize(file.info->mapSize);
	mWater.seaLevel = file.info->seaLevel;
}
Level::Level(string bmp)
{
	Image* image = loadBMP(bmp.c_str());
	float* t = new float[image->width * image->height];
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			t[y * image->width + x] = (float)image->pixels[3 * (y * image->width + x)]/255.0;
		}
	}
	//assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL"); 
	mGround = new heightmapGL(Vec2u(image->height,image->width),t);
	delete[] t;
	delete image;
}
LevelFile Level::getLevelFile() 
{
	LevelFile f;
	f.header.magicNumber = 0x454c49465f4c564c;
	f.header.version = 1;
	f.info = new LevelFile::Info;
	f.info->mapSize = mGround->getSize();
	f.info->mapResolution = mGround->getResolution();
	f.info->seaLevel = mWater.seaLevel;
	f.heights = mGround->heights;
	return f;
}
void Level::exportBMP(string filename)
{
	int width = mGround->getResolutionX();
	int size = (3*width+width%4)*mGround->getResolutionZ() + 3*width*mGround->getResolutionZ()%4;


	BITMAPFILEHEADER header;
	header.bfType					= 0x4D42;
	header.bfSize					= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + size;
	header.bfReserved1				= 0;
	header.bfReserved2				= 0;
	header.bfOffBits				= sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);

	BITMAPINFO info;
	info.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth			= mGround->getResolutionX();
	info.bmiHeader.biHeight			= mGround->getResolutionZ();
	info.bmiHeader.biPlanes			= 1;
	info.bmiHeader.biBitCount		= 24;
	info.bmiHeader.biCompression	= BI_RGB;
	info.bmiHeader.biSizeImage		= 0;
	info.bmiHeader.biXPelsPerMeter	= 100;
	info.bmiHeader.biYPelsPerMeter	= 100;
	info.bmiHeader.biClrUsed		= 0;
	info.bmiHeader.biClrImportant	= 0;
	memset(&info.bmiColors,0,sizeof(info.bmiColors));

	mGround->setMinMaxHeights();
	unsigned char* colors = new unsigned char[size];
	memset(colors,0,size);
	for(int z=0;z<mGround->getResolutionZ();z++)
	{
		for(int x=0;x<mGround->getResolutionX();x++)
		{
			colors[(z*(3*width+width%4) + 3*x) + 0] = 255.0*(mGround->getRasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
			colors[(z*(3*width+width%4) + 3*x) + 1] = 255.0*(mGround->getRasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
			colors[(z*(3*width+width%4) + 3*x) + 2] = 255.0*(mGround->getRasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
		}
	}
	ofstream fout(filename,ios::out|ios::binary|ios::trunc);
	fout.write((char*)&header,sizeof(header));
	fout.write((char*)&info,sizeof(info));
	fout.write((char*)colors,size);
	delete[] colors;
	fout.close();
}
const Level::heightmapBase* const Level::ground() const
{
	return mGround;
}
void Level::render()
{
	glColor3f(1,1,0);
	

	glPushMatrix();
	glScalef(size,1,size);

	
	//if(!mGround->VBOvalid && mGround->dynamic)
	//	mGround->setVBO();
	//if(!mGround->minMaxValid && mGround->dynamic)
	//	mGround->setMinMaxHeights();
	//if(!mGround->texValid && mGround->dynamic)
	//	mGround->setTex();
	if(mSettings.water)
	{
		mWater.seaLevel = (mGround->minHeight + (mGround->maxHeight-mGround->minHeight)/3)*mGround->getSizeY();

		int s=dataManager.getId("ocean");
		dataManager.bind("ocean");

		dataManager.bind("hardNoise",0);
		dataManager.bindTex(((heightmapGL*)mGround)->groundTex,1);
		dataManager.bind("rock",2);
		dataManager.bind("sand",3);

		glUniform1i(glGetUniformLocation(s, "bumpMap"), 0);
		glUniform1i(glGetUniformLocation(s, "groundTex"), 1);
		glUniform1i(glGetUniformLocation(s, "rock"), 2);
		glUniform1i(glGetUniformLocation(s, "sand"), 3);
		glUniform1f(glGetUniformLocation(s, "time"), gameTime());
		glUniform1f(glGetUniformLocation(s, "seaLevel"), 0.333f);
		glUniform1f(glGetUniformLocation(s, "XZscale"), mGround->size.x*mGround->resolution.x);
		glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->resolution.x)/uPowerOfTwo(mGround->resolution.x),(float)(mGround->resolution.y)/uPowerOfTwo(mGround->resolution.y));

		glBegin(GL_QUADS);
			glVertex3f(0,mWater.seaLevel,0);
			glVertex3f(0,mWater.seaLevel,mGround->size.z*(mGround->getResolutionZ()-1));
			glVertex3f(mGround->size.x*(mGround->getResolutionX()-1),mWater.seaLevel,mGround->size.z*(mGround->getResolutionZ()-1));
			glVertex3f(mGround->size.x*(mGround->getResolutionX()-1),mWater.seaLevel,0);
		glEnd();

		dataManager.bindTex(0,3);
		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		dataManager.bindShader(0);
	}

	mGround->render();
	//if(mGround->shader != 0)
	//{
	//	int textures[6]={0,1,2,3,4,5};
	//	glUseProgram(mGround->shader);
	//	dataManager.bind("sand",0);
	//	dataManager.bind("grass",1);
	//	dataManager.bind("rock",2);
	//	dataManager.bind("snow",3);
	//	dataManager.bind("LCnoise",4);
	//	//dataManager.bind("hardNoise",5);
	//	dataManager.bindTex(mGround->groundTex,5);

	//	glUniform1f(glGetUniformLocation(mGround->shader, "maxHeight"),	mGround->maxHeight);
	//	glUniform1f(glGetUniformLocation(mGround->shader, "minHeight"),	mWater.seaLevel);//mGround->minHeight + (mGround->maxHeight-mGround->minHeight)/3);
	//	glUniform1f(glGetUniformLocation(mGround->shader, "XZscale"),	mGround->size);
	//	glUniform1f(glGetUniformLocation(mGround->shader, "time"),		gameTime());
	//	glUniform1i(glGetUniformLocation(mGround->shader, "sand"),		textures[0]);
	//	glUniform1i(glGetUniformLocation(mGround->shader, "grass"),		textures[1]);
	//	glUniform1i(glGetUniformLocation(mGround->shader, "rock"),		textures[2]);
	//	glUniform1i(glGetUniformLocation(mGround->shader, "snow"),		textures[3]);
	//	glUniform1i(glGetUniformLocation(mGround->shader, "LCnoise"),	textures[4]);
	//	glUniform1i(glGetUniformLocation(mGround->shader, "groundTex"),	5);

	//	glPushMatrix();
	//	glBindBuffer(GL_ARRAY_BUFFER, mGround->VBOvert);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGround->VBOindex);
	//	
	//	glEnableClientState(GL_VERTEX_ARRAY);
	//	glVertexPointer(3, GL_FLOAT, 0, 0);
	//	glDrawElements(GL_TRIANGLE_STRIP, mGround->numIdices, GL_UNSIGNED_INT, 0);
	//	glDisableClientState(GL_VERTEX_ARRAY);
	//	
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	glPopMatrix();
	//	
	//	dataManager.bindTex(0,6);
	//	dataManager.bindTex(0,5);
	//	dataManager.bindTex(0,4);
	//	dataManager.bindTex(0,3);
	//	dataManager.bindTex(0,2);
	//	dataManager.bindTex(0,1);
	//	dataManager.bindTex(0,0);
	//	glUseProgram(0);



	//	//glDisable(GL_LIGHTING);
	//	//int x,z;
	//	//glColor3f(0,1,0);
	//	//glBegin(GL_LINES);
	//	//for(x=0;x<size;x+=1)
	//	//{
	//	//	for(z=0; z<size; z+=1)
	//	//	{
	//	//		glVertex3f(x,mGround->vertices[x*mGround->size+z].y,z);
	//	//		glVertex3f(x +										250.0*(2.0/255.0*mGround->normalValues[(x+z*mGround->size)*3+0] - 1.0)/size,
	//	//				   mGround->vertices[x*mGround->size+z].y +	250.0*(1.0/255.0*mGround->normalValues[(x+z*mGround->size)*3+1]),
	//	//				   z +										250.0*(2.0/255.0*mGround->normalValues[(x+z*mGround->size)*3+2] - 1.0)/size);
	//	//	}
	//	//}
	//	//glEnd();
	//	//glEnable(GL_LIGHTING);


	//}
	//else
	//{

	//	glBindBuffer(GL_ARRAY_BUFFER, mGround->VBOvert);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGround->VBOindex);

	//	glEnableClientState(GL_VERTEX_ARRAY);
	//	glDrawElements(GL_TRIANGLE_STRIP, mGround->numIdices, GL_UNSIGNED_INT, 0);
	//	glDisableClientState(GL_VERTEX_ARRAY);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//}





	//glScalef(mGround->getSizeX(),mGround->getSizeY(),mGround->getSizeZ());
	//float h = mSettings.water ? (mGround->minHeight + (mGround->maxHeight-mGround->minHeight)/3) : mGround->minHeight-20.0;
	//dataManager.bind("layers");
	//glDisable(GL_LIGHTING);
	//glColor3f(1,1,1);
	//float t=0.0;

	//glBegin(GL_TRIANGLE_STRIP);
	//for(int i = 0; i < mGround->getResolutionX()-1; i++)
	//{
	//	glTexCoord2f(t,(mGround->getRasterHeight(i,0)-h)/256);		glVertex3f(i,h,0);
	//	glTexCoord2f(t,(mGround->getRasterHeight(i,0)-h)/1024);		glVertex3f(i,max(mGround->getRasterHeight(i,0),h) ,0);
	//	t+=0.2;
	//}
	//for(int i = 0; i < mGround->getResolutionZ()-1; i++)
	//{
	//	glTexCoord2f(t,(mGround->getRasterHeight(mGround->getResolutionX()-1,i)-h)/256);	glVertex3f(mGround->getResolutionX()-1,h,i);
	//	glTexCoord2f(t,(mGround->getRasterHeight(mGround->getResolutionX()-1,i)-h)/1024);	glVertex3f(mGround->getResolutionX()-1,max(mGround->getRasterHeight(mGround->getResolutionX()-1,i),h),i);
	//	t+=0.2;	
	//}
	//t+=0.2;
	//for(int i = mGround->getResolutionX()-1; i > 0; i--)
	//{
	//	t-=0.2;
	//	glTexCoord2f(t,(mGround->getRasterHeight(i,mGround->getResolutionZ()-1)-h)/256);	glVertex3f(i,h,mGround->getResolutionZ()-1);
	//	glTexCoord2f(t,(mGround->getRasterHeight(i,mGround->getResolutionZ()-1)-h)/1024);	glVertex3f(i,max(mGround->getRasterHeight(i,mGround->getResolutionZ()-1),h),mGround->getResolutionZ()-1);
	//}
	//for(int i = mGround->getResolutionZ()-1; i >= 0; i--)
	//{
	//	t-=0.2;
	//	glTexCoord2f(t,(mGround->getRasterHeight(0,i)-h)/256);		glVertex3f(0,h,i);
	//	glTexCoord2f(t,(mGround->getRasterHeight(0,i)-h)/1024);		glVertex3f(0,max(mGround->getRasterHeight(0,i),h),i);
	//}
	//glEnd();
	//dataManager.bindTex(0);
	//if(!mSettings.water)
	//{
	//	glColor3f(0.73,0.6,0.47);
	//	glBegin(GL_QUADS);
	//		glVertex3f(0,h,0);
	//		glVertex3f(0,h,mGround->getResolutionZ()-1);
	//		glVertex3f(mGround->getResolutionX()-1,h,mGround->getResolutionZ()-1);
	//		glVertex3f(mGround->getResolutionX()-1,h,0);
	//	glEnd();
	//}
	glPopMatrix();
}
//__________________________________________________________________________________________________________________________________________________________//

editLevel::editLevel()
{

}
Level::heightmapBase* editLevel::ground()
{
	return mGround;
}
Level::levelSettings* editLevel::settings()
{
	return &mSettings;
}
void editLevel::newGround(unsigned int x, unsigned int z, float* heights)
{
	if(mGround != NULL)
		delete mGround;
	if(heights != NULL)
		mGround = new heightmapGL(Vec2u(x,z),heights);
	else
		mGround = new heightmapGL(Vec2u(x,z));
}
void editLevel::addObject(int type,int team, Vec3f pos)
{
	object o;
	o.type=type;
	o.team=team;
	o.startloc=pos;
	mObjects.push_back(o);
}
void editLevel::renderObjects()
{
	for(vector<object>::iterator i=mObjects.begin();i!=mObjects.end();i++)
	{
		if(i->type & PLANE)
		{
			glPushMatrix();
			glTranslatef(i->startloc.x,i->startloc.y,i->startloc.z);
			glScalef(10,10,10);
			dataManager.draw((planeType)i->type);
			glPopMatrix();
		}
	}
}