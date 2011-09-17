
#include "engine.h"

DataManager::~DataManager()
{
	shutdown();
}
bool DataManager::registerFont(string name, string filename) //loads a "text" .fnt file as created by Bitmap Font Generator from http://www.angelcode.com/products/bmfont/
{
	//todo: add code
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
	


	ifstream fin(filename, ios::binary);
	if(!fin.is_open()) return false;
	fin.read((char*)(&info), sizeof(info));

	string s;	
	getline(fin,s);
	getline(fin,s);
	sscanf(s.c_str(),"common lineHeight=%d base=%d scaleW=%d scaleH=%d", &info.lineHeight, &info.base, &info.width, &info.height);

	getline(fin,s);
	char texturePath[256];
	sscanf(s.c_str(),"page id=0 file=\"%s", texturePath);
	string texPath(texturePath);
	trim_right_if(texPath,is_any_of(" \"\t\n")); 

	int dLoc=filename.find_last_of("/\\");
	if(dLoc!=string::npos)	texPath = filename.substr(0,dLoc+1) + texPath;


	getline(fin,s);

	int numChars=0;
	sscanf(s.c_str(),"chars count=%d", &numChars);

	fontChar* fontChars = new fontChar[numChars];

	int i=0;
	while(!fin.eof() && i < numChars)
	{
		getline(fin, s);
		
		s.erase(remove_if(s.begin(), s.end(), isspace), s.end());

		int n = sscanf(s.c_str(),"char id=%dx=%dy=%dwidth=%dheight=%dxoffset=%dyoffset=%dxadvance=%d", &fontChars[i].id, &fontChars[i].x, &fontChars[i].y, &fontChars[i].w, &fontChars[i].h, &fontChars[i].xOffset, &fontChars[i].yOffset, &fontChars[i].xAdvance);

		i++;
	}
	if(i < numChars) numChars = i;

	string texName = name+"_TEXTURE";
	if(registerPNG(texName, texPath))
	{
		fontAsset* f = new fontAsset;
		f->id = assets[texName]->id;
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

		assets[name] = f;

		delete [] fontChars;
		return true;
	}
	else
	{
		delete [] fontChars;
		return false;
	}
}
bool DataManager::registerTGA(string name, string filename)
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
		return FALSE;
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
		return false;
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
	textureAsset* a = new textureAsset;
	a->id = texV;
	a->type = asset::TEXTURE;
	a->width = tga.Width;
	a->height = tga.Height;
	a->bpp = tga.Bpp;
	a->data = NULL;
	assets[name] = a;
	return true;
}
bool DataManager::registerPNG(string name, string filename)
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
		return false;
	}
	
	unsigned char sig[8];
	/* Check for the 8-byte signature */
	fread(sig, 1, 8, infile);
	if (!png_check_sig((unsigned char *) sig, 8)) {
		fclose(infile);
		return false;
	}
	/* 
	 * Set up the PNG structs 
	 */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(infile);
		return false; /* out of memory */
	}
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(infile);
		return false; /* out of memory */
	}
	
	png_infop end_ptr = png_create_info_struct(png_ptr);
	if (!end_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(infile);
		return false; /* out of memory */
	}
	
	/*
	 * block to handle libpng errors, 
	 * then check whether the PNG file had a bKGD chunk
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
		fclose(infile);
		return false;
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
		return false;
	}
	if ((row_pointers = (png_bytep*)malloc(height*sizeof(png_bytep))) == NULL) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return false;
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

	textureAsset* a = new textureAsset;
	a->id = texV;
	a->type = asset::TEXTURE;
	a->width = width;
	a->height = height;
	a->bpp = colorChannels*8;
	a->data = NULL;
	assets[name] = a;

	return true;
}

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
		if(boundShader == name)
			return;
		else if(boundShaderId == assets[name]->id)
		{
			boundShader = name;
			return;
		}

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

			assetFile tmpAssetFile;
			tmpAssetFile.type = asset::TEXTURE;
			while(textureElement != NULL)
			{
				c = textureElement->Attribute("name");	tmpAssetFile.name = c!=NULL ? c : "";
				c = textureElement->Attribute("file");	tmpAssetFile.filename[0] = c!=NULL ? c : "";

				if(tmpAssetFile.name != "" && tmpAssetFile.filename[0] != "")
				{
					const char* preload = textureElement->Attribute("preload");
					if(preload == NULL || string(preload) != "true")
						assetFiles.push(tmpAssetFile);
					else
						assetFilesPreload.push(tmpAssetFile);
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

			assetFile tmpAssetFile;
			tmpAssetFile.type = asset::SHADER;
			while(shaderElement != NULL)
			{
				c = shaderElement->Attribute("name");		tmpAssetFile.name = c!=NULL ? c : "";
				c = shaderElement->Attribute("vertex");		tmpAssetFile.filename[0] = c!=NULL ? c : "";
				c = shaderElement->Attribute("fragment");	tmpAssetFile.filename[1] = c!=NULL ? c : "";

				const char* use_sAspect = shaderElement->Attribute("sAspect");
				if(use_sAspect != NULL && string(use_sAspect) == "true")
					tmpAssetFile.options.insert("use_sAspect");

				if(tmpAssetFile.name != "" && tmpAssetFile.filename[0] != "" && tmpAssetFile.filename[1] != "")
				{
					const char* preload = shaderElement->Attribute("preload");
					if(preload == NULL || string(preload) != "true")
						assetFiles.push(tmpAssetFile);
					else
						assetFilesPreload.push(tmpAssetFile);
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

			assetFile tmpAssetFile;
			tmpAssetFile.type = asset::MODEL;
			while(modelElement != NULL)
			{
				c = modelElement->Attribute("name");	tmpAssetFile.name = c!=NULL ? c : "";
				c = modelElement->Attribute("file");	tmpAssetFile.filename[0] = c!=NULL ? c : "";
				
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
	if(node != NULL)
	{
		TiXmlElement* assetsElement		= NULL;
		TiXmlElement* assetElement		= NULL;

		assetsElement = node->ToElement();
		if(assetsElement != NULL)
		{
			node = assetsElement->FirstChildElement();
			if(node != NULL) assetElement = node->ToElement();

			assetFile tmpAssetFile;
			tmpAssetFile.type = asset::FONT;
			while(assetElement != NULL)
			{
				c = assetElement->Attribute("name");	tmpAssetFile.name = c!=NULL ? c : "";
				c = assetElement->Attribute("file");	tmpAssetFile.filename[0] = c!=NULL ? c : "";

				const char* preload = assetElement->Attribute("preload");
				if(preload == NULL || string(preload) != "true")
					assetFiles.push(tmpAssetFile);
				else
					assetFilesPreload.push(tmpAssetFile);

				if(tmpAssetFile.name !="" && tmpAssetFile.filename[0] != "")
				{
					const char* preload = assetElement->Attribute("preload");
					if(preload == NULL || string(preload) != "true")
						assetFiles.push(tmpAssetFile);
					else
						assetFilesPreload.push(tmpAssetFile);
				}
				else debugBreak();

				assetElement = assetElement->NextSiblingElement();
			}
		}
	}

	return true;
}
void DataManager::loadAssetFile(assetFile &file)
{
	if(file.type == asset::TEXTURE)
	{
		registerTexture(file.name, file.filename[0]);
	}
	else if(file.type == asset::SHADER)
	{
		if(registerShader(file.name, file.filename[0], file.filename[1]) && file.options.count("use_sAspect") != 0)
		{
			auto s = assets.find(file.name);
			((shaderAsset*)(s->second))->use_sAspect = true;
	
			bind(file.name);
			setUniform1f("sAspect",sAspect);
			unbind(file.name);
		}
	}
	else if(file.type == asset::MODEL)
	{
		registerOBJ(file.name, file.filename[0]);
	}
	else if(file.type == asset::FONT)
	{
		registerFont(file.name, file.filename[0]);
	}
}
void DataManager::preloadAssets()
{
	while(!assetFilesPreload.empty())
	{
		loadAssetFile(assetFilesPreload.front());
		assetFilesPreload.pop();
	}
}
int DataManager::loadAsset()
{
	if(!assetFiles.empty())
	{
		loadAssetFile(assetFiles.front());
		assetFiles.pop();
	}
	return assetFiles.size();
}
bool DataManager::registerTexture(string name, string filename)
{
	string ext=filesystem::extension(filename);
	if(ext.compare(".tga") == 0)		return registerTGA(name, filename);
	else if(ext.compare(".png") == 0)	return registerPNG(name, filename);
	else return false;
}
bool DataManager::registerShader(string name, string vert, string frag, bool use_sAspect)
{
	bool errorFlag = false;
	GLuint v=0,f=0,p;
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	char * ff = textFileRead((char*)frag.c_str());
	char * vv = textFileRead((char*)vert.c_str());
	if(ff == NULL || vv == NULL) return false;

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
	if(i == GL_FALSE && !errorFlag)
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
	if(i == GL_FALSE && !errorFlag)
	{
		glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
		char* cl=(char*)malloc(i); memset(cl,0,i);
		glGetProgramInfoLog(p,i,&i,cl);
		messageBox(frag + "(link): " + cl);
		errorFlag = true;
	}
	glUseProgram(0); 

	if(!errorFlag)
	{
		shaderAsset* a = new shaderAsset;
		a->id = p;
		a->type = asset::SHADER;
		a->use_sAspect = use_sAspect;
		assets[name] = a;
	}
	return !errorFlag;
}
bool DataManager::registerTerrainShader(string name, string frag)
{
	bool errorFlag = false;
	static GLuint v=0;
	
	if(v==0)
	{
		v = glCreateShader(GL_VERTEX_SHADER);
		char * vv = textFileRead("media/terrain.vert");
		if(vv == NULL) return false;
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
		if(i == GL_FALSE && !errorFlag)
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
		if(i == GL_FALSE && !errorFlag)
		{
			glGetProgramiv(p,GL_INFO_LOG_LENGTH,&i);
			char* cl=(char*)malloc(i); memset(cl,0,i);
			glGetProgramInfoLog(p,i,&i,cl);
			messageBox(frag + "(link): " + cl);
			errorFlag = true;
		}

	}
	glUseProgram(0);

	if(!errorFlag)
	{
		shaderAsset* a = new shaderAsset;
		a->id = p;
		a->type = asset::SHADER;
		a->use_sAspect = false;
		assets[name] = a;
	}
	return !errorFlag;
}
bool DataManager::registerOBJ(string name, string filename)
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
	if(fopen_s(&fp,filename.c_str(), "r"))
		return false;

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
					//string ext=(file+s[1]).substr((file+s[1]).find_last_of(".")+1);
					//if(ext.compare("tga")==0)
					mMtl.tex=mtlFile + "/" + s[1];
					if(!registerTexture(mMtl.tex, file+s[1]))
						mMtl.tex = "";
						//mMtl.tex=dataManager.loadPNG(file+s[1]);
						//registerTexture(mtlFile + "/" + s[1],mMtl.tex);
					//else if(ext.compare("mmp")==0)
					//	mMtl.tex=loadMMP( (char*)(file+s[1]).c_str());
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

	unsigned int* fs = new unsigned int[totalFaces*3];
	for(int itt = 0;itt<totalFaces;itt++)
	{
		fs[itt*3+0] = faces[itt].v[0];
		fs[itt*3+1] = faces[itt].v[1];
		fs[itt*3+2] = faces[itt].v[2];
	}
	modelAsset* a = new modelAsset;
	a->type = asset::MODEL;
	a->trl = std::shared_ptr<CollisionChecker::triangleList>(new CollisionChecker::triangleList(vertices,fs,totalVerts,totalFaces));

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	texturedLitVertex3D* VBOverts = new texturedLitVertex3D[totalFaces*3];
	unsigned int lNum=0, vNum = 0;
	for(int m=0; m<numMtls; m++)
	{
		for(int i=0; i < totalFaces; i++)
		{
			if(faces[m].material == m)
			{
				for(int vi=0; vi < 3; vi++, vNum++)
				{
					VBOverts[vNum].position	= vertices[faces[i].v[vi]];
					VBOverts[vNum].normal	= normals[faces[i].n[vi]];
					VBOverts[vNum].UV		= Vec2f(texCoords[faces[i].t[vi]].u,texCoords[faces[i].t[vi]].v);
				}
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

	glGenBuffers(1,(GLuint*)&a->id);
	glBindBuffer(GL_ARRAY_BUFFER, a->id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedLitVertex3D)*vNum, VBOverts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	assets[name] = a;

	delete[] fs;
	delete[] vertices;
	delete[] texCoords;
	delete[] faces;
	delete[] normals;
	delete[] mtls;
	
	return true;
}
int DataManager::getId(string name)
{
	if(assets.find(name)==assets.end())
		return 0;
	return assets[name]->id;
}
int DataManager::getId(objectType t)
{
	return getId(objectTypeString(t));
}
bool DataManager::assetLoaded(string name)
{
	return assets.find(name) != assets.end();
}
//CollisionChecker::triangleList* DataManager::getModel(objectType type)
//{
//	auto i = models.find(getId(type));
//	return i != models.end() ? i->second : NULL;
//}
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
		else if(i->second->type == asset::TEXTURE)		glDeleteTextures(1,(const GLuint*)&i->second->id);
	}
	assets.clear();
}