
#include "main.h"

void modeMapBuilder::zoom(float rotations)
{
	scroll = clamp(scroll + rotations,-8,25);
}
void modeMapBuilder::trackBallUpdate(int newX, int newY)
{
	int oldX = input->getMouseState(MIDDLE_BUTTON).x;
	int oldY = input->getMouseState(MIDDLE_BUTTON).y;
	if(newX==oldX && newY==oldY) return;

	Vec2f oldP(2.0*oldX/sw-sw/2.0,2.0*oldY/sh-sh/2.0);
	Vec2f newP(2.0*newX/sw-sw/2.0,2.0*newY/sh-sh/2.0);

	Vec3f xAxis = rot * Vec3f(-1,0,0);

	Vec3f axis = xAxis * (newP.y-oldP.y) + Vec3f(0,-1,0) * (newP.x-oldP.x);
	Angle ang = sqrt( (newP.x-oldP.x)*(newP.x-oldP.x) + (newP.y-oldP.y)*(newP.y-oldP.y) )/2.0;
	rot = Quat4f(axis,ang) * rot;
}
void modeMapBuilder::resetView()
{
	rot = Quat4f(Vec3f(1,0,0),1.0);
	center = Vec3f(level->ground()->sizeX()/2,minHeight,level->ground()->sizeZ()/2);
}
void modeMapBuilder::diamondSquare(float h, float m)//mapsize must be (2^x+1, 2^x+1)!!!
{
	int sLen=max(level->ground()->resolutionX(),level->ground()->resolutionZ());
	if( !(!((sLen-1) & (sLen-2) ) && sLen > 1) || level->ground()->resolutionX() != level->ground()->resolutionZ())//if v is not one more than a power of 2
	{
		sLen--;
		sLen |= sLen >> 1;
		sLen |= sLen >> 2;
		sLen |= sLen >> 4;
		sLen |= sLen >> 8;
		sLen |= sLen >> 16;
		sLen+=2;//changes mapsize to one more than a power of 2
		level->newGround(sLen,sLen);
	}

	
	//set corners

	int x, z, numSquares, squareSize;

	float c[4];

	//h=pow(2.0f,-h);
	float rVal=h, y;
		

	for(x=0;x<sLen;x++)
	{
		for(y=0;y<sLen;y++)
		{
			level->ground()->setHeight(x,0,y);
		}
	}

	//level->ground()->setHeight(0,0,0);
	//level->ground()->setHeight(0,0,sLen-1);
	//level->ground()->setHeight(sLen-1,0,level->ground()->resolutionZ()-1);
	//level->ground()->setHeight(sLen-1,0,0);

	for(int itt=3; (0x1 << itt) < (level->ground()->resolutionX()-1);itt++, rVal*=m)
	{
		numSquares = 0x1 << itt;
		squareSize = (sLen-1)/numSquares;
		//diamond
		for(x = 0; x < numSquares; x++)
		{
			for(z = 0; z < numSquares; z++)
			{
				y = (level->ground()->rasterHeight(x*squareSize,z*squareSize)+
					level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize)+
					level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize+squareSize)+
					level->ground()->rasterHeight(x*squareSize,z*squareSize+squareSize))/4;
				y += random(1000.0*rVal);

				level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize+squareSize/2);
			}
		}
		//square
		for(x = 0; x < numSquares; x++)
		{
			for(z = 0; z < numSquares; z++)
			{
				//left
				c[0]=level->ground()->rasterHeight(x*squareSize,z*squareSize);
				c[1]=level->ground()->rasterHeight(x*squareSize,z*squareSize+squareSize);
				c[2]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
				c[3]=level->ground()->rasterHeight(x*squareSize-squareSize/2,z*squareSize+squareSize/2);
				if(x==0)			y=(c[0]+c[1]+c[2])/3	  +	random(1000.0*rVal);
				else				y=(c[0]+c[1]+c[2]+c[3])/4 + random(1000.0*rVal);
				level->ground()->setHeight(x*squareSize,y,z*squareSize+squareSize/2);


				//top
				c[0]=level->ground()->rasterHeight(x*squareSize,z*squareSize);
				c[1]=level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize);
				c[2]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
				c[3]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize-squareSize/2);
				if(z==0)			y=(c[0]+c[1]+c[2])/3	  +	random(1000.0*rVal);
				else				y=(c[0]+c[1]+c[2]+c[3])/4 + random(1000.0*rVal);
				level->ground()->setHeight(x*squareSize+squareSize/2,y,z*squareSize);

				if(x == numSquares-1)//right
				{
					c[0]=level->ground()->rasterHeight((x+1)*squareSize,z*squareSize);
					c[1]=level->ground()->rasterHeight((x+1)*squareSize,z*squareSize+squareSize);
					//c[2]=getHeight((x+1)*squareSize+squareSize/2,z*squareSize+squareSize/2);
					c[3]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					y=(c[0]+c[1]+c[3])/3	  +	random(1000.0*rVal);
					//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
					level->ground()->setHeight((x+1)*squareSize,y,z*squareSize+squareSize/2);
				}
				if(z == numSquares-1)//bottom
				{
					c[0]=level->ground()->rasterHeight(x*squareSize,z*squareSize+squareSize);
					c[1]=level->ground()->rasterHeight(x*squareSize+squareSize,z*squareSize+squareSize);
					//c[2]=getHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					c[3]=level->ground()->rasterHeight(x*squareSize+squareSize/2,z*squareSize+squareSize/2);
					y=(c[0]+c[1]+c[3])/3	  +	random(1000.0*rVal);
					//else				y=(c[0]+c[1]+c[2]+c[3])/4 + float(rand()%2000-1000)/500.0f*rVal;
					level->ground()->setHeight(x*squareSize+squareSize/2,y,(z+1)*squareSize);
				}
			}
		}
		//rVal*=h;
	}
	//for(x = 0; x < level->ground()->getSize(); x++)
	//	level->ground()->setHeight(x,getHeight(x,0),level->ground()->getSize()-2);
	//for(z = 0; z < level->ground()->getSize(); z++)
	//	level->ground()->setHeight(level->ground()->getSize()-2,getHeight(x,z),z);
	//maxHeight=-9999;
	//minHeight=9999;
	//float h;
	//for(x=0;x<level->ground()->resolutionX();x++)
	//{
	//	for(y=0;y<level->ground()->resolutionZ();y++)
	//	{
	//		h = level->ground()->rasterHeight(x,y);
	//		if(h>maxHeight) maxHeight=h;
	//		if(h<minHeight) minHeight=h;
	//	}
	//}
	level->ground()->setMinMaxHeights();
	minHeight=level->ground()->getMinHeight();

	for(x=0;x<sLen;x++)
	{
		for(y=0;y<sLen;y++)
		{
			level->ground()->setHeight(x,(level->ground()->height(x,y)-minHeight)*clamp(2.0-2.0*sqrt((x-sLen/2)*(x-sLen/2)+(y-sLen/2)*(y-sLen/2))/(sLen/2),1.0,0.0),y);
		}
	}
	

	level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));



	level->ground()->setMinMaxHeights();
	maxHeight=level->ground()->getMaxHeight();
	minHeight=level->ground()->getMinHeight();
	((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.333);
	resetView();
}
void modeMapBuilder::faultLine()
{
	float pd;
	float a,b,c;
	float disp=0.5  * 100.0;
	float x,y;
	for(x=0;x<level->ground()->resolutionX();x++)
	{
		for(y=0;y<level->ground()->resolutionZ();y++)
		{
			level->ground()->setHeight(x,0,y);
		}
	}
	for(int i=0;i<100;i++)
	{
		a = random(-1.0f,1.0f);
		b = random(-1.0f,1.0f);
		c = random(-0.5f,0.5f) * level->ground()->resolutionX();
		for(x=0;x<level->ground()->resolutionX();x++)
		{
			for(y=0;y<level->ground()->resolutionZ();y++)
			{
				pd = ((x-level->ground()->resolutionX()/2) * a + (y-level->ground()->resolutionZ()/2) * b + c)/(128.0/30)/2;//  ***/2500
				if (pd > 1.57) pd = 1.57;
				else if (pd < 0.05) pd = 0.05;
				level->ground()->increaseHeight(x,-disp/2 + sin(pd)*disp,y);
			}
		}		
	}

	//float h;
	//for(x=0;x<level->ground()->resolutionX();x++)
	//{
	//	for(y=0;y<level->ground()->resolutionZ();y++)
	//	{
	//		h = level->ground()->rasterHeight(x,y);
	//		if(h>maxHeight) maxHeight=h;
	//		if(h<minHeight) minHeight=h;
	//	}
	//}

	level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));

	level->ground()->setMinMaxHeights();
	maxHeight=level->ground()->getMaxHeight();
	minHeight=level->ground()->getMinHeight();
	((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.333);
	resetView();
}
void modeMapBuilder::fromFile(string filename)
{
	string ext=filesystem::extension(filename);
	if(ext.compare(".bmp")==0)
	{
		Image* image = loadBMP(filename.c_str());
		float* t = new float[image->width * image->height];
		for(int y = 0; y < image->height; y++) {
			for(int x = 0; x < image->width; x++) {
				t[y * image->width + x] = (unsigned char)image->pixels[3 * (y * image->width + x)] * 10.0;
			}
		}
		//assert(image->height == image->width || "MAP WIDTH AND HEIGHT MUST BE EQUAL"); 
		level->newGround(image->height,image->width,t);
		level->ground()->setSize(Vec2f(level->ground()->resolutionX()*100,level->ground()->resolutionZ()*100));
		delete[] t;
		delete image;
	}
	
	level->ground()->setMinMaxHeights();
	maxHeight=level->ground()->getMaxHeight();
	minHeight=level->ground()->getMinHeight();
	((menu::levelEditor*)menuManager.getMenu())->sliders["sea level"]->setValue(0.333);
	resetView();
}
void modeMapBuilder::addObject(int type, int team, int controlType, int x, int y)//in screen coordinates
{
	double mViewMat[16];
	double projMat[16];
	int viewport[4]  = {0,0,sw,sh};
	glGetDoublev(GL_MODELVIEW_MATRIX,mViewMat);
	glGetDoublev(GL_PROJECTION_MATRIX,projMat);

	Vec3d P0, P1, dir;

	gluUnProject(x,sh-y,0.0,mViewMat,projMat,viewport,&P0.x,&P0.y,&P0.z);
	gluUnProject(x,sh-y,1.0,mViewMat,projMat,viewport,&P1.x,&P1.y,&P1.z);
	dir = P0-P1;

	if(abs(dir.y) < 0.001) return;
	Vec3d val = P1 + dir*(maxHeight+objPlacementAlt-P1.y)/dir.y;
	((editLevel*)level)->addObject(type,team,controlType,Vec3f(val.x,val.y,val.z));

}
int modeMapBuilder::update()
{
	//if(input->getKey(0x52) && !menuManager.getMenu()->popupActive())//r key
	//	rot+=value/1000;
	POINT p;
	GetCursorPos(&p);
	if(!input->getMouseState(MIDDLE_BUTTON).down && (p.x < 2 || p.x > sw-2 || p.y < 2 || p.y > sh-2))
	{
		if(p.x < 2)		center -= rot * Vec3f(0.25,0,0) * level->ground()->resolutionX() * size * pow(1.1f,-scroll) * world.time.length() / 1000 * pow(1.1f,-scroll);
		if(p.x > sw-2)	center += rot * Vec3f(0.25,0,0) * level->ground()->resolutionX() * size * pow(1.1f,-scroll) * world.time.length() / 1000 * pow(1.1f,-scroll);
		if(p.y < 2)		center -= rot * Vec3f(0,0,0.25) * level->ground()->resolutionZ() * size * pow(1.1f,-scroll) * world.time.length() / 1000 * pow(1.1f,-scroll);
		if(p.y > sh-2)	center += rot * Vec3f(0,0,0.25) * level->ground()->resolutionZ() * size * pow(1.1f,-scroll) * world.time.length() / 1000 * pow(1.1f,-scroll);
	}
	menuManager.update();
	return 7;
}
void modeMapBuilder::draw3D() 
{	
	gluPerspective(80.0, (double)sw / ((double)sh),10.0, 500000.0);
	static FrustumG frustum;
	frustum.setCamInternals(80.0, (double)sw / ((double)sh),10.0, 500000.0);
	

	Vec3f e,c,u;
	c = center;
	if(input->getMouseState(MIDDLE_BUTTON).down)
	{
		POINT p;
		GetCursorPos(&p);

		Vec2f oldP(2.0*input->getMouseState(MIDDLE_BUTTON).x/sw-sw/2.0,2.0*input->getMouseState(MIDDLE_BUTTON).y/sh-sh/2.0);
		Vec2f newP(2.0*p.x/sw-sw/2.0,2.0*p.y/sh-sh/2.0);
			
			
		Vec3f xAxis = rot * Vec3f(-1,0,0);

		Vec3f axis = (xAxis * (newP.y-oldP.y) + Vec3f(0,-1,0) * (newP.x-oldP.x)).normalize();
		Angle ang = sqrt( (newP.x-oldP.x)*(newP.x-oldP.x) + (newP.y-oldP.y)*(newP.y-oldP.y) )/2.0;

		Quat4f tmpRot;
		if(ang > 0.01)	tmpRot = Quat4f(axis,ang) * rot;
		else			tmpRot = rot;

		e = c + tmpRot * Vec3f(0,0.75,0) * max(level->ground()->sizeX(),level->ground()->sizeZ()) * pow(1.1f,-scroll);
		u = tmpRot * Vec3f(0,0,-1);
 
	}
	else
	{
		e = c + rot * Vec3f(0,0.75,0) * max(level->ground()->sizeX(),level->ground()->sizeZ()) * pow(1.1f,-scroll);
		u = rot * Vec3f(0,0,-1);
	}
	gluLookAt(e.x,e.y,e.z,	c.x,c.y,c.z,	u.x,u.y,u.z);
	frustum.setCamDef(e,c,u);

	GLfloat lightPos0[] = {-0.3f, 0.7f, -0.4f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	if(menus->getShader() != -1)
		((Level::heightmapGL*)level->ground())->setShader(menus->toggles["shaders"]->getValue() + 1);
	bool w = menus->getShader() != 1;
	float sl = menus->sliders["sea level"]->getValue();

	glPushMatrix();
	glScalef(1,pow(10.0f,menus->sliders["height scale"]->getValue()),1);
 	level->renderPreview(w,sl * (maxHeight - minHeight) + minHeight);
	glPopMatrix();

	if(menus->getTab() == menu::levelEditor::OBJECTS)
	{
		level->renderObjectsPreview();
		if(menus->placingObject() != 0)
		{
			////////////////////////////////draw object//////////////////////////////////
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			double mViewMat[16];
			double projMat[16];
			int viewport[4]  = {0,0,sw,sh};
			glGetDoublev(GL_MODELVIEW_MATRIX,mViewMat);
			glGetDoublev(GL_PROJECTION_MATRIX,projMat);

			Vec3d P0, P1, dir;

			gluUnProject(cursorPos.x,sh-cursorPos.y,0.0,mViewMat,projMat,viewport,&P0.x,&P0.y,&P0.z);
			gluUnProject(cursorPos.x,sh-cursorPos.y,1.0,mViewMat,projMat,viewport,&P1.x,&P1.y,&P1.z);
			dir = P0-P1;
				
			if(abs(dir.y) < 0.001) return;
			Vec3d val = P1 + dir*(maxHeight+objPlacementAlt-P1.y)/dir.y;
			glPushMatrix();
			glTranslatef(val.x,val.y,val.z);
			glScalef(10,10,10);
			dataManager.draw((planeType)((menu::levelEditor*)menuManager.getMenu())->placingObject());
			glPopMatrix();
			////////////////////////////////draw grid//////////////////////////////////
			glDepthMask(false);
			glDisable(GL_LIGHTING);
			glColor4f(0.1,0.3,1.0,0.3);
				
			glBegin(GL_QUADS);
			glVertex3f(0,									maxHeight+objPlacementAlt,	0);
			glVertex3f(0,									maxHeight+objPlacementAlt,	level->ground()->resolutionZ()*size);
			glVertex3f(level->ground()->resolutionX()*size,	maxHeight+objPlacementAlt,	level->ground()->resolutionZ()*size);
			glVertex3f(level->ground()->resolutionX()*size,	maxHeight+objPlacementAlt,	0);
			glEnd();

			glColor4f(0.3,0.5,1.0,0.2);
			glBegin(GL_LINES);
			for(int i=0;i<level->ground()->resolutionZ()*size; i+=size*4)
			{
				glVertex3f(i,maxHeight+10,0);
				glVertex3f(i,maxHeight+10,level->ground()->resolutionZ()*size);
			}
			for(int i=0;i<level->ground()->resolutionX()*size; i+=size*4)
			{
				glVertex3f(0,maxHeight+10,i);
				glVertex3f(level->ground()->resolutionX()*size,maxHeight+10,i);
			}
			glEnd();
			glColor3f(1,1,1);
			glDepthMask(true);
			////////////////////////////////end grid///////////////////////////////////
		}		
		glDisable(GL_DEPTH_TEST);
	
		glBindTexture(GL_TEXTURE_2D,0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 1, (float)sh/sw , 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(0,1,0);

		for(auto i = level->objects().begin(); i!= level->objects().end(); i++)
		{
			Vec2f s = frustum.project(i->startloc);
			float r;

			auto p = dataManager.getModel(i->type);
			if(p==NULL)
			{
				r = 0.006;
			}
			else
			{
				Vec2f t = frustum.project(i->startloc + p->getCenter() + u*p->getRadius()*10);
				r = max(0.004,s.distance(t));
			}
			if(frustum.sphereInFrustum(i->startloc,r)!=FrustumG::OUTSIDE && s.x > -r && s.x < 1.0+r && s.y > -r && s.y < 1.0+r)
			{
				graphics->drawOverlay(s.x - r, s.y*sh/sw - r, r*2, r*2,"target ring");
			}	
		}
	
		glMatrixMode( GL_PROJECTION );			// Select Projection
		glPopMatrix();							// Pop The Matrix
		glMatrixMode( GL_MODELVIEW );			// Select Modelview
		glEnable(GL_DEPTH_TEST);
	}
}
void modeMapBuilder::draw2D()
{

}
bool modeMapBuilder::init()
{
	menus = new menu::levelEditor;
	menus->mode = this;
	menuManager.setMenu(menus);

	if(level != NULL) delete level;
	level = new editLevel;

	scroll=0.0;
	level->newGround(513,513);
	diamondSquare(0.17,0.5);

	resetView();
	return true;
}