
#include "main.h"

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
	x *= size.x/resolution.x;
	z *= size.z/resolution.y;

	if(x-floor(x)+z-floor(z)<1.0)
	{
		float A = position.y+size.y/255*heights[clamp((int)floor(x),0,getResolutionX()-1) + clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX()];
		float B = position.y+size.y/255*heights[clamp((int)floor(x),0,getResolutionX()-1) + clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX()];
		float D = position.y+size.y/255*heights[clamp((int)floor(x)+1,0,getResolutionX()-1) + clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX()];
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x));
	}
	else
	{
		float B = position.y+size.y/255*heights[clamp((int)floor(x),0,getResolutionX()-1) + clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX()];
		float C = position.y+size.y/255*heights[clamp((int)floor(x)+1,0,getResolutionX()-1) + clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX()];
		float D = position.y+size.y/255*heights[clamp((int)floor(x)+1,0,getResolutionX()-1) + clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX()];
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}

Vec3f Level::heightmapBase::getRasterNormal(unsigned int x, unsigned int z) const
{
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
	x *= size.x/resolution.x;
	z *= size.z/resolution.y;

	if(x-floor(x)+z-floor(z)<1.0)
	{
		Vec3f A = getRasterNormal(clamp((int)floor(x),0,getResolutionX()-1), clamp((int)floor(z),0,getResolutionZ()-1));
		Vec3f B = getRasterNormal(clamp((int)floor(x),0,getResolutionX()-1), clamp((int)floor(z)+1,0,getResolutionZ()-1));
		Vec3f D = getRasterNormal(clamp((int)floor(x)+1,0,getResolutionX()-1), clamp((int)floor(z),0,getResolutionZ()-1));
		return lerp(lerp(A,B,z-floor(z)),D,x-floor(x)).normalize();
	}
	else
	{
		Vec3f B = getRasterNormal(clamp((int)floor(x),0,getResolutionX()-1), clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX());
		Vec3f C = getRasterNormal(clamp((int)floor(x)+1,0,getResolutionX()-1), clamp((int)floor(z)+1,0,getResolutionZ()-1)*getResolutionX());
		Vec3f D = getRasterNormal(clamp((int)floor(x)+1,0,getResolutionX()-1), clamp((int)floor(z),0,getResolutionZ()-1)*getResolutionX());
		return lerp(lerp(B,C,x-floor(x)),D,1.0-(z-floor(z)));
	}
}
Level::heightmapBase::heightmapBase(Vector2<unsigned int> Resolution): resolution(Resolution), position(0,0,0), size(Resolution.x,1,Resolution.y), heights(NULL)
{
	heights = new float[resolution.x*resolution.y];

}
Level::heightmapBase::heightmapBase(Vector2<unsigned int> Resolution, float* hts): resolution(Resolution), position(0,0,0), size(Resolution.x,1,Resolution.y), heights(NULL)
{
	heights = new float[resolution.x*resolution.y];
	memcpy(heights,hts,resolution.x*resolution.y*sizeof(float));
}
void Level::heightmapGL::init()
{
	glGenTextures(1,(GLuint*)&groundTex);
	groundValues = (unsigned char*)malloc(resolution.x*resolution.y*sizeof(unsigned char)*4);
	memset(groundValues,0,resolution.x*resolution.y*sizeof(unsigned char)*4);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4 , resolution.x, resolution.y, 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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

	Vec3f a,b,n;
	for(x=0;x<getResolutionX();x++)
	{
		for(z=0; z<getResolutionZ(); z++)
		{
			n = getRasterNormal(x,z);
			if(x > 0)					n += getNormal(x-1,z) * 0.5;
			if(x < getResolutionX()-1)	n += getNormal(x+1,z) * 0.5;
			if(x > 0)					n += getNormal(x-1,z) * 0.5;
			if(x < getResolutionZ()-1)	n += getNormal(x+1,z) * 0.5;
			if(n.magnitudeSquared() < 0.001) n = Vec3f(0.0,1.0,0.0);
			n = n.normalize();

			groundValues[(x+z*getResolutionX())*4+0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x+z*getResolutionX())*4+1] = (unsigned char)(n.y*255.0);
			groundValues[(x+z*getResolutionX())*4+2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x+z*getResolutionX())*4+3] = (unsigned char)(255.0*(heights[x+z*getResolutionX()]-minHeight)/(maxHeight-minHeight));
		}
	}
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getResolutionX(), getResolutionZ(), GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
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
		glUniform1f(glGetUniformLocation(shader, "minHeight"),	minHeight);
		glUniform1f(glGetUniformLocation(shader, "XZscale"),	resolution.x);
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
void Level::heightmap::setVBO() const
{
	glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
	glBufferData(GL_ARRAY_BUFFER, numVerts*sizeof(vertex), vertices, GL_DYNAMIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	VBOvalid = true;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
Vec3f Level::heightmap::getNormal(unsigned int x, unsigned int z) const
{
	float Cy = (z < size-1)	? (vertices[x*size+(z+1)].y - vertices[x*size+z].y)/(maxHeight-minHeight)  : 0.0f;
	float Ay = (x < size-1)	? (vertices[(x+1)*size+z].y - vertices[x*size+z].y)/(maxHeight-minHeight)  : 0.0f;
	float Dy = (  z > 0	)	? (vertices[x*size+(z-1)].y - vertices[x*size+z].y)/(maxHeight-minHeight)  : 0.0f;
	float By = (  x > 0	)	? (vertices[(x-1)*size+z].y - vertices[x*size+z].y)/(maxHeight-minHeight)  : 0.0f;
	if(z >= size-1) Cy=Dy;
	if(x >= size-1) Ay=By;
	if(z <= 0) Dy = Cy;
	if(x <= 0) By = Ay;
	return Vec3f(Cy - Ay, 2.0 / 15.0, Dy - By).normalize();	
}
void Level::heightmap::setTex() const
{	//////////////////////////////////
	//		B		//		-z		//
	//	C		A	//	-x		+x	//
	//		D		//		+z		//
	//////////////////////////////////
	int x,z;

	Vec3f a,b,n;
	for(x=0;x<size;x++)
	{
		for(z=0; z<size; z++)
		{
			n = getNormal(x,z);
			//if(x > 0)		n += getNormal(x-1,z) * 0.5;
			//if(x < size-1)	n += getNormal(x+1,z) * 0.5;
			//if(x > 0)		n += getNormal(x-1,z) * 0.5;
			//if(x < size-1)	n += getNormal(x+1,z) * 0.5;
			if(n.magnitude() < 0.0001) n = Vec3f(0.0,1.0,0.0);
			n = n.normalize();

			groundValues[(x+z*size)*4+0] = (unsigned char)(127.0+n.x*128.0);
			groundValues[(x+z*size)*4+1] = (unsigned char)(n.y*255.0);
			groundValues[(x+z*size)*4+2] = (unsigned char)(127.0+n.z*128.0);
			groundValues[(x+z*size)*4+3] = (unsigned char)(255.0*(vertices[x*size+z].y-minHeight)/(maxHeight-minHeight));
		}
	}
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	//glTexImage2D(GL_TEXTURE_2D, 0, 4 , size, size, 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	texValid = true;
}
void Level::heightmap::init(float* heights)
{
	int x,z,i;
	////////////////////////////vertices//////////////////////////////////
	numVerts=size*size;
	vertices=(vertex*)malloc(numVerts*sizeof(vertex));
	for(x=0;x<size;x++)
	{
		for(z=0;z<size;z++)
		{
			vertices[x*size+z].x=x;
			vertices[x*size+z].y=heights[x*size+z];
			vertices[x*size+z].z=z;
		}
	}
	/////////////////////////////indices///////////////////////////////////
	numIdices=size*(size-1)*2;
	indices=(index*)malloc(numIdices*sizeof(index));
	for(x=0,i=0;;)
	{
		for(z=0; z<size; z++)
		{
			indices[i++] = x+z*size;
			indices[i++] = x+1+z*size;
		}
		if(++x==size-1) break;
		for(z=size-1; z>=0; z--)
		{
			indices[i++] = x+1+z*size;
			indices[i++] = x+z*size;
		}
		if(++x==size-1) break;
	}
	/////////////////////////////init VBO//////////////////////////////////////
	glGenBuffers(1,(GLuint*)&VBOvert);
	glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
	glBufferData(GL_ARRAY_BUFFER, numVerts*sizeof(vertex), vertices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glGenBuffers(1,(GLuint*)&VBOindex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIdices*sizeof(index), indices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	setMinMaxHeights();
	VBOvalid = true;
	/////////////////////////////////TEXTURES//////////////////////////////////
	glGenTextures(1,(GLuint*)&groundTex);
	groundValues = (unsigned char*)malloc(size*size*sizeof(unsigned char)*4);
	memset(groundValues,0,size*size*sizeof(unsigned char)*4);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4 , size, size, 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (void*)groundValues);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	setTex();
	if(!dynamic)
	{
		delete groundValues;
		groundValues = NULL;
	}
}
Level::heightmap::heightmap(int Size, bool Dynamic): size(Size), dynamic(Dynamic), groundTex(0), groundValues(NULL)
{
	float* tmpHeights=(float*)malloc(size*size*sizeof(float));
	memset(tmpHeights,0,size*size*sizeof(float));
	init(tmpHeights);
	free(tmpHeights);
}
Level::heightmap::heightmap(int Size, float* heights, bool Dynamic): size(Size), dynamic(Dynamic), groundTex(0), groundValues(NULL)
{
	init(heights);
}
void Level::heightmap::setHeight(unsigned int x, float height, unsigned int z)
{
	if(dynamic && x<size && z<size)
	{
		vertices[x*size+z].y=height;
		VBOvalid=false;
		minMaxValid = false;
		texValid = false;
	}
}
void Level::heightmap::increaseHeight(unsigned int x, float height, unsigned int z)
{
	if(dynamic && x<size && z<size)
	{
		vertices[x*size+z].y += height;
		VBOvalid = false;
		minMaxValid = false;
		texValid = false;
	}
}
float Level::heightmap::getHeight(unsigned int x, unsigned int z) const
{
	if(x<size && z<size)
		return vertices[x*size+z].y;
	return 0.0f;
}
//void Level::heightmap::draw(int shader) const
//{
//
//	if(!VBOvalid && dynamic)
//		setVBO();
//	if(!minMaxValid && dynamic)
//		setMinMaxHeights();
//	if(!texValid && dynamic)
//		setTex();
//	if(shader != 0)
//	{
//		glUseProgram(shader);
//		dataManager.bind("sand",0);
//		dataManager.bind("grass",1);
//		dataManager.bind("rock",2);
//		dataManager.bind("snow",3);
//		dataManager.bind("LCnoise",4);
//		dataManager.bind("hardNoise",5);
//		dataManager.bindTex(groundTex,6);
//
//		glUniform1f(glGetUniformLocation(shader, "maxHeight"),	maxHeight);
//		glUniform1f(glGetUniformLocation(shader, "minHeight"),	minHeight + (maxHeight-minHeight)/3);
//		glUniform1f(glGetUniformLocation(shader, "XZscale"),	size);
//		glUniform1f(glGetUniformLocation(shader, "time"),		gameTime());
//		glUniform1i(glGetUniformLocation(shader, "sand"),		0);
//		glUniform1i(glGetUniformLocation(shader, "grass"),		1);
//		glUniform1i(glGetUniformLocation(shader, "rock"),		2);
//		glUniform1i(glGetUniformLocation(shader, "snow"),		3);
//		glUniform1i(glGetUniformLocation(shader, "LCnoise"),	4);
//		glUniform1i(glGetUniformLocation(shader, "normalMap"),	5);
//
//		glPushMatrix();
//		glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex);
//		
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glVertexPointer(3, GL_FLOAT, 0, 0);
//		glDrawElements(GL_TRIANGLE_STRIP, numIdices, GL_UNSIGNED_INT, 0);
//		glDisableClientState(GL_VERTEX_ARRAY);
//		
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glPopMatrix();
//		
//		dataManager.bindTex(0,6);
//		dataManager.bindTex(0,5);
//		dataManager.bindTex(0,4);
//		dataManager.bindTex(0,3);
//		dataManager.bindTex(0,2);
//		dataManager.bindTex(0,1);
//		dataManager.bindTex(0,0);
//		glUseProgram(0);
//
//	}
//	else
//	{
//
//		glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex);
//
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glDrawElements(GL_TRIANGLE_STRIP, numIdices, GL_UNSIGNED_INT, 0);
//		glDisableClientState(GL_VERTEX_ARRAY);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//	}
//}
unsigned int Level::heightmap::getSize() const
{
	return size;
}
void Level::heightmap::setMinMaxHeights() const
{
	maxHeight=vertices[0].y+0.001;
	minHeight=vertices[0].y;
	for(int x=0;x<size;x++)
	{
		for(int z=0;z<size;z++)
		{
			if(vertices[x*size+z].y>maxHeight) maxHeight=vertices[x*size+z].y;
			if(vertices[x*size+z].y<minHeight) minHeight=vertices[x*size+z].y;
		}
	}
	minMaxValid=true;
}
//__________________________________________________________________________________________________________________________________________________________//

Level::Level(LevelFile file)
{
	mGround = new heightmapGL(file.mapSize,file.heights);
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
	mGround = new heightmapGL(Vector2<unsigned int>(image->height,image->width),t);
	delete[] t;
	delete image;
}
const Level::heightmapBase* const Level::ground() const
{
	return mGround;
}
void Level::render()
{
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
		mWater.seaLevel = 0;//mGround->minHeight + (mGround->maxHeight-mGround->minHeight)/3;

		int s=dataManager.getId("ocean");
		dataManager.bind("ocean");

		dataManager.bind("hardNoise",0);
		//dataManager.bindTex(mGround->groundTex,1);
		dataManager.bind("rock",2);
		dataManager.bind("sand",3);

		glUniform1i(glGetUniformLocation(s, "bumpMap"), 0);
		glUniform1i(glGetUniformLocation(s, "groundTex"), 1);
		glUniform1i(glGetUniformLocation(s, "rock"), 2);
		glUniform1i(glGetUniformLocation(s, "sand"), 3);
		glUniform1f(glGetUniformLocation(s, "time"), gameTime());
		//glUniform1f(glGetUniformLocation(s, "seaLevel"), (mWater.seaLevel-mGround->minHeight)/(mGround->maxHeight-mGround->minHeight));
		glUniform1f(glGetUniformLocation(s, "XZscale"), mGround->size.x);

		glBegin(GL_QUADS);
			glVertex3f(0,mWater.seaLevel,0);
			glVertex3f(0,mWater.seaLevel,mGround->size.z-1);
			glVertex3f(mGround->size.x-1,mWater.seaLevel,mGround->size.z-1);
			glVertex3f(mGround->size.x-1,mWater.seaLevel,0);
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

	//float h = mSettings.water ? mWater.seaLevel : mGround->minHeight-20.0;
	//dataManager.bind("layers");
	//glDisable(GL_LIGHTING);
	//glColor3f(1,1,1);
	//glBegin(GL_TRIANGLE_STRIP);
	//for(int i = 0; i < mGround->size-1; i++)
	//{
	//	glTexCoord2f(0.2*i,(mGround->vertices[i*mGround->size+0].y-h)/256);	glVertex3f(i,h,0);
	//	glTexCoord2f(0.2*i,(mGround->vertices[i*mGround->size+0].y-h)/1024);		glVertex3f(i,max(mGround->vertices[i*mGround->size+0].y,h) ,0);
	//}
	//for(int i = 0; i < mGround->size-1; i++)
	//{
	//	glTexCoord2f(0.2*i,(mGround->vertices[(mGround->size-1)*mGround->size+i].y-h)/256);	glVertex3f(mGround->size-1,h,i);
	//	glTexCoord2f(0.2*i,(mGround->vertices[(mGround->size-1)*mGround->size+i].y-h)/1024);		glVertex3f(mGround->size-1,max(mGround->vertices[(mGround->size-1)*mGround->size+i].y,h),i);
	//}
	//for(int i = mGround->size-1; i > 0; i--)
	//{
	//	glTexCoord2f(0.2*i,(mGround->vertices[i*mGround->size+mGround->size-1].y-h)/256);	glVertex3f(i,h,mGround->size-1);
	//	glTexCoord2f(0.2*i,(mGround->vertices[i*mGround->size+mGround->size-1].y-h)/1024);		glVertex3f(i,max(mGround->vertices[i*mGround->size+mGround->size-1].y,h),mGround->size-1);
	//}
	//for(int i = mGround->size-1; i >= 0; i--)
	//{
	//	glTexCoord2f(0.2*i,(mGround->vertices[0*mGround->size+i].y-h)/256);	glVertex3f(0,h,i);
	//	glTexCoord2f(0.2*i,(mGround->vertices[0*mGround->size+i].y-h)/1024);		glVertex3f(0,max(mGround->vertices[0*mGround->size+i].y,h),i);
	//}
	//glEnd();
	//dataManager.bindTex(0);
	//if(!mSettings.water)
	//{
	//	glColor3f(0.73,0.6,0.47);
	//	glBegin(GL_QUADS);
	//		glVertex3f(0,h,0);
	//		glVertex3f(0,h,mGround->size-1);
	//		glVertex3f(mGround->size-1,h,mGround->size-1);
	//		glVertex3f(mGround->size-1,h,0);
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
vector<Level::zone>* editLevel::zones()
{
	return &mZones;
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
		mGround = new heightmapGL(Vector2<unsigned int>(x,z),heights);
	else
		mGround = new heightmapGL(Vector2<unsigned int>(x,z));
}