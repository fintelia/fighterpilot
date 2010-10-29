
class mapBuilder: public modes
{
private:
	Angle rot;
	editLevel* level;
	float maxHeight;
	float minHeight;

	void diamondSquare(float h)//mapsize must be (2^x+1, 2^x+1)!!!
	{
		int v=level->ground()->getSize();
		if( !(!((v-1) & (v-2) ) && v > 1) )//if v is not one more than a power of 2
		{
			v--;
			v |= v >> 1;
			v |= v >> 2;
			v |= v >> 4;
			v |= v >> 8;
			v |= v >> 16;
			v+=2;//changes mapsize to one more than a power of 2
			level->newGround(v);
		}

		
		//set corners

		int x, z, numSquares, squareSize;

		float c[4];

		h=pow(2.0f,-h);
		float rVal=10*(1.0-h), y;
		

		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				level->ground()->setHeight(x,0,y);
			}
		}

		level->ground()->setHeight(0,0,0);
		level->ground()->setHeight(0,0,level->ground()->getSize()-1);
		level->ground()->setHeight(level->ground()->getSize()-1,0,level->ground()->getSize()-1);
		level->ground()->setHeight(level->ground()->getSize()-1,0,0);

		for(int itt=0; (0x1 << itt) < (level->ground()->getSize()-1);itt++, rVal*=1.0-h)
		{
			numSquares = 0x1 << itt;
			squareSize = (level->ground()->getSize()-1)/numSquares;
			//diamond
			for(x = 0; x < numSquares; x++)
			{
				for(z = 0; z < numSquares; z++)
				{
					y = (level->ground()->getHeight(x*squareSize,z*squareSize)+
						level->ground()->getHeight(x*squareSize+squareSize,z*squareSize)+
						level->ground()->getHeight(x*squareSize+squareSize,z*squareSize+squareSize)+
						level->ground()->getHeight(x*squareSize,z*squareSize+squareSize))/4;
					y += float(rand()%2000-1000)/500.0f*rVal;

					level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize+squareSize/2);
				}
			}
			//square
			for(x = 0; x < numSquares; x++)
			{
				for(z = 0; z < numSquares; z++)
				{
					//left
					c[0]=level->ground()->getHeight(x*squareSize,z*squareSize);
					c[1]=level->ground()->getHeight(x*squareSize,z*squareSize+squareSize);
					c[2]=level->ground()->getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					c[3]=level->ground()->getHeight(x*squareSize-squareSize/2,z*squareSize+squareSize/2);
					if(x==0)			y=(c[0]+c[1]+c[2])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
					else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
					level->ground()->setHeight(x*squareSize,y,z*squareSize+squareSize/2);


					//top
					c[0]=level->ground()->getHeight(x*squareSize,z*squareSize);
					c[1]=level->ground()->getHeight(x*squareSize+squareSize,z*squareSize);
					c[2]=level->ground()->getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					c[3]=level->ground()->getHeight(x*squareSize+squareSize/2,z*squareSize-squareSize/2);
					if(z==0)			y=(c[0]+c[1]+c[2])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
					else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
					level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize);

					if(x == numSquares-1)//right
					{
						c[0]=level->ground()->getHeight((x+1)*squareSize,z*squareSize);
						c[1]=level->ground()->getHeight((x+1)*squareSize,z*squareSize+squareSize);
						//c[2]=getHeight((x+1)*squareSize+squareSize/2,z*squareSize+squareSize/2);
						c[3]=level->ground()->getHeight((x+1)*squareSize-squareSize/2,z*squareSize+squareSize/2);
						y=(c[0]+c[1]+c[3])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
						//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
						level->ground()->setHeight(x*squareSize,y,z*squareSize+squareSize/2);
					}
					if(z == numSquares-1)//buttom
					{
						c[0]=level->ground()->getHeight(x*squareSize,z*squareSize);
						c[1]=level->ground()->getHeight(x*squareSize+squareSize,z*squareSize);
						//c[2]=getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
						c[3]=level->ground()->getHeight(x*squareSize+squareSize/2,z*squareSize-squareSize/2);
						y=(c[0]+c[1]+c[3])/3	  +	float(rand()%2000-1000)/500.0f*rVal;
						//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
						level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize);
					}
				}
			}
			//rVal*=h;
		}
		//for(x = 0; x < level->ground()->getSize(); x++)
		//	level->ground()->setHeight(x,getHeight(x,0),level->ground()->getSize()-2);
		//for(z = 0; z < level->ground()->getSize(); z++)
		//	level->ground()->setHeight(level->ground()->getSize()-2,getHeight(x,z),z);
		maxHeight=-9999;
		minHeight=9999;
		float h;
		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				h = level->ground()->getHeight(x,y);
				if(h>maxHeight) maxHeight=h;
				if(h<minHeight) minHeight=h;
			}
		}
	}
	void faultLine()
	{
		float pd;
		float a,b,c;
		float disp=0.5;
		float x,y;
		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				level->ground()->setHeight(x,0,y);
			}
		}
		for(int i=0;i<100;i++)
		{
			a = float(rand()%1000)/500.0-1.0;
			b = float(rand()%1000)/500.0-1.0;
			c = rand()%level->ground()->getSize()-level->ground()->getSize()/2;
			for(x=0;x<level->ground()->getSize();x++)
			{
				for(y=0;y<level->ground()->getSize();y++)
				{
					pd = ((x-level->ground()->getSize()/2) * a + (y-level->ground()->getSize()/2) * b + c)/(128.0/30)/2;//  ***/2500
					if (pd > 1.57) pd = 1.57;
					else if (pd < 0.05) pd = 0.05;
					level->ground()->increaseHeight(x,-disp/2 + sin(pd)*disp,y);
				}
			}		
		}
		maxHeight=-9999;
		minHeight=9999;
		float h;
		for(x=0;x<level->ground()->getSize();x++)
		{
			for(y=0;y<level->ground()->getSize();y++)
			{
				h = level->ground()->getHeight(x,y);
				if(h>maxHeight) maxHeight=h;
				if(h<minHeight) minHeight=h;
			}
		}
		level->ground()->setMinMaxHeights();
	}
	void fromFile(string filename)
	{
		string ext=filesystem::extension(filename);
		if(ext.compare(".bmp")==0)
		{
			Image* image = loadBMP(filename.c_str());
			float* t = new float[image->width * image->height];
			for(int y = 0; y < image->height; y++) {
				for(int x = 0; x < image->width; x++) {
					t[y * image->width + x] = 30.0 * (((unsigned char)image->pixels[3 * (y * image->width + x)] / 255.0f) - 0.5f);
				}
			}
			assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL"); 
			level->newGround(image->height,t);
			delete[] t;
			delete image;
		}
	}
	vector<int> shaderButtons;
	friend class menuLevelEditor;
public:
	virtual int update(float value)
	{
		if(input->getKey(0x52) && !menuManager.getMenu()->popupActive())//r key
			rot+=value/1000;
		Redisplay=true;
		menuManager.update();
		return 15;
	}
	virtual void draw() 
	{
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.5f,0.8f,0.9f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, sw, sh);
		gluPerspective(80.0, (double)sw / ((double)sh),10.0, 25000.0);
		Vec3f e = Vec3f(level->ground()->getSize()/2+level->ground()->getSize()/2*sin(rot), 50, level->ground()->getSize()/2+level->ground()->getSize()/2*cos(rot))*size;
		Vec3f c = Vec3f(level->ground()->getSize()/2,0,level->ground()->getSize()/2)*size;
		gluLookAt(e.x,e.y,e.z,	c.x,c.y,c.z,	0,1,0);
		

		glEnable(GL_LIGHTING);
		GLfloat lightPos0[] = {-0.3f, -0.3f, -0.4f, 0.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		
		//level->settings()->water = ((menuLevelEditor*)menuManager.getMenu())->bMapType->getValue() == 0;

		if(((menuLevelEditor*)menuManager.getMenu())->getShader() != -1)
			level->ground()->setShader(shaderButtons[((menuLevelEditor*)menuManager.getMenu())->getShader()]);
		level->render();

		glDisable(GL_LIGHTING);
		//glDisable(GL_DEPTH_TEST);
		//viewOrtho(sw,sh);
		//menuManager.render();
		//viewPerspective();
	}
	mapBuilder(): rot(0), level(NULL), maxHeight(0), minHeight(0) {}
	void init()
	{
		menuManager.setMenu("menuLevelEditor");
		//if(Cmenu != NULL) delete Cmenu;
		//Cmenu=new closedMenu;

		if(level != NULL) delete level;
		level = new editLevel;

		rot=0;
		Redisplay=true;
		newMode=(modeType)0;
		level->newGround(65);
		faultLine();
	}
};