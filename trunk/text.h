#pragma once

//extern int fontBase;
//void RenderText(float x,float y,string String);
//void RenderText(float x,float y,float number);
//void RenderText(float x,float y,int number);
//void RenderAAText(float x,float y,string String);
//void RenderAAText(float x,float y,float number);
//void RenderAAText(float x,float y,int number);
//char *textFileRead(char *fn);
//bool checkExtension(char* extensions, char* checkFor);
//
extern double GetTime();

class TextManager
{
private:
	int textureId;
	int textureWidth;
	int textureHeight;
	struct character
	{
		unsigned short x;
		unsigned short y;
		unsigned short width;
		unsigned short height;
	};

	character chars[128];

	void renderCharacter(char c, int x, int y)
	{
		if(c<32 || c>=128) return;

		double	cx=(double)chars[c].x/textureWidth+0.001,
				cy=(double)chars[c].y/textureWidth+0.001,
				cwidth=(double)chars[c].width/textureWidth-0.002,
				cheight=(double)chars[c].height/textureWidth-0.002;

		glBindTexture(GL_TEXTURE_2D, textureId);
		glBegin(GL_QUADS);
			glTexCoord2d(cx,cy);				glVertex2i(x,y);
			glTexCoord2d(cx+cwidth,cy);			glVertex2i(x+chars[c].width,y);
			glTexCoord2d(cx+cwidth,cy+cheight);	glVertex2i(x+chars[c].width,y+chars[c].height);
			glTexCoord2d(cx,cy+cheight);		glVertex2i(x,y+chars[c].height);
		glEnd();
	}
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
		GLuint bytesPerPixel;			// Number Of BYTES Per Pixel (3 Or 4)
		GLuint imageSize;			// Amount Of Memory Needed To Hold The Image
		GLuint type;				// The Type Of Image, GL_RGB Or GL_RGBA
		GLuint Height;				// Height Of Image					
		GLuint Width;				// Width Of Image				
		GLuint Bpp;				// Number Of BITS Per Pixel (24 Or 32)
	} TGA;
	void loadTGA(string filename)
	{
		GLuint texV[1]={-1};
		int j=0;
	
		Texture texture;

		TGAHeader tgaheader;				// Used To Store Our File Header
		TGA tga;					// Used To Store File Information
		GLubyte uTGAcompare[12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
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
			__debugbreak();
			return;
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
			__debugbreak();
			return;
		}
		fin.read((char *)(texture.imageData), tga.imageSize);
		//Byte Swapping Optimized By Steve Thomas
		for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
		{
			texture.imageData[cswap] ^= texture.imageData[cswap+2] ^= texture.imageData[cswap] ^= texture.imageData[cswap+2];
		}
	
		fin.close();
	
		glGenTextures(1,texV);
		glBindTexture(GL_TEXTURE_2D, texV[0]); // Bind the ID texture specified by the 2nd parameter


		// Finally we define the 2d texture
		glTexImage2D(GL_TEXTURE_2D, 0, 4 , tga.Width, tga.Height, 0,texture.type, GL_UNSIGNED_BYTE, (GLvoid *)texture.imageData);

		// The next commands sets the texture parameters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // We don't combine the color with the original surface color, use only the texture map.
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tga.Width, tga.Height, texture.type, GL_UNSIGNED_BYTE, texture.imageData);

		free(texture.imageData);
		textureId = texV[0];
		textureWidth = texture.width;
		textureHeight = texture.height;
	}
	void loadXML(string filename)
	{
		double t=GetTime();
		int charId;
		string line;
		ifstream fin(filename, ios::in);
		char_separator<char> sep(" <>=\t\"", "");
		tokenizer<char_separator<char> >::iterator i;
		tokenizer<char_separator<char> >::iterator end;
		string tok;

		enum {CHAR_ID,X,Y,WIDTH,HEIGHT,VALUE} lastToken=VALUE;
		if(fin.is_open())
		{
			while(!fin.eof())
			{
				getline(fin,line);
				tokenizer<char_separator<char> > Tokenizer(line, sep);
				end=Tokenizer.end();
				for (i = Tokenizer.begin(); i != end; i++)
				{

					tok=*i;
					if(lastToken==CHAR_ID)							charId=lexical_cast<int>(tok);
					else if(lastToken==X)							chars[charId].x=lexical_cast<int>(tok);
					else if(lastToken==Y)							chars[charId].y=lexical_cast<int>(tok);
					else if(lastToken==WIDTH)						chars[charId].width=lexical_cast<int>(tok);
					else if(lastToken==HEIGHT)						chars[charId].height=lexical_cast<int>(tok);
					lastToken = VALUE;

					if(tok.compare("id") == 0)				lastToken=CHAR_ID;
					else if(tok.compare("x") == 0)			lastToken=X;
					else if(tok.compare("y") == 0)			lastToken=Y;
					else if(tok.compare("width") == 0)		lastToken=WIDTH;
					else if(tok.compare("height") == 0)		lastToken=HEIGHT;
				}
			}
		}
		t = GetTime()-t;
		t = GetTime();
	}

public:
	TextManager(): textureId(0), textureWidth(0), textureHeight(0)
	{
		memset(chars,0,sizeof(chars));
	}
	void clear()
	{
		textureId=0;
		textureWidth=0;
		textureHeight=0;
		memset(chars,0,sizeof(chars));
	}
	void init(string filename)
	{
		double t=GetTime();
		clear();
		loadXML(filename + ".xml");
		loadTGA(filename + ".tga");
		t=GetTime()-t;
		t=GetTime();
	}
	void renderText(string text, int x, int y)
	{
		glDisable(GL_MULTISAMPLE);
		int xpos=x;
		for(string::iterator i = text.begin();i != text.end();i++)
		{
			if(*i>=32 && *i < 128)
			{
				renderCharacter(*i,xpos,y);
				xpos+=chars[*i].width;
			}
		}
		glEnable(GL_MULTISAMPLE);
	}
	int getTextWidth(string text)
	{
		int width=0;
		for(string::iterator i = text.begin();i != text.end();i++)
		{
			if(*i>=32 && *i < 128)	width+=chars[*i].width;
		}
		return width;
	}
	int getTextHeight(string text)
	{
		int height=0;
		for(string::iterator i = text.begin();i != text.end();i++)
		{
			if(*i>=32 && *i < 128 && chars[*i].height>height)	height=chars[*i].height;
		}
		return height;
	}
};

extern TextManager* textManager;