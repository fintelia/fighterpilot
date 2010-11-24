//#include "main.h"
#include <fstream>
#include "windows.h"
#include "GL/glee.h"
#include <GL/glu.h>
#include "texture2.h" 
using namespace std;
extern double GetTime();

typedef struct
{
	unsigned char* imageData;	// Hold All The Color Values For The Image.
	GLuint bpp; 				// Hold The Number Of Bits Per Pixel.			
	GLuint width;				// The Width Of The Entire Image.	
	GLuint height;				// The Height Of The Entire Image.	
	GLuint texID;				// Texture ID For Use With glBindTexture.	
	GLuint type;			 	// Data Stored In * ImageData (GL_RGB Or GL_RGBA)
} Texture;

typedef struct
{
	GLubyte Header[12];			// File Header To Determine File Type
} TGAHeader;

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

GLuint LoadBitMap(char *filename,bool NPOT)
{
	double t=GetTime();
	static bool ext_NPOT = (bool)GLEE_ARB_texture_non_power_of_two;//0!=strstr((char*)glGetString(GL_EXTENSIONS), "GL_ARB_texture_non_power_of_two");
	GLuint texV[1]={-1};
    int j=0; //Index variables
    //unsigned char *l_texture; //The pointer to the memory zone in which we will load the texture
	
	Texture texture;

	TGAHeader tgaheader;				// Used To Store Our File Header
	TGA tga;					// Used To Store File Information
	// Uncompressed TGA Header
	GLubyte uTGAcompare[12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
	ifstream fin(filename, ios::binary);
    fin.read((char *)(&tgaheader), sizeof(tgaheader));
	//if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) != 0)
	//{
	////	fin.close();
	////	return -1;
	//}

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
		return -1;
	}

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
		return -1;
	}
	fin.read((char *)(texture.imageData), tga.imageSize);
	//Byte Swapping Optimized By Steve Thomas
	for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		texture.imageData[cswap] ^= texture.imageData[cswap+2] ^= texture.imageData[cswap] ^= texture.imageData[cswap+2];
	}
	//for(i=0;i<(signed int)tga.imageSize;i+=tga.bytesPerPixel)
	//{
	//	if(texture.imageData[i+0]==255 && texture.imageData[i+1]==0 && texture.imageData[i+2]==0)
	//	{
	//		texture.imageData[i+0]=(unsigned char)r;
	//		texture.imageData[i+1]=(unsigned char)g;
	//		texture.imageData[i+2]=(unsigned char)b;
	//	}
	//}
	
	fin.close();

	t=GetTime()-t;
	t=GetTime();
	
	glGenTextures(1,texV);
    glBindTexture(GL_TEXTURE_2D, texV[0]); // Bind the ID texture specified by the 2nd parameter

		t=GetTime()-t;
	t=GetTime();

	// Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4 , tga.Width, tga.Height, 0,texture.type, GL_UNSIGNED_BYTE, (GLvoid *)texture.imageData);

		t=GetTime()-t;
	t=GetTime();

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(ext_NPOT && NPOT)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // We don't combine the color with the original surface color, use only the texture map.

		t=GetTime()-t;
	t=GetTime();

	if(ext_NPOT && NPOT)
		glTexImage2D(GL_TEXTURE_2D,0, texture.type, tga.Width, tga.Height,0, texture.type, GL_UNSIGNED_BYTE, texture.imageData);
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tga.Width, tga.Height, texture.type, GL_UNSIGNED_BYTE, texture.imageData);

		t=GetTime()-t;
	t=GetTime();

	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    free(texture.imageData); // Free the memory we used to load the texture
	t=GetTime()-t;
	t=GetTime();
    return (texV[0]); // Returns the current texture OpenGL ID
}
struct MMPheader
{
	int size_x;
	int size_y;
	int channels;
};
GLuint loadMMP(char *filename)
{
	MMPheader h;
	ifstream fin;
	fin.open(filename,ios::binary);
	if(!fin.is_open()) return -1;
	fin.read((char*)&h,sizeof(h));
	if(h.channels != 3 && h.channels != 4) return -1;


	unsigned char* imageData=(unsigned char*)malloc(h.size_x*h.size_y*h.channels);
	if(imageData==NULL) return -1;

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