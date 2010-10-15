
#include "main.h"



void Level::heightmap::setVBO() const
{
	glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
	glBufferData(GL_ARRAY_BUFFER, numVerts*sizeof(vertex), vertices, GL_DYNAMIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	VBOvalid = true;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
}
Level::heightmap::heightmap(int Size, bool Dynamic): size(Size), dynamic(Dynamic), sand(0), rock(0), grass(0), snow(0), LCnoise(0), normals(0)
{
	float* tmpHeights=(float*)malloc(size*size*sizeof(float));
	memset(tmpHeights,0,size*size*sizeof(float));
	init(tmpHeights);
	free(tmpHeights);
}
Level::heightmap::heightmap(int Size, float* heights, bool Dynamic): size(Size), dynamic(Dynamic), sand(0), rock(0), grass(0), snow(0), LCnoise(0), normals(0)
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
	}
}
void Level::heightmap::increaseHeight(unsigned int x, float height, unsigned int z)
{
	if(dynamic && x<size && z<size)
	{
		vertices[x*size+z].y += height;
		VBOvalid = false;
		minMaxValid = false;
	}
}
float Level::heightmap::getHeight(unsigned int x, unsigned int z) const
{
	if(x<size && z<size)
		return vertices[x*size+z].y;
	return 0.0f;
}
void Level::heightmap::draw(int shader) const
{
	if(!VBOvalid && dynamic)
		setVBO();
	if(!minMaxValid && dynamic)
		setMinMaxHeights();
	if(shader != 0)
	{
		int textures[6]={0,1,2,3,4,5};
		glUseProgram(shader);
		dataManager.bind("sand",0);
		dataManager.bind("grass",1);
		dataManager.bind("rock",2);
		dataManager.bind("snow",3);
		dataManager.bind("LCnoise",4);
		dataManager.bind("hardNoise",5);

		glUniform1f(glGetUniformLocation(shader, "maxHeight"),	maxHeight);
		glUniform1f(glGetUniformLocation(shader, "minHeight"),	minHeight);
		glUniform1f(glGetUniformLocation(shader, "XZscale"),	size);
		glUniform1f(glGetUniformLocation(shader, "time"),		gameTime());
		glUniform1f(glGetUniformLocation(shader, "sea_level"),	(maxHeight-minHeight)/3+minHeight);
		glUniform1i(glGetUniformLocation(shader, "sand"),		textures[0]);
		glUniform1i(glGetUniformLocation(shader, "grass"),		textures[1]);
		glUniform1i(glGetUniformLocation(shader, "rock"),		textures[2]);
		glUniform1i(glGetUniformLocation(shader, "snow"),		textures[3]);
		glUniform1i(glGetUniformLocation(shader, "LCnoise"),	textures[4]);
		glUniform1i(glGetUniformLocation(shader, "waterBumpMap"),textures[5]);

		glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawElements(GL_TRIANGLE_STRIP, numIdices, GL_UNSIGNED_INT, 0);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);



		dataManager.bindTex(0,5);
		dataManager.bindTex(0,4);
		dataManager.bindTex(0,3);
		dataManager.bindTex(0,2);
		dataManager.bindTex(0,1);
		dataManager.bindTex(0,0);
		glUseProgram(0);
	}
	else
	{

		glBindBuffer(GL_ARRAY_BUFFER, VBOvert);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOindex);

		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawElements(GL_TRIANGLE_STRIP, numIdices, GL_UNSIGNED_INT, 0);
		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}
unsigned int Level::heightmap::getSize() const
{
	return size;
}
void Level::heightmap::setMinMaxHeights() const
{
	maxHeight=vertices[0].y;
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
	mGround = new heightmap(file.mapSize,file.heights);
}
Level::Level(string bmp)
{
	Image* image = loadBMP(bmp.c_str());
	float* t = new float[image->width * image->height];
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color = (unsigned char)image->pixels[3 * (y * image->width + x)];
			t[y * image->width + x] = settings.HEIGHT_RANGE * ((color / 255.0f) - 0.5f);
		}
	}
	assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL"); 
	mGround = new heightmap(image->height,t,true);
	delete[] t;
	delete image;
}
const Level::heightmap* const Level::ground() const
{
	return mGround;
}
void Level::render()
{
	glPushMatrix();
	glScalef(size,1,size);
	mGround->draw(dataManager.getId("grass new terrain"));
	glPopMatrix();
}
//__________________________________________________________________________________________________________________________________________________________//

editLevel::editLevel()
{

}
Level::heightmap* editLevel::ground()
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
void editLevel::newGround(unsigned int size, float* heights)
{
	if(mGround != NULL)
		delete mGround;
	if(heights != NULL)
		mGround = new heightmap(size,heights,true);
	else
		mGround = new heightmap(size,true);
}