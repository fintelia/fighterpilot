
#include "main.h"

bool GraphicsManager::drawObject(gID obj)
{
	if(objects.find(obj) !=objects.end())
	{
		objects[obj]->drawFlag = true;
		return true;
	}
	return false;
}
void GraphicsManager::render3D()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
		if(i->second->drawFlag && !i->second->transparent)	i->second->render();
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
		if(i->second->drawFlag && i->second->transparent)	i->second->render();
}
GraphicsManager::gID OpenGLgraphics::newModel(string disp)
{
	gID nID = idGen();
	objects.insert(pair<gID,object*>(nID, new model(disp)));
	return nID;
}
GraphicsManager::gID OpenGLgraphics::newParticleEffect(string texture, int size, string shader)
{
	gID nID = idGen();
	objects.insert(pair<gID,object*>(nID, new particleEffect(texture,shader,size)));
	return nID;
}
void GraphicsManager::reset()
{
	for(map<gID,object*>::iterator i=objects.begin();i!=objects.end();i++)
	{
		i->second->reset(); 
	}
}
void GraphicsManager::flashTaskBar(int times, int length)
{
	FLASHWINFO f;
	f.cbSize = sizeof(FLASHWINFO);
	f.hwnd = hWnd;
	f.dwFlags = FLASHW_TRAY;
	f.dwTimeout = length;
	f.uCount = times;
	FlashWindowEx(&f);
}
void GraphicsManager::minimizeWindow()
{
	ShowWindow(hWnd, SW_MINIMIZE);
}
bool OpenGLgraphics::drawModel(gID obj, Vec3f pos, Quat4f rot)
{
	if(objects.find(obj) !=objects.end() && objects[obj]->type==object::MODEL)
	{
		objects[obj]->drawFlag = true;
		((model*)objects[obj])->pos = pos;
		((model*)objects[obj])->rot = rot;
		return true;
	}
	return false;
}
bool OpenGLgraphics::drawModel(gID obj)
{
	if(objects.find(obj) !=objects.end() && objects[obj]->type==object::MODEL)
	{
		objects[obj]->drawFlag = true;
		return true;
	}
	return false;
}
OpenGLgraphics::particleEffect::particleEffect(string tex, string Shader, int s): object(PARTICLE_EFFECT,true), texture(tex), shader(Shader), size(s), num(0), compacity(32), particles(NULL)
{
	particles = new (nothrow) particle[compacity];
	if(particles == NULL)
		compacity = 0;
	glGenBuffers(1,&VBO);
}
void OpenGLgraphics::particleEffect::reset()
{
	drawFlag=false;
	num = 0;
}
bool OpenGLgraphics::particleEffect::setParticle(Vec3f p, float life)
{

	if(compacity <= num)
	{
		particle* tmpParticles = new (nothrow) particle[compacity*2];
		if(tmpParticles == NULL) 
			return false;

		memcmp(tmpParticles,particles,compacity*sizeof(particle));
		delete[] particles;
		particles = tmpParticles;

		compacity *= 2;
	}
	particles[num].vert = p;
	//particles[num].r = c.r;
	//particles[num].g = c.g;
	//particles[num].b = c.b;
	//particles[num].a = c.a;
	particles[num].life = life;

	drawFlag=true;
	num++;
	return true;
}
void OpenGLgraphics::particleEffect::render()
{
	if(num==0)return;

	dataManager.bind(texture);

	if(shader == "partical shader")
	{
		dataManager.bind(shader);
		glUniform1i(glGetUniformLocation(dataManager.getId(shader), "tex"), 0);
		glUniform1f(glGetUniformLocation(dataManager.getId(shader), "size1"), size * 1.00);
		glUniform1f(glGetUniformLocation(dataManager.getId(shader), "size2"), size * 0.05);//not currently used


		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particle)*num, particles, GL_DYNAMIC_DRAW);

		glEnableClientState(GL_VERTEX_ARRAY);
		GLuint aLife = glGetAttribLocation(dataManager.getId("partical shader"),"life");
		glEnableVertexAttribArray(aLife);

		glVertexPointer(3, GL_FLOAT, sizeof(particle), 0);
		glVertexAttribPointer(aLife,1,GL_FLOAT,false, sizeof(particle), (void*)(7*sizeof(float)));
		//glColorPointer(4, GL_FLOAT, sizeof(particle), (void*)sizeof(Vec3f));

		glDrawArrays(GL_POINTS, 0, num);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableVertexAttribArray(aLife);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	dataManager.bindShader(0);
	dataManager.bindTex(0);
}
OpenGLgraphics::particleEffect::~particleEffect()
{
	glDeleteBuffers(1,&VBO);
	delete[] particles;
}
bool OpenGLgraphics::drawParticle(gID id, Vec3f pos, float life)
{
	if(objects.find(id) !=objects.end() && objects[id]->type==object::PARTICLE_EFFECT)
	{
		return ((particleEffect*)objects[id])->setParticle(pos, life);
	}
	return false;
}
bool OpenGLgraphics::drawOverlay(Vec2f origin, Vec2f size)
{
	overlay[0].position = origin + Vec2f(0.0,		0.0);
	overlay[1].position = origin + Vec2f(size.x,	0.0);
	overlay[2].position = origin + Vec2f(size.x,	size.y);
	overlay[3].position = origin + Vec2f(0.0,		size.y);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);

	return true;
}
bool OpenGLgraphics::drawOverlay(Vec2f origin, Vec2f size, string tex)
{
	overlay[0].position = origin + Vec2f(0.0,		0.0);
	overlay[1].position = origin + Vec2f(size.x,	0.0);
	overlay[2].position = origin + Vec2f(size.x,	size.y);
	overlay[3].position = origin + Vec2f(0.0,		size.y);

	overlay[0].texCoord = Vec2f(0.0,0.0);
	overlay[1].texCoord = Vec2f(1.0,0.0);
	overlay[2].texCoord = Vec2f(1.0,1.0);
	overlay[3].texCoord = Vec2f(0.0,1.0);

	dataManager.bind(tex);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	dataManager.unbind(tex);
	return true;
}
bool OpenGLgraphics::drawOverlay(float x,float y,float width,float height)
{
	overlay[0].position = Vec2f(x,y);
	overlay[1].position = Vec2f(x+width,y);
	overlay[2].position = Vec2f(x+width,y+height);
	overlay[3].position = Vec2f(x,y+height);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	return true;
}
bool OpenGLgraphics::drawOverlay(float x,float y,float width,float height,string tex)
{
	overlay[0].position = Vec2f(x,y);
	overlay[1].position = Vec2f(x+width,y);
	overlay[2].position = Vec2f(x+width,y+height);
	overlay[3].position = Vec2f(x,y+height);

	overlay[0].texCoord = Vec2f(0.0,0.0);
	overlay[1].texCoord = Vec2f(1.0,0.0);
	overlay[2].texCoord = Vec2f(1.0,1.0);
	overlay[3].texCoord = Vec2f(0.0,1.0);

	dataManager.bind(tex);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	dataManager.unbind(tex);
	return true;
}
bool OpenGLgraphics::drawRotatedOverlay(Vec2f origin, Vec2f size, Angle rotation, string tex)
{
	float w2 = size.x/2;
	float h2 = size.y/2;
	float cosRot = cos(rotation);
	float sinRot = sin(rotation);
	overlay[0].position = Vec2f(origin.x+w2 + w2*cos(-rotation+PI*0.25) , origin.y+h2 + h2*sin(-rotation+PI*0.25));
	overlay[1].position = Vec2f(origin.x+w2 + w2*cos(-rotation+PI*0.75) , origin.y+h2 + h2*sin(-rotation+PI*0.75));
	overlay[2].position = Vec2f(origin.x+w2 + w2*cos(-rotation+PI*1.25) , origin.y+h2 + h2*sin(-rotation+PI*1.25));
	overlay[3].position = Vec2f(origin.x+w2 + w2*cos(-rotation+PI*1.75) , origin.y+h2 + h2*sin(-rotation+PI*1.75));

	overlay[0].texCoord = Vec2f(0.0,0.0);
	overlay[1].texCoord = Vec2f(1.0,0.0);
	overlay[2].texCoord = Vec2f(1.0,1.0);
	overlay[3].texCoord = Vec2f(0.0,1.0);

	dataManager.bind(tex);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	dataManager.unbind(tex);

	return true;
}
bool OpenGLgraphics::drawPartialOverlay(Vec2f origin, Vec2f size, Vec2f tOrigin, Vec2f tSize, string tex)
{
	overlay[0].position = origin + Vec2f(0.0,		0.0);
	overlay[1].position = origin + Vec2f(size.x,	0.0);
	overlay[2].position = origin + Vec2f(size.x,	size.y);
	overlay[3].position = origin + Vec2f(0.0,		size.y);

	overlay[0].texCoord = tOrigin + Vec2f(0.0,		0.0);
	overlay[1].texCoord = tOrigin + Vec2f(tSize.x,	0.0);
	overlay[2].texCoord = tOrigin + Vec2f(tSize.x,	tSize.y);
	overlay[3].texCoord = tOrigin + Vec2f(0.0,		tSize.y);

	dataManager.bind(tex);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	dataManager.unbind(tex);

	//dataManager.bind(tex);
	//glBegin(GL_QUADS);
	//	glTexCoord2f(tOrigin.x,tOrigin.y);					glVertex2f(origin.x,origin.y);
	//	glTexCoord2f(tOrigin.x+tSize.x,tOrigin.y);			glVertex2f(origin.x+size.x,origin.y);
	//	glTexCoord2f(tOrigin.x+tSize.x,tOrigin.y+tSize.y);	glVertex2f(origin.x+size.x,origin.y+size.y);
	//	glTexCoord2f(tOrigin.x,tOrigin.y+tSize.y);			glVertex2f(origin.x,origin.y+size.y);
	//glEnd();
	//dataManager.unbind(tex);
	return true;
}
void OpenGLgraphics::bindRenderTarget(RenderTarget t)
{
	if(t == SCREEN)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	else if(t == FBO_0 || t == FBO_1)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[t]);
	}
}
void OpenGLgraphics::renderFBO(RenderTarget src)
{
	if(src != FBO_0 && src != FBO_1)
		return;

	glViewport(0,0,sw,sh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0, sw , sh , 0, -1, 1 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//double time = world.time()/10000;
	//double r = sqrt((float)sw*sw + sh*sh)/2;

	dataManager.bindTex(renderTextures[src]);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);	glVertex2f(sw,0);//glVertex2f(sw/2 + r * cos(time+PI*0.0), sh/2 + r * sin(time+PI*0.0));
		glTexCoord2f(1,0);	glVertex2f(sw,sh);//glVertex2f(sw/2 + r * cos(time+PI*0.5), sh/2 + r * sin(time+PI*0.5));
		glTexCoord2f(0,0);	glVertex2f(0,sh);//glVertex2f(sw/2 + r * cos(time+PI*1.0), sh/2 + r * sin(time+PI*1.0));
		glTexCoord2f(0,1);	glVertex2f(0,0);//glVertex2f(sw/2 + r * cos(time+PI*1.5), sh/2 + r * sin(time+PI*1.5));
	glEnd();
	dataManager.bindTex(0);
}
bool OpenGLgraphics::init()
{
	if(!GLEE_VERSION_2_0)
	{
		string s("Your version of opengl must be above 2.0");
		s+="\n   Opengl version: ";
		s+=(char*)glGetString(GL_VERSION);
		s+="\n   Renderer: ";
		s+=(char*)glGetString(GL_RENDERER);
		s+="\n   Vender: ";
		s+=(char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}
	else if(!GLEE_EXT_framebuffer_object)
	{
		string s("Your graphics card must support GL_EXT_framebuffer_object");
		s+="\n   Opengl version: ";
		s+=(char*)glGetString(GL_VERSION);
		s+="\n   Renderer: ";
		s+=(char*)glGetString(GL_RENDERER);
		s+="\n   Vender: ";
		s+=(char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);	
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);


	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if(GLEE_ARB_framebuffer_sRGB)	glEnable(GL_FRAMEBUFFER_SRGB);
	glActiveTextureARB(GL_TEXTURE4_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE3_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE2_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);	glEnable(GL_TEXTURE_2D);

	float quadratic[] =  {0.0f, 0.0f, 0.000001f};
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);
	glPointParameterf(GL_POINT_SIZE_MIN_ARB, 1);
	glPointParameterf(GL_POINT_SIZE_MAX_ARB, 8192);
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);


	//FRAME BUFFER OBJECTS
	//glGenTextures(2, renderTextures);
	//glGenFramebuffersEXT(2, FBOs);
	//glGenRenderbuffersEXT(2, depthRenderBuffers);

	//for(int i=0;i<2;i++)
	//{
	//	glBindTexture(GL_TEXTURE_2D, renderTextures[i]);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	//
	//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i]);
	//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderTextures[i], 0);

	//	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);
	//	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, sw, sh);

	//	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);

	//	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//	switch(status)
	//	{
	//	case GL_FRAMEBUFFER_COMPLETE_EXT:
	//		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//		break;

	//	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	//		std::cout << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
	//		return false;

	//	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	//		std::cout << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
	//		return false;

	//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	//		std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
	//		return false;

	//	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	//		std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
	//		return false;

	//	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	//		std::cout << "[ERROR] Framebuffer incomplete: Draw buffer." << std::endl;
	//		return false;

	//	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
	//		std::cout << "[ERROR] Framebuffer incomplete: Read buffer." << std::endl;
	//		return false;

	//	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	//		std::cout << "[ERROR] Unsupported by FBO implementation." << std::endl;
	//		return false;

	//	default:
	//		std::cout << "[ERROR] Unknow error." << std::endl;
	//		return false;
	//	}
	//	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
	//		return false;
	//}

	return true;
}
void OpenGLgraphics::resize(int w, int h)
{
	sh = h;
	sw = w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}
void OpenGLgraphics::render()
{
/////////////////////////////////////START TIMING/////////////////////////////////////
	double t=GetTime();
	static double frameTimes[20]={20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};//in milliseconds
	static int curentFrame=0;
	double time=GetTime();
	static double lastTime=time;
	frameTimes[curentFrame++]=time-lastTime;
	lastTime=time;
	if(curentFrame>20)curentFrame=0;

	double spf=0.0;
	for(int i=0;i<20;i++)
		spf+=(frameTimes[i]*0.001)/20;
	fps=1.0/spf;

/////////////////////////////////////BIND BUFFER//////////////////////////////////////////
	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[0]);

///////////////////////////////////CLEAR BUFFERS/////////////////////////////////
	glClearColor(0.47f,0.57f,0.63f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, sw, sh);

	dataManager.bind("noShader");
	glUseProgram(0);
/////////////////////////////////////START 3D////////////////////////////////////
	modeManager.render3D();

/////////////////////////////////////START 2D////////////////////////////////////
	glViewport(0,0,sw,sh);
	glMatrixMode(GL_PROJECTION);			// Select Projection
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, sw , sh , 0, -1, 1 );		// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);				// Select Modelview Matrix
	glLoadIdentity();						// Reset The Matrix

	glDisable(GL_DEPTH_TEST);
	modeManager.render2D();
	menuManager.render();


	#ifdef _DEBUG
		if(fps<29.0)glColor3f(1,0,0);
		else glColor3f(0,0,0);
 		//textManager->renderText(lexical_cast<string>(floor(fps)),sw/2-25,25);
		glColor3f(1,1,1);
		Profiler.draw();
	#endif

	glEnable(GL_DEPTH_TEST);
	glMatrixMode( GL_PROJECTION );			// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );			// Select Modelview

///////////////////////////////////////Post Processing//////////////////////////////////
	//if(GLEE_ARB_framebuffer_sRGB)	glEnable(GL_FRAMEBUFFER_SRGB);

	//bindRenderTarget(SCREEN);
	//glClearColor(0.0,0.0,0.0,1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//renderFBO(FBO_0);

	//if(GLEE_ARB_framebuffer_sRGB)	glDisable(GL_FRAMEBUFFER_SRGB);

////////////////////////////////////START RESET///////////////////////////////////////
	reset();
	glError();

}
void OpenGLgraphics::viewport(int x,int y,int width,int height)
{
	view.viewport.x = x;
	view.viewport.y = y;
	view.viewport.width = width;
	view.viewport.height = height;
	glViewport(x,y,width,height);
}
void OpenGLgraphics::perspective(float fovy, float aspect, float zNear, float zFar)
{
	view.projection.type = View::Projection::PERSPECTIVE;
	view.projection.fovy = fovy;
	view.projection.aspect = aspect;
	view.projection.zNear = zNear;
	view.projection.zFar = zFar;
	gluPerspective(fovy, aspect, zNear, zFar);
}
void OpenGLgraphics::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	view.projection.type = View::Projection::ORTHOGRAPHIC;
	view.projection.left = left;
	view.projection.right = right;
	view.projection.bottom = bottom;
	view.projection.top = top;
	view.projection.zNear = zNear;
	view.projection.zFar = zFar;
	glOrtho(left, right, bottom, top, zNear, zFar);
}
void OpenGLgraphics::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	view.camera.eye = eye;
	view.camera.center = center;
	view.camera.up = up;
	gluLookAt(eye.x,eye.y,eye.z,center.x,center.y,center.z,up.x,up.y,up.z);
}
void OpenGLgraphics::destroyWindow()
{
	glDeleteTextures(2, renderTextures);
	//glDeleteFramebuffersEXT(2, FBOs);
	//glDeleteRenderbuffersEXT(2, depthRenderBuffers);
	
	ChangeDisplaySettings(NULL,0);					// Switch Back To The Desktop
	ShowCursor(true);								// Show Mouse Pointer

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			//MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			//MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		//MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		//MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(L"OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}
void OpenGLgraphics::setGamma(float gamma)
{
	if(hDC == NULL)
		return;

	WORD gammaRamp[3][256];
	for(int i=0;i<256;i++)
	{
		int value = i*((int)((gamma*0.5f+0.5f)*255)+128);
		value = min(value,65535);
		gammaRamp[0][i] = 
		gammaRamp[1][i] = 
		gammaRamp[2][i] = value;
	}
	
	if (SetDeviceGammaRamp(hDC, gammaRamp))
	{
		currentGamma = gamma; // Store gama setting
	}
	else
		assert(false);
}
bool OpenGLgraphics::createWindow(char* title, RECT WindowRect, bool checkMultisample)
{
	static bool arbMultisampleSupported=false;
	static int arbMultisampleFormat=0;
	bool fullscreenflag=true;

	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASSEX	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									//- No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= (HICON)LoadImage(NULL,L"media/icon.ico",IMAGE_ICON,256,256,LR_LOADFROMFILE);// Load The Icon
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= L"OpenGL";							// Set The Class Name

	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return false
	}
	
	DEVMODE dmScreenSettings;								// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= WindowRect.right;		// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= WindowRect.bottom;	// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
		// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
		if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
		{
			fullscreenflag=false;		// Windowed Mode Selected.
		}
		else
		{
			// Pop Up A Message Box Letting User Know The Program Is Closing.
			return false;									// Return false
		}
	}

	if (fullscreenflag)											// Are We Still In Fullscreen Mode?
	{
		dwExStyle=0;//WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(false);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowExA(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		destroyWindow();					// Reset The Display
		MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}



	static	PIXELFORMATDESCRIPTOR pfd2=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_SUPPORT_COMPOSITION |					// needed for vista
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0,0,0,0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}
	if(!arbMultisampleSupported)
	{
		if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			destroyWindow();								// Reset The Display
			MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
			return false;								// Return false
		}
	}
	else
	{
		PixelFormat = arbMultisampleFormat;
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}


	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}


	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	if(!arbMultisampleSupported && checkMultisample && !lowQuality && wglChoosePixelFormatARB)
	{
		int pixelFormat;
		UINT numFormats;
		float fAttributes[] = {0,0};
		int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,24,
			WGL_ALPHA_BITS_ARB,8,
			WGL_DEPTH_BITS_ARB,16,
			WGL_STENCIL_BITS_ARB,0,
			WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
			WGL_SAMPLES_ARB, 16 ,						// Check For 16x Multisampling
			0,0};

		while(iAttributes[19] > 1)
		{
			if (wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats) && numFormats >= 1)
			{
				arbMultisampleSupported = true;
				arbMultisampleFormat	= pixelFormat;	
				destroyWindow();
				return graphics->createWindow(title, WindowRect, false);
			}
			iAttributes[19] >>= 1;
		}
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	graphics->resize(WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top);	// Set Up Our Perspective GL Screen
	//setGamma(0.0);
	if (!graphics->init())							// Initialize Our Newly Created GL Window
	{
		destroyWindow();					// Reset The Display
		MessageBox(NULL,L"Initialization Failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	RegisterHotKey(hWnd,IDHOT_SNAPWINDOW,0,VK_SNAPSHOT);
	RegisterHotKey(hWnd,IDHOT_SNAPDESKTOP,0,VK_SNAPSHOT);

	return true;									// Success
}
void OpenGLgraphics::swapBuffers()
{
	SwapBuffers(hDC);
}
void OpenGLgraphics::takeScreenshot()
{
	int size = (3*sw+sw%4)*sh + 3*sw*sh%4;


	BITMAPFILEHEADER header;
	header.bfType					= 0x4D42;
	header.bfSize					= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + size;
	header.bfReserved1				= 0;
	header.bfReserved2				= 0;
	header.bfOffBits				= sizeof(BITMAPINFO)+sizeof(BITMAPFILEHEADER);

	BITMAPINFO info;
	info.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth			= sw;
	info.bmiHeader.biHeight			= sh;
	info.bmiHeader.biPlanes			= 1;
	info.bmiHeader.biBitCount		= 24;
	info.bmiHeader.biCompression	= BI_RGB;
	info.bmiHeader.biSizeImage		= 0;
	info.bmiHeader.biXPelsPerMeter	= 100;
	info.bmiHeader.biYPelsPerMeter	= 100;
	info.bmiHeader.biClrUsed		= 0;
	info.bmiHeader.biClrImportant	= 0;
	memset(&info.bmiColors,0,sizeof(info.bmiColors));

	unsigned char* colors = new unsigned char[size];
	memset(colors,0,size);
	glReadPixels(0, 0, sw, sh, GL_RGB, GL_UNSIGNED_BYTE, colors); 

	for(int x=0;x<sw;x++)
	{
		for(int y=0;y<sh;y++)
		{
			swap(   colors[(y*(3*sw+sw%4) + 3*x) + 0]   ,    colors[(y*(3*sw+sw%4) + 3*x) + 2]   );
			//colors[(y*(3*sw+sw%4) + 3*x) + 0] = 255 - colors[(y*(3*sw+sw%4) + 3*x) + 0];
			//colors[(y*(3*sw+sw%4) + 3*x) + 1] = 255 - colors[(y*(3*sw+sw%4) + 3*x) + 1];
			//colors[(y*(3*sw+sw%4) + 3*x) + 2] = 255 - colors[(y*(3*sw+sw%4) + 3*x) + 2];
		}
	}
	ofstream fout("screen shot.bmp",ios::out|ios::binary|ios::trunc);
	fout.write((char*)&header,sizeof(header));
	fout.write((char*)&info,sizeof(info));
	fout.write((char*)colors,size);
	delete[] colors;
	fout.close();


	//see: http://zarb.org/~gc/html/libpng.html
	FILE *fp;
	fopen_s(&fp,"screen shot.png", "w");
	if(fp)
	{
		colors = new unsigned char[size];
		memset(colors,0,size);
		glReadPixels(0, 0, sw, sh, GL_RGB, GL_UNSIGNED_BYTE, colors); 
		png_bytepp rows = new unsigned char*[sh];
		for(int i=0;i<sh;i++) rows[i] = colors + (3*sw+sw%4)*i;

		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,	NULL, NULL);
		if (!png_ptr)
		{
			fclose(fp);
			delete[] colors;
			return;
		}
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
			fclose(fp);
			delete[] colors;
			return;
		}
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_write_struct(&png_ptr, &info_ptr);
			fclose(fp);
			delete[] colors;
			return;
		}
		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, sw, sh, 8, PNG_COLOR_TYPE_RGB , PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		//png_set_write_status_fn(png_ptr, [&write_row_callback](png_ptr, png_uint_32 row, int pass){});
		png_set_rows(png_ptr,info_ptr,rows);

		//png_write_info(png_ptr, info_ptr);

		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
		//png_write_image(png_ptr,rows);
		//png_write_info(png_ptr, info_ptr);
		//png_write_end(png_ptr, NULL);

		if (setjmp(png_jmpbuf(png_ptr))) debugBreak();
		png_destroy_write_struct(&png_ptr, &info_ptr);

		fclose(fp);
		delete[] rows;
		delete[] colors;
	}
	
}