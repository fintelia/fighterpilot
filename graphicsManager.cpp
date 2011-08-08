
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
Vec2f GraphicsManager::project(Vec3f p)
{
	p -= view.camera.eye;

	Vec3f f = (view.camera.center - view.camera.eye).normalize();
	Vec3f UP = (view.camera.up).normalize();

	Vec3f s = (f.cross(UP)).normalize();
	Vec3f u = (s.cross(f)).normalize();
	
	float F = 1.0/tan((view.projection.fovy*PI/180.0) / 2.0);

	Vec3f v = Vec3f(s.dot(p)*F/view.projection.aspect,   -u.dot(p)*F,   f.dot(p)*(view.projection.zNear+view.projection.zFar)-2.0*view.projection.zNear*view.projection.zFar) / (f.dot(p));

	return Vec2f( (v.x + 1.0) / 2.0, (v.y + 1.0) / 2.0 );
}
Vec3f GraphicsManager::unProject(Vec3f p)// from x=0 to 1 && y=0 to 1 && z from 0 to 1
{
	float alpha = view.projection.fovy * 0.5 * (p.y-0.5)*2 * PI/180;
	float beta = view.projection.fovy * 0.5 * (p.x-0.5)*2  * PI/180;

	Vec3f up = view.camera.up;
	Vec3f center = (view.camera.center - view.camera.eye).normalize();
	Vec3f right = up.cross(center);

	float d = (view.projection.zNear + (view.projection.zFar-view.projection.zNear)*p.z);

	Profiler.setOutput("p.x",(p.x*2-1.0));
	Profiler.setOutput("p.y",(p.y*2-1.0));

	Vec3f r = view.camera.eye + (center + up * tan(alpha) + right * tan(beta) * view.projection.aspect) * d;

	Vec2f pj = project(r);

	Profiler.setOutput("pj.x",((1.0-pj.x)*2-1.0));
	Profiler.setOutput("pj.y",((1.0-pj.y)*2-1.0));

	Profiler.setOutput("pj/p.x",((1.0-pj.x)*2-1.0)/(p.x*2-1.0));
	Profiler.setOutput("pj/p.y",((1.0-pj.y)*2-1.0)/(p.y*2-1.0));

	return r;


	//p = p-eye;
	//Angle alpha = acosA( (p.normalize()).dot(view.camera.center.normalize()) );
	//Angle beta = acosA( (p.normalize()).dot(view.camera.up.normalize()) );



	//matrix4x4<float> PM = view.modelViewMat * view.projectionMat;
	//view.projection.
}
OpenGLgraphics::OpenGLgraphics():renderTarget(SCREEN)
{
	renderTextures[0]		= renderTextures[1]		= 0;
	colorRenderBuffers[0]	= colorRenderBuffers[1] = 0;
	depthRenderBuffers[0]	= depthRenderBuffers[1] = 0;
	depthRenderBuffers[0]	= depthRenderBuffers[1] = 0;
	FBOs[0]					= FBOs[1]				= 0;
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
void OpenGLgraphics::model::render()
{
	//todo write code
}
bool OpenGLgraphics::drawOverlay(Vec2f p1, Vec2f p2)
{
	//bool orthoShader = dataManager.getCurrentShaderId() == 0;
	//if(orthoShader)
	//{
	//	dataManager.bind("ortho");
	//	if(!dataManager.texturesBound())
	//		dataManager.bind("white");
	//}

	overlay[0].position = Vec2f(p1.x,p1.y);
	overlay[1].position = Vec2f(p2.x,p1.y);
	overlay[2].position = Vec2f(p2.x,p2.y);
	overlay[3].position = Vec2f(p1.x,p2.y);

	overlay[0].texCoord = Vec2f(0.0,0.0);
	overlay[1].texCoord = Vec2f(1.0,0.0);
	overlay[2].texCoord = Vec2f(1.0,1.0);
	overlay[3].texCoord = Vec2f(0.0,1.0);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//if(orthoShader)
	//	dataManager.unbindShader();
	return true;
}
bool OpenGLgraphics::drawOverlay(Vec2f p1, Vec2f p2, string tex)
{
	dataManager.bind(tex);
	drawOverlay(p1, p2);
	dataManager.unbind(tex);
	return true;
}
bool OpenGLgraphics::drawOverlay(float x,float y,float x2,float y2)
{
	//bool orthoShader = dataManager.getCurrentShaderId() == 0;
	//if(orthoShader)
	//{
	//	dataManager.bind("ortho");
	//	if(!dataManager.texturesBound())
	//		dataManager.bind("white");
	//}

	overlay[0].position = Vec2f(x,y);
	overlay[1].position = Vec2f(x2,y);
	overlay[2].position = Vec2f(x2,y2);
	overlay[3].position = Vec2f(x,y2);

	overlay[0].texCoord = Vec2f(0.0,0.0);
	overlay[1].texCoord = Vec2f(1.0,0.0);
	overlay[2].texCoord = Vec2f(1.0,1.0);
	overlay[3].texCoord = Vec2f(0.0,1.0);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//if(orthoShader)
	//	dataManager.unbindShader();

	return true;
}
bool OpenGLgraphics::drawOverlay(float x,float y,float x2,float y2,string tex)
{
	dataManager.bind(tex);
	drawOverlay(x, y, x2, y2);
	dataManager.unbind(tex);
	return true;
}
bool OpenGLgraphics::drawRotatedOverlay(Vec2f origin, Vec2f size, Angle rotation, string tex)
{
	//bool orthoShader = dataManager.getCurrentShaderId() == 0;
	//if(orthoShader)
	//	dataManager.bind("ortho");

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

	//if(orthoShader)
	//	dataManager.unbindShader();

	return true;
}
bool OpenGLgraphics::drawPartialOverlay(Vec2f origin, Vec2f size, Vec2f tOrigin, Vec2f tSize, string tex)
{
	//bool orthoShader = dataManager.getCurrentShaderId() == 0;
	//if(orthoShader)
	//	dataManager.bind("ortho");

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

	//if(orthoShader)
	//	dataManager.unbindShader();

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
		string s("Your version of OpenGL must be above 2.0");
		s+="\n   OpenGL version: ";
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
		s+="\n   OpenGL version: ";
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
	//if(GLEE_ARB_framebuffer_sRGB)	glEnable(GL_FRAMEBUFFER_SRGB);
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
	
	glGenFramebuffersEXT(2, FBOs);
	glGenRenderbuffersEXT(2, depthRenderBuffers);

	int samples;
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
	samples = 8;
	for(int i=0;i<2;i++)
	{
		if(GLEE_EXT_framebuffer_multisample && i == 0 )
		{
			glGenRenderbuffersEXT(1, colorRenderBuffers+i);
			//glBindTexture(GL_TEXTURE_2D, renderTextures[i]);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, colorRenderBuffers[i]);
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_RGBA, sw, sh);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_DEPTH_COMPONENT24, sw, sh);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i]);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorRenderBuffers[i]);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);



			////glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i]);
			////glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderTextures[i], 0);
			////glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);

		}
		else
		{
			glGenTextures(1, renderTextures+i);
			glBindTexture(GL_TEXTURE_2D, renderTextures[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i]);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderTextures[i], 0);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, sw, sh);

			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBuffers[i]);

			
		}
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		string errorMsg;
		if(status == GL_FRAMEBUFFER_COMPLETE_EXT)							glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)			errorMsg = "Framebuffer incomplete: Attachment is NOT complete.";
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)	errorMsg = "Framebuffer incomplete: No image is attached to FBO.";
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)			errorMsg = "Framebuffer incomplete: Attached images have different dimensions.";
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)			errorMsg = "Framebuffer incomplete: Color attached images have different internal formats.";
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)		errorMsg = "Framebuffer incomplete: Draw buffer.";
		else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)		errorMsg = "Framebuffer incomplete: Read buffer.";
		else if(status == GL_FRAMEBUFFER_UNSUPPORTED_EXT)					errorMsg = "Unsupported by FBO implementation.";
		else 																errorMsg = "Unknow frame buffer error.";

		if(errorMsg != "")
		{
			MessageBoxA(NULL, errorMsg.c_str(),"ERROR",MB_OK);
			return false;
		}
	}

	return true;
}
void OpenGLgraphics::resize(int w, int h)
{
	if(sw > 0 && sh > 0)
	{
		sh = h;
		sw = w;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	}
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
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[0]);

///////////////////////////////////CLEAR BUFFERS/////////////////////////////////
	glClearColor(0.47f,0.57f,0.63f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, sw, sh);

	//dataManager.unbindShader();


	//glUseProgram(0);
/////////////////////////////////////START 3D////////////////////////////////////
	modeManager.render3D();
	menuManager.render3D();

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
 		textManager->renderText(lexical_cast<string>(floor(fps)),sw/2-25,25);
		glColor3f(1,1,1);
		Profiler.draw();
	#endif

	glEnable(GL_DEPTH_TEST);
	glMatrixMode( GL_PROJECTION );			// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );			// Select Modelview

///////////////////////////////////////Post Processing//////////////////////////////////
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, FBOs[0] );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, FBOs[1] );
	glBlitFramebufferEXT( 0, 0, sw, sh, 0, 0, sw, sh, GL_COLOR_BUFFER_BIT, GL_NEAREST );
	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, 0 );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, 0 );


	bindRenderTarget(SCREEN);
	glClearColor(0.0,0.0,0.0,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	dataManager.bind("gamma shader");
	dataManager.setUniform1f("gamma",2.2);
	dataManager.setUniform1i("tex",0);
	renderFBO(FBO_1);
	dataManager.unbindShader();
////////////////////////////////////START RESET///////////////////////////////////////
	//reset();

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

	float f = 1.0 / tan(fovy*PI/180 / 2.0);
	view.projectionMat.set(	f/aspect,	0.0,		0.0,						0.0,
							0.0,		f,			0.0,						0.0,
							0.0,		0.0,		(zFar+zNear)/(zNear-zFar),	2*zFar*zNear/(zNear-zFar),
							0.0,		0.0,		-1.0,						0.0);

	glLoadIdentity();
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

	view.projectionMat.set(	2.0/(right-left),	0.0,				0.0,				(left+right)/(left-right),
							0.0,				2.0/(top-bottom),	0.0,				(bottom+top)/(bottom-top),
							0.0,				0.0,				2.0/(zNear-zFar),	(zNear+zFar)/(zNear-zFar),
							0.0,				0.0,				0.0,				1.0);

	glOrtho(left, right, bottom, top, zNear, zFar);
}
void OpenGLgraphics::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f f = (center-eye).normalize();
	Vec3f s = f.cross(up.normalize());
	Vec3f u = s.cross(f);

	view.modelViewMat.set(	s.x,	s.y,	s.z,	s.dot(-eye),
							u.x,	u.y,	u.z,	u.dot(-eye),
							-f.x,	-f.y,	-f.z,	(-f).dot(-eye),
							0.0,	0.0,	0.0,	1.0);

	gluLookAt(eye.x,eye.y,eye.z,center.x,center.y,center.z,up.x,up.y,up.z);

	view.camera.eye = eye;
	view.camera.center = center;
	view.camera.up = u;
}
void OpenGLgraphics::destroyWindow()
{
	if(renderTextures[0] != 0)		glDeleteTextures(2, renderTextures);
	if(renderTextures[1] != 0)		glDeleteTextures(1, renderTextures+1);
	if(colorRenderBuffers[0] != 0)	glDeleteTextures(2, colorRenderBuffers);
	if(colorRenderBuffers[1] != 0)	glDeleteTextures(1, colorRenderBuffers+1);
	if(depthRenderBuffers[0] != 0)	glDeleteTextures(2, depthRenderBuffers);
	if(depthRenderBuffers[1] != 0)	glDeleteTextures(1, depthRenderBuffers+1);
	if(FBOs[0] != 0)				glDeleteFramebuffersEXT(1, FBOs);
	if(FBOs[1] != 0)				glDeleteFramebuffersEXT(1, FBOs+1);


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



	//static	PIXELFORMATDESCRIPTOR pfd2=				// pfd Tells Windows How We Want Things To Be
	//{
	//	sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
	//	1,											// Version Number
	//	PFD_DRAW_TO_WINDOW |						// Format Must Support Window
	//	PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
	//	PFD_SUPPORT_COMPOSITION |					// needed for vista
	//	PFD_DOUBLEBUFFER,							// Must Support Double Buffering
	//	PFD_TYPE_RGBA,								// Request An RGBA Format
	//	32,											// Select Our Color Depth
	//	0, 0, 0, 0, 0, 0,							// Color Bits Ignored
	//	0,											// Alpha Buffer
	//	0,											// Shift Bit Ignored
	//	0,											// No Accumulation Buffer
	//	0,0,0,0,									// Accumulation Bits Ignored
	//	24,											// 24Bit Z-Buffer (Depth Buffer)
	//	0,											// No Stencil Buffer
	//	0,											// No Auxiliary Buffer
	//	PFD_MAIN_PLANE,								// Main Drawing Layer
	//	0,											// Reserved
	//	0, 0, 0										// Layer Masks Ignored
	//};
	
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_COMPOSITION;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
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

	if(!arbMultisampleSupported && checkMultisample && wglChoosePixelFormatARB)
	{
		int pixelFormat;
		UINT numFormats;
		float fAttributes[] = {0,0};
		int iAttributes[] = {
			WGL_SAMPLES_ARB,			16,					// Check For 16x Multisampling
			WGL_DRAW_TO_WINDOW_ARB,		GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,		GL_TRUE,
			WGL_ACCELERATION_ARB,		WGL_FULL_ACCELERATION_ARB,
			//WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,			24,
			WGL_ALPHA_BITS_ARB,			8,
			WGL_DEPTH_BITS_ARB,			24,
//			WGL_STENCIL_BITS_ARB,		0,
			WGL_DOUBLE_BUFFER_ARB,		GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,		GL_TRUE,
			0,0};

		while(iAttributes[1] > 1)
		{
			if (wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats) && numFormats >= 1)
			{
				arbMultisampleSupported = true;
				arbMultisampleFormat	= pixelFormat;	
				destroyWindow();
				return graphics->createWindow(title, WindowRect, false);
			}
			iAttributes[1] >>= 1;
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
bool OpenGLgraphics::recreateWindow(Vec2i resolution, int multisample)
{
	return false;
}
bool OpenGLgraphics::changeResolution(Vec2f resolution)
{
	DEVMODE dmScreenSettings;										// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize			= sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= resolution.x;					// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= resolution.y;					// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= 32;							// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
	{
		SetWindowPos(hWnd,0,0,0,resolution.x,resolution.y,0);
		resize(resolution.x,resolution.y);
		return true;
	}
	return false;
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

void OpenGLgraphics::drawSphere(Vec3f position, float radius)
{
	static GLUquadric* quadric = gluNewQuadric();

	glPushMatrix();
	glTranslatef(position.x,position.y,position.z);
	gluSphere(quadric,radius,5,5);
	glPopMatrix();
}