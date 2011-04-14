
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
		fin.read((char*)objects,info->numObjects*sizeof(Object));

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
		fout.write((char*)objects,info->numObjects*sizeof(Object));
		fout.write((char*)heights,info->mapResolution.x*info->mapResolution.y*sizeof(float));
	}
	fout.close();
}
LevelFile::LevelFile(): info(NULL), objects(NULL), heights(NULL)
{
	header.magicNumber = 0x454c49465f4c564c;
	header.version = 0;
}
LevelFile::LevelFile(string filename): info(NULL), objects(NULL), heights(NULL)
{
	header.magicNumber = 0x454c49465f4c564c;
	header.version = 0;
	load(filename);
}
//________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________//
float Level::heightmapBase::rasterHeight(unsigned int x,unsigned int z) const
{
	return heights[clamp(x,0,resolutionX()-1) + clamp(z,0,resolutionZ()-1)*resolutionX()];
}
float Level::heightmapBase::interpolatedHeight(float x, float z) const
{
	if(x-floor(x)+z-floor(z)<1.0)
	{
		float A = rasterHeight(floor(x),floor(z));
		float B = rasterHeight(floor(x),floor(z+1));
		float D = rasterHeight(floor(x+1),floor(z));
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x));
	}
	else
	{
		float B = rasterHeight(floor(x),floor(z+1));
		float C = rasterHeight(floor(x+1),floor(z+1));
		float D = rasterHeight(floor(x+1),floor(z));
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}
float Level::heightmapBase::height(float x, float z) const
{	//    A------D		.-----x+
	//	  |      |		|
	//    |      |		|
	//    B------C		z+

	x -= mPosition.x;
	z -= mPosition.z;
	x *= mResolution.x/mSize.x;
	z *= mResolution.y/mSize.y;
	return interpolatedHeight(x,z);
}

Vec3f Level::heightmapBase::rasterNormal(unsigned int h, unsigned int k) const
{
	unsigned int x=h; unsigned int z=k;
	float Cy = (z < resolutionZ()-1)	? ((float)heights[x	+(z+1)*resolutionX()]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	float Ay = (x < resolutionX()-1)	? ((float)heights[(x+1)+z*resolutionX()	]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	float Dy = (  z > 0	)				? ((float)heights[x	+(z-1)*resolutionX()]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	float By = (  x > 0	)				? ((float)heights[(x-1)+z*resolutionX()	]	- heights[x+z*resolutionX()])/255.0  : 0.0f;
	//if(z >= resolutionZ()-1) Cy=Dy;
	//if(x >= resolutionX()-1) Ay=By;
	//if(z <= 0) Dy = Cy;
	//if(x <= 0) By = Ay;
	return Vec3f(Cy - Ay, 3.0 / 15.0, Dy - By).normalize();	
}
Vec3f Level::heightmapBase::interpolatedNormal(float x, float z) const
{
	if(x-floor(x)+z-floor(z)<1.0)
	{
		Vec3f A = rasterNormal(floor(x),floor(z));
		Vec3f B = rasterNormal(floor(x),floor(z+1));
		Vec3f D = rasterNormal(floor(x+1),floor(z));
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x));
	}
	else
	{
		Vec3f B = rasterNormal(floor(x),floor(z+1));
		Vec3f C = rasterNormal(floor(x+1),floor(z+1));
		Vec3f D = rasterNormal(floor(x+1),floor(z));
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}
Vec3f Level::heightmapBase::normal(float x, float z) const
{	//    A------D		.-----x+
	//	  |      |		|
	//    |      |		|
	//    B------C		z+

	x -= mPosition.x;
	z -= mPosition.z;
	x *= mResolution.x/mSize.x;
	z *= mResolution.y/mSize.y;
	return interpolatedNormal(x,z);
}
Level::heightmapBase::heightmapBase(Vec2u Resolution): mResolution(Resolution), mPosition(0,0,0), mSize(Resolution.x,Resolution.y), heights(NULL)
{
	heights = new float[mResolution.x*mResolution.y];
	memset(heights,0,mResolution.x*mResolution.y*sizeof(float));
}
Level::heightmapBase::heightmapBase(Vec2u Resolution, float* hts): mResolution(Resolution), mPosition(0,0,0), mSize(Resolution.x,Resolution.y), heights(NULL)
{
	heights = new float[mResolution.x*mResolution.y];
	memcpy(heights,hts,mResolution.x*mResolution.y*sizeof(float));
}
void Level::heightmapGL::init()
{
	glGenTextures(1,(GLuint*)&groundTex);
	groundValues = (unsigned char*)malloc(uPowerOfTwo(mResolution.x)*uPowerOfTwo(mResolution.y)*sizeof(unsigned char)*4);
	memset(groundValues,0,uPowerOfTwo(mResolution.x)*uPowerOfTwo(mResolution.y)*sizeof(unsigned char)*4);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4 , uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//setTex();
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
	Vec2f bSize((float)(mResolution.x)/uPowerOfTwo(mResolution.x),(float)(mResolution.y)/uPowerOfTwo(mResolution.y));

	for(x=0; x < uPowerOfTwo(mResolution.x); x += 1)
	{
		for(z=0; z< uPowerOfTwo(mResolution.y); z += 1)
		{
			n = interpolatedNormal(bSize.x*x,bSize.y*z);
			//if(x > 0.99)					n += getNormal(bSize.x*x-1,	bSize.y*z) * 0.5;
			//if(x < resolutionX()-0.99)	n += getNormal(bSize.x*x+1,	bSize.y*z) * 0.5;
			//if(z > 0.99)					n += getNormal(bSize.x*x,	bSize.y*z-1) * 0.5;
			//if(z < resolutionZ()-0.99)	n += getNormal(bSize.x*x,	bSize.y*z+1) * 0.5;
			if(n.magnitudeSquared() < 0.001)n = Vec3f(0.0,1.0,0.0);
			n = n.normalize();

			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+1] = (unsigned char)(n.y*255.0);
			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x+z*uPowerOfTwo(mResolution.x))*4+3] = (unsigned char)(255.0*(interpolatedHeight(bSize.x*x,bSize.y*z)-minHeight)/(maxHeight-minHeight));//(255.0*(getHeight(bSize.x*x,bSize.y*z)/size.y-minHeight)/(maxHeight-minHeight));
		}
	}
	glBindTexture(GL_TEXTURE_2D, groundTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, 4 , uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, uPowerOfTwo(mResolution.x), uPowerOfTwo(mResolution.y), GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
}
void Level::heightmapGL::createList() const
{
	dispList = glGenLists(1);
	glNewList(dispList,GL_COMPILE);
	for(unsigned int z=0;z<mResolution.y-1;z++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int x=0;x<mResolution.x;x++)
		{
			glVertex3f((float)x/(mResolution.x-1),heights[x+z*mResolution.x],	(float)(z)/(mResolution.y-1));
			glVertex3f((float)x/(mResolution.x-1),heights[x+(z+1)*mResolution.x],(float)(z+1)/(mResolution.y-1));
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
		glUniform1f(glGetUniformLocation(shader, "minHeight"),	0);
		glUniform1f(glGetUniformLocation(shader, "XZscale"),	1);
		glUniform1f(glGetUniformLocation(shader, "time"),		world.time());
		glUniform1i(glGetUniformLocation(shader, "sand"),		0);
		glUniform1i(glGetUniformLocation(shader, "grass"),		1);
		glUniform1i(glGetUniformLocation(shader, "rock"),		2);
		glUniform1i(glGetUniformLocation(shader, "snow"),		3);
		glUniform1i(glGetUniformLocation(shader, "LCnoise"),	4);
		glUniform1i(glGetUniformLocation(shader, "groundTex"),	5);

		glPushMatrix();
		glTranslatef(mPosition.x,mPosition.y,mPosition.z);
		glScalef(mSize.x,1,mSize.y);
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
	//		glVertex3f(x,rasterHeight(x,y),y);
	//		glVertex3f(x,rasterHeight(x,y+1),y+1);
	//	}
	//}
	//for(int y=0;y<resolution.y; y+=4)
	//{
	//	for(int x=0;x<resolution.x-1; x+=1)
	//	{
	//		glVertex3f(x,rasterHeight(x,y),y);
	//		glVertex3f(x+1,rasterHeight(x+1,y),y);
	//	}
	//}
	//glEnd();
	//glPopMatrix();
}
void Level::heightmapGL::renderPreview(float seaLevelOffset) const
{
	if(seaLevelOffset >= maxHeight)
		return;

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
		glUniform1f(glGetUniformLocation(shader, "minHeight"),	max(minHeight,seaLevelOffset));
		glUniform1f(glGetUniformLocation(shader, "XZscale"),	1);
		glUniform1f(glGetUniformLocation(shader, "time"),		world.time());
		glUniform1i(glGetUniformLocation(shader, "sand"),		0);
		glUniform1i(glGetUniformLocation(shader, "grass"),		1);
		glUniform1i(glGetUniformLocation(shader, "rock"),		2);
		glUniform1i(glGetUniformLocation(shader, "snow"),		3);
		glUniform1i(glGetUniformLocation(shader, "LCnoise"),	4);
		glUniform1i(glGetUniformLocation(shader, "groundTex"),	5);

		glPushMatrix();
		glTranslatef(mPosition.x,mPosition.y,mPosition.z);
		glScalef(mSize.x,1,mSize.y);
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
}
void Level::heightmapBase::setMinMaxHeights() const
{
	if(heights == NULL || resolutionX()==0 || resolutionZ()==0) return;
	maxHeight=heights[0]+0.001;
	minHeight=heights[0];
	for(int x=0;x<resolutionX();x++)
	{
		for(int z=0;z<resolutionZ();z++)
		{
			if(heights[x+z*resolutionX()]>maxHeight) maxHeight=heights[x+z*resolutionX()];
			if(heights[x+z*resolutionX()]<minHeight) minHeight=heights[x+z*resolutionX()];
		}
	}
}
void Level::heightmapGL::increaseHeights(float amount)
{
	if(heights == NULL || resolutionX()==0 || resolutionZ()==0) return;
	int x,z;
	for(x=0;x<resolutionX();x++)
	{
		for(z=0;z<resolutionZ();z++)
		{
			heights[x+z*resolutionX()] += amount;
		}
	}
}
//________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________//

Level::Level(LevelFile file)
{
	mGround = new heightmapGL(file.info->mapResolution,file.heights);
	mGround->setSize(file.info->mapSize);
	mGround->init();
	for(int i=0; i < file.info->numObjects; i++)
		mObjects.push_back(file.objects[i]);
}
Level::Level(string BMP, Vec3f size, float seaLevel)
{
	Image* image = loadBMP(BMP.c_str());
	float* t = new float[image->width * image->height];
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			t[y * image->width + x] = (float)((unsigned char)image->pixels[3 * (y * image->width + x)])/255.0*size.y - seaLevel;
		}
	}
	//assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL"); 
	mGround = new heightmapGL(Vec2u(image->height,image->width),t);
	mGround->init();
	mGround->setSize(Vec2f(size.x,size.z));
	mGround->setMinMaxHeights();

	delete[] t;
	delete image;
}
LevelFile Level::getLevelFile() 
{
	LevelFile f;
	f.header.magicNumber = 0x454c49465f4c564c;
	f.header.version = 1;
	f.info = new LevelFile::Info;
	f.info->mapSize = mGround->size();
	f.info->mapResolution = mGround->resolution();
	f.info->numObjects = mObjects.size();
	f.heights = mGround->heights;
	f.objects = new LevelFile::Object[mObjects.size()];
	memcpy(f.objects, &(*mObjects.begin()),mObjects.size()*sizeof(LevelFile::Object));

	return f;
}
LevelFile Level::getLevelFile(float seaLevelOffset)
{
	LevelFile f;
	f.header.magicNumber = 0x454c49465f4c564c;
	f.header.version = 1;
	f.info = new LevelFile::Info;
	f.info->mapSize = mGround->size();
	f.info->mapResolution = mGround->resolution();
	f.info->numObjects = mObjects.size();
	f.heights = mGround->heights;
	if(mObjects.size() > 0)
	{
		f.objects = new LevelFile::Object[mObjects.size()];
		memcpy(f.objects, &(*mObjects.begin()),mObjects.size()*sizeof(LevelFile::Object));
	}

	int x,z;
	for(x=0;x<mGround->resolutionX();x++)
	{
		for(z=0;z<mGround->resolutionZ();z++)
		{
			f.heights[x+z*mGround->resolutionX()] -= seaLevelOffset;
		}
	}

	return f;
}


void Level::exportBMP(string filename)
{
	int width = mGround->resolutionX();
	int size = (3*width+width%4)*mGround->resolutionZ() + 3*width*mGround->resolutionZ()%4;


	BITMAPFILEHEADER header;
	header.bfType					= 0x4D42;
	header.bfSize					= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + size;
	header.bfReserved1				= 0;
	header.bfReserved2				= 0;
	header.bfOffBits				= sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);

	BITMAPINFO info;
	info.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth			= mGround->resolutionX();
	info.bmiHeader.biHeight			= mGround->resolutionZ();
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
	for(int z=0;z<mGround->resolutionZ();z++)
	{
		for(int x=0;x<mGround->resolutionX();x++)
		{
			colors[(z*(3*width+width%4) + 3*x) + 0] = 255.0*(mGround->rasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
			colors[(z*(3*width+width%4) + 3*x) + 1] = 255.0*(mGround->rasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
			colors[(z*(3*width+width%4) + 3*x) + 2] = 255.0*(mGround->rasterHeight(x,z)-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight);
		}
	}
	ofstream fout(filename,ios::out|ios::binary|ios::trunc);
	fout.write((char*)&header,sizeof(header));
	fout.write((char*)&info,sizeof(info));
	fout.write((char*)colors,size);
	delete[] colors;
	fout.close();
}
void Level::renderPreview(bool drawWater, float seaLevelOffset)
{
	glColor3f(1,1,0);
	

	glPushMatrix();
	
	if(drawWater)
	{
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
		glUniform1f(glGetUniformLocation(s, "time"), world.time());
		glUniform1f(glGetUniformLocation(s, "seaLevel"), (seaLevelOffset-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight));
		glUniform1f(glGetUniformLocation(s, "XZscale"), mGround->mSize.x);
		//glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->mResolution.x)/uPowerOfTwo(mGround->mResolution.x),(float)(mGround->mResolution.y)/uPowerOfTwo(mGround->mResolution.y));

		glBegin(GL_QUADS);
			glVertex3f(0,seaLevelOffset,0);
			glVertex3f(0,seaLevelOffset,mGround->mSize.y);
			glVertex3f(mGround->mSize.x,seaLevelOffset,mGround->mSize.y);
			glVertex3f(mGround->mSize.x,seaLevelOffset,0);
		glEnd();

		dataManager.bindTex(0,3);
		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		dataManager.bindShader(0);
	}

	mGround->renderPreview(seaLevelOffset);


	glScalef(mGround->sizeX()/(mGround->resolutionX()-1),1,mGround->sizeZ()/(mGround->resolutionZ()-1));
	float h = max(mGround->minHeight-20.0,seaLevelOffset);//needs to be adjusted for sea level
	dataManager.bind("layers");
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	float t=0.0;

	glBegin(GL_TRIANGLE_STRIP);
	for(int i = 0; i < mGround->resolutionX()-1; i++)
	{
		glTexCoord2f(t,(mGround->rasterHeight(i,0)-h)/256);		glVertex3f(i,h,0);
		glTexCoord2f(t,0);	glVertex3f(i,max(mGround->rasterHeight(i,0),h) ,0);
		t+=0.2;
	}
	for(int i = 0; i < mGround->resolutionZ()-1; i++)
	{
		glTexCoord2f(t,(mGround->rasterHeight(mGround->resolutionX()-1,i)-h)/256);	glVertex3f(mGround->resolutionX()-1,h,i);
		glTexCoord2f(t,0);	glVertex3f(mGround->resolutionX()-1,max(mGround->rasterHeight(mGround->resolutionX()-1,i),h),i);
		t+=0.2;	
	}
	t+=0.2;
	for(int i = mGround->resolutionX()-1; i > 0; i--)
	{
		t-=0.2;
		glTexCoord2f(t,(mGround->rasterHeight(i,mGround->resolutionZ()-1)-h)/256);	glVertex3f(i,h,mGround->resolutionZ()-1);
		glTexCoord2f(t,0);	glVertex3f(i,max(mGround->rasterHeight(i,mGround->resolutionZ()-1),h),mGround->resolutionZ()-1);
	}
	for(int i = mGround->resolutionZ()-1; i >= 0; i--)
	{
		t-=0.2;
		glTexCoord2f(t,(mGround->rasterHeight(0,i)-h)/256);		glVertex3f(0,h,i);
		glTexCoord2f(t,0);	glVertex3f(0,max(mGround->rasterHeight(0,i),h),i);
	}
	glEnd();
	dataManager.bindTex(0);
	if(!drawWater)
	{
		glColor3f(0.73,0.6,0.47);
		glBegin(GL_QUADS);
			glVertex3f(0,h,0);
			glVertex3f(0,h,mGround->resolutionZ()-1);
			glVertex3f(mGround->resolutionX()-1,h,mGround->resolutionZ()-1);
			glVertex3f(mGround->resolutionX()-1,h,0);
		glEnd();
	}
	glPopMatrix();
}
void Level::renderObjectsPreview()
{
	for(auto i=mObjects.begin();i!=mObjects.end();i++)
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
void Level::render(Vec3f eye)
{
	glDisable(GL_CULL_FACE);
	glPushMatrix();

	mGround->render();

	//if(mSettings.water)
	{
		glDepthMask(false);
		glEnable(GL_BLEND);
		//mWater.seaLevel = mGround->minHeight + (mGround->maxHeight-mGround->minHeight)/3;

		//int s=dataManager.getId("ocean");
		//dataManager.bind("ocean");

		//dataManager.bind("hardNoise",0);
		//dataManager.bindTex(((heightmapGL*)mGround)->groundTex,1);
		//dataManager.bind("rock",2);
		//dataManager.bind("sand",3);

		//glUniform1i(glGetUniformLocation(s, "bumpMap"), 0);
		//glUniform1i(glGetUniformLocation(s, "groundTex"), 1);
		//glUniform1i(glGetUniformLocation(s, "rock"), 2);
		//glUniform1i(glGetUniformLocation(s, "sand"), 3);
		//glUniform1f(glGetUniformLocation(s, "time"), world.time());
		//glUniform1f(glGetUniformLocation(s, "seaLevel"), 0.333f);
		//glUniform1f(glGetUniformLocation(s, "XZscale"), mGround->mSize.x);
		////glUniform2f(glGetUniformLocation(s, "texScale"), (float)(mGround->mResolution.x)/uPowerOfTwo(mGround->mResolution.x),(float)(mGround->mResolution.y)/uPowerOfTwo(mGround->mResolution.y));

		//glBegin(GL_QUADS);
		//	glVertex3f(0,mWater.seaLevel,0);
		//	glVertex3f(0,mWater.seaLevel,mGround->mSize.y);
		//	glVertex3f(mGround->mSize.x,mWater.seaLevel,mGround->mSize.y);
		//	glVertex3f(mGround->mSize.x,mWater.seaLevel,0);
		//glEnd();

		//dataManager.bindTex(0,3);
		//dataManager.bindTex(0,2); 
		//dataManager.bindTex(0,1);
		//dataManager.bindTex(0,0);
		//dataManager.bindShader(0);
		//--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\
		//--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||--||
		//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//--\\--//

		Vec3d center(eye.x,0,eye.z);
		double radius = (eye.y)*tan(asin(6000000/(6000000+eye.y)));
		float cAng,sAng;

		int s=dataManager.getId("horizon2");
		dataManager.bind("horizon2");

		dataManager.bind("hardNoise",0);
		dataManager.bindTex(((Level::heightmapGL*)mGround)->groundTex,1);
		dataManager.bind("sand",2);
		//dataManager.bind("sand",2);

		glUniform1i(glGetUniformLocation(s, "bumpMap"), 0);
		glUniform1i(glGetUniformLocation(s, "ground"), 1);
		glUniform1i(glGetUniformLocation(s, "tex"), 2);
		glUniform1f(glGetUniformLocation(s, "time"), world.time());
		glUniform1f(glGetUniformLocation(s, "seaLevel"), -mGround->minHeight/(mGround->maxHeight-mGround->minHeight));
		glUniform2f(glGetUniformLocation(s, "center"), center.x,center.z); 

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glPushMatrix();
		
		glBegin(GL_TRIANGLE_FAN);
			glTexCoord2f(center.x/mGround->sizeX(),center.z/mGround->sizeZ());
			glVertex3f(center.x,center.y,center.z);

			for(float ang = 0; ang < PI*2.0+0.01; ang +=PI/8.0)
			{
				cAng=cos(ang);
				sAng=sin(ang);
				glTexCoord2f((center.x-cAng*radius)/mGround->sizeX(),(center.z-sAng*radius)/mGround->sizeZ());
				glVertex3f(center.x-cAng*radius,center.y,center.z-sAng*radius);
			}
		glEnd();
		glPopMatrix();

		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		dataManager.bindShader(0);

		glDepthMask(true);
		glDisable(GL_BLEND);
	}



	glPopMatrix();
}
//________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________//
editLevel::editLevel()
{

}
Level::heightmapBase* editLevel::ground()
{
	return mGround;
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
void editLevel::addObject(int type,int team,int controlType,Vec3f pos, Quat4f rot)
{
	LevelFile::Object o;
	o.type=type;
	o.team=team;
	o.controlType=controlType;
	o.startloc=pos;
	o.startRot=rot;
	mObjects.push_back(o);
}
void editLevel::setWater(string shaderName)
{
	water.name = shaderName;
}