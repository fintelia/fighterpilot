
#include "engine.h"
#include "GL/glee.h"
#include <GL/glu.h>
#include "png/png.h"


struct OpenGLgraphics::Context
{
	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance; // not initialized?
	Context(): hDC(NULL),hRC(NULL),hWnd(NULL){}
};

void OpenGLgraphics::flashTaskBar(int times, int length)
{
#ifdef VISUAL_STUDIO
	FLASHWINFO f;
	f.cbSize = sizeof(FLASHWINFO);
	f.hwnd = context->hWnd;
	f.dwFlags = FLASHW_TRAY;
	f.dwTimeout = length;
	f.uCount = times;
	FlashWindowEx(&f);
#endif
}
void OpenGLgraphics::minimizeWindow()
{
	ShowWindow(context->hWnd, SW_MINIMIZE);
}
OpenGLgraphics::OpenGLgraphics():multisampling(false),samples(0),renderTarget(SCREEN)
{
	FBOs[0]					= FBOs[1]				= 0;
	renderTextures[0]		= renderTextures[1]		= 0;
	depthTextures[0]		= depthTextures[1]		= 0;
	colorRenderBuffers	= 0;
	depthRenderBuffers	= 0;

	context = new Context;

	//useAnagricStereo(true);
	//setInterOcularDistance(0.75);
}
OpenGLgraphics::~OpenGLgraphics()
{
	delete context;
}
bool OpenGLgraphics::drawOverlay(Rect4f r, string tex)
{
	if(tex != "") dataManager.bind(tex);

	overlay[0].position = Vec2f(r.x,		r.y+r.h);
	overlay[1].position = Vec2f(r.x+r.w,	r.y+r.h);
	overlay[2].position = Vec2f(r.x+r.w,	r.y);
	overlay[3].position = Vec2f(r.x,		r.y);

	overlay[0].texCoord = Vec2f(0.0,1.0);
	overlay[1].texCoord = Vec2f(1.0,1.0);
	overlay[2].texCoord = Vec2f(1.0,0.0);
	overlay[3].texCoord = Vec2f(0.0,0.0);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(tex != "") dataManager.unbind(tex);
	return true;
}
bool OpenGLgraphics::drawRotatedOverlay(Rect4f r, Angle rotation, string tex)
{
	if(tex != "") dataManager.bind(tex);

	float w2 = r.w/2;
	float h2 = r.h/2;
	overlay[0].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*0.25) , r.y+h2 + h2*sin(-rotation+PI*0.25));
	overlay[1].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*0.75) , r.y+h2 + h2*sin(-rotation+PI*0.75));
	overlay[2].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*1.25) , r.y+h2 + h2*sin(-rotation+PI*1.25));
	overlay[3].position = Vec2f(r.x+w2 + w2*cos(-rotation+PI*1.75) , r.y+h2 + h2*sin(-rotation+PI*1.75));

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

	if(tex != "") dataManager.unbind(tex);
	return true;
}
bool OpenGLgraphics::drawPartialOverlay(Rect4f r, Rect4f t, string tex)
{
	if(tex != "") dataManager.bind(tex);

	overlay[0].position = Vec2f(r.x,		r.y);
	overlay[1].position = Vec2f(r.x+r.w,	r.y);
	overlay[2].position = Vec2f(r.x+r.w,	r.y+r.h);
	overlay[3].position = Vec2f(r.x,		r.y+r.h);

	overlay[0].texCoord = Vec2f(t.x,		t.y);
	overlay[1].texCoord = Vec2f(t.x+t.w,	t.y);
	overlay[2].texCoord = Vec2f(t.x+t.w,	t.y+t.h);
	overlay[3].texCoord = Vec2f(t.x,		t.y+t.h);


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].position.x);
	glTexCoordPointer(2, GL_FLOAT, sizeof(texturedVertex2D), &overlay[0].texCoord.x);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(tex != "") dataManager.unbind(tex);
	return true;
}
void OpenGLgraphics::drawText(string text, Vec2f pos, string font)
{
	auto f = dataManager.getFont(font);

	if(f != NULL)
	{
		dataManager.bind(f->texName);

		Vec2f charPos = pos;
		Rect charRect;
		//if(dataManager.getBoundShader() == "ortho")
		//{
			for(auto i = text.begin(); i != text.end(); i++)
			{
				if(*i == '\n')
				{
					charPos.x = pos.x;
					charPos.y += f->height / sh;
				}
				else if(f->characters.count(*i) != 0)
				{
					auto& c = f->characters.find(*i)->second;

					charRect.x = charPos.x + c.xOffset / sh;
					charRect.y = charPos.y + c.yOffset / sh;
					charRect.w = c.width / sh;
					charRect.h = c.height / sh;

					drawPartialOverlay(charRect, c.UV);
					charPos.x += c.xAdvance  / sh;
				}
			}
		//}
		//else
		//{
		//	for(auto i = text.begin(); i != text.end(); i++)
		//	{
		//		if(*i == '\n')
		//		{
		//			charPos.x = pos.x;
		//			charPos.y += f->height;
		//		}
		//		else if(f->characters.count(*i) != 0)
		//		{
		//			auto& c = f->characters.find(*i)->second;

		//			charRect.x = charPos.x + c.xOffset;
		//			charRect.y = charPos.y + c.yOffset;
		//			charRect.w = c.width;
		//			charRect.h = c.height;

		//			drawPartialOverlay(charRect, c.UV);
		//			charPos.x += c.xAdvance;
		//		}
		//	}
		//}
		dataManager.unbind(f->texName);
	}
}
void OpenGLgraphics::drawText(string text, Rect rect, string font)
{
	auto f = dataManager.getFont(font);

	if(f == NULL)
	{
		dataManager.bind(f->texName);

		Vec2f charPos = rect.origin();
		Rect charRect;
		for(auto i = text.begin(); i != text.end(); i++)
		{
			if(*i == '\n')
			{
				charPos.x = rect.x;
				charPos.y += f->height;
			}
			else if(f->characters.count(*i) != 0 && rect.inRect(charPos))
			{
				auto& c = f->characters.find(*i)->second;

				charRect.x = charPos.x + c.xOffset;
				charRect.y = charPos.y + c.yOffset;
				charRect.w = c.width;
				charRect.h = c.height;

				Rect UV = c.UV;
				if(charRect.x < rect.x + rect.w && charRect.y < rect.y + rect.h)
				{
					if(charRect.x + charRect.w > rect.x + rect.w)	UV.w *= ((rect.x + rect.w) - charRect.x) / charRect.w;
					if(charRect.y + charRect.h > rect.y + rect.h)	UV.h *= ((rect.y + rect.h) - charRect.y) / charRect.h;

					drawPartialOverlay(charRect, UV);
				}
				charPos.x += c.xAdvance;
			}
		}

		dataManager.unbind(f->texName);
	}
}
Vec2f OpenGLgraphics::textSize(string text, string font)
{
	auto f = dataManager.getFont(font);
	if(f == nullptr)
		return Vec2f(0,0);

	Vec2f textSize(0,f->height);
	for(auto i = text.begin(); i != text.end(); i++)
	{
		if(*i == '\n')
		{
			textSize.y += f->height;
		}
		else if(f->characters.count(*i) != 0)
		{
			textSize.x += f->characters.find(*i)->second.xAdvance;
		}
	}

	//if(dataManager.getBoundShader() == "ortho")
		return textSize / sh;
	//else
	//	return textSize;
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

	dataManager.bindTex(renderTextures[src]);
	drawOverlay(Rect::XYXY(-1,-1,1,1));
	dataManager.bindTex(0);
}
bool OpenGLgraphics::init()
{
	if(!GLEE_VERSION_2_0)
	{
		string s = string("Your version of OpenGL must be at least 2.0\n   OpenGL version: ") + (char*)glGetString(GL_VERSION) + "\n   Renderer: " + (char*)glGetString(GL_RENDERER) + "\n   Vender: " + (char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}
	else if(!GLEE_EXT_framebuffer_blit)
	{
		string s = string("Your graphics card must support GL_EXT_framebuffer_blit\n   OpenGL version: ") + (char*)glGetString(GL_VERSION) + "\n   Renderer: " + (char*)glGetString(GL_RENDERER) + "\n   Vender: " + (char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE4_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0_ARB);	glEnable(GL_TEXTURE_2D);

	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//FRAME BUFFER OBJECTS

	glGenFramebuffersEXT(2, FBOs);

	glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
	samples = samples >= 8 ? 8 : samples;
	multisampling = GLEE_EXT_framebuffer_multisample && samples > 1;
	for(int i=0;i<2;i++)
	{
		if(multisampling && i == 0 )
		{
			glGenRenderbuffersEXT(1, &colorRenderBuffers);
			glGenRenderbuffersEXT(1, &depthRenderBuffers);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, colorRenderBuffers);
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_RGBA, sw, sh);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRenderBuffers);
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_DEPTH_COMPONENT , sw, sh);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i]);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorRenderBuffers);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthRenderBuffers);
		}
		else
		{
			glGenTextures(1, renderTextures+i);
			glBindTexture(GL_TEXTURE_2D, renderTextures[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sw, sh, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

			glGenTextures(1, depthTextures+i);
			glBindTexture(GL_TEXTURE_2D, depthTextures[i]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, sw, sh, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOs[i]);

			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderTextures[i], 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTextures[i], 0);
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
		sAspect = ((float)sw)/sh;
	}
}
void OpenGLgraphics::render()
{
/////////////////////////////////////START TIMING/////////////////////////////////////
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
	glEnable(GL_DEPTH_TEST);

	//dataManager.unbindShader();


	//glUseProgram(0);
/////////////////////////////////////START 3D////////////////////////////////////
	if(stereo) // only the first view is drawn with stereo rendering
	{
		glViewport(0, 0, sw, sh);

		leftEye = true;
		glColorMask(true,false,false,true);
		menuManager.render3D(0);

		glClear(GL_DEPTH_BUFFER_BIT);

		leftEye = false;
		glColorMask(false,true,true,true);
		menuManager.render3D(0);

		glColorMask(true,true,true,true);
	}
	else
	{
		for(currentView=0; currentView<views.size(); currentView++)
		{
			glViewport(views[currentView].viewport.x * sh, views[currentView].viewport.y * sh, views[currentView].viewport.width * sh, views[currentView].viewport.height * sh);
			glMatrixMode(GL_PROJECTION);	glLoadMatrixf(views[currentView].projectionMat.ptr());
			glMatrixMode(GL_MODELVIEW);		glLoadIdentity();

			menuManager.render3D(currentView);
			sceneManager.renderScene();
		}
	}
///////////////////////////////////START PARTICLES///////////////////////

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, FBOs[0] );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, FBOs[1] );
	glBlitFramebufferEXT( 0, 0, sw, sh, 0, 0, sw, sh, GL_DEPTH_BUFFER_BIT, GL_NEAREST );
	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, 0 );
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, 0 );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, FBOs[0]);

	glDisable(GL_DEPTH_TEST);
	//glDepthMask(false);
	dataManager.bindTex(depthTextures[1],1);

	for(currentView=0; currentView<views.size(); currentView++)
	{
		glViewport(views[currentView].viewport.x * sh, views[currentView].viewport.y * sh, views[currentView].viewport.width * sh, views[currentView].viewport.height * sh);
		glMatrixMode(GL_PROJECTION);	glLoadMatrixf(views[currentView].projectionMat.ptr());
		glMatrixMode(GL_MODELVIEW);		glLoadMatrixf(views[currentView].modelViewMat.ptr());

		particleManager.render();
	}

	dataManager.unbindTextures();
	//glDepthMask(true);

/////////////////////////////////////START 2D////////////////////////////////////
	glViewport(0,0,sw,sh);
	glDisable(GL_DEPTH_TEST);


	dataManager.bind("ortho");
	dataManager.setUniform4f("color",white);
	menuManager.render();

	#ifdef _DEBUG
		if(fps<59.0)dataManager.setUniform4f("color",red);
		else dataManager.setUniform4f("color",black);
 		graphics->drawText(lexical_cast<string>(floor(fps)),Vec2f(sAspect*0.5-0.5*graphics->textSize(lexical_cast<string>(floor(fps))).x,0.02));
		dataManager.setUniform4f("color",white);
		Profiler.draw();
	#endif

	dataManager.unbindShader();


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
//void OpenGLgraphics::viewport(int x,int y,int width,int height)
//{
//	GraphicsManager::viewport(x,y,width,height);
//	glViewport(x,y,width,height);
//}
//void OpenGLgraphics::perspective(float fovy, float aspect, float zNear, float zFar)
//{
//	GraphicsManager::perspective(fovy, aspect, zNear, zFar);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(fovy, aspect, zNear, zFar);
//}
//void OpenGLgraphics::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
//{
//	GraphicsManager::ortho(left, right, bottom, top, zNear, zFar);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(left, right, bottom, top, zNear, zFar);
//}
void OpenGLgraphics::lookAt(Vec3f eye, Vec3f center, Vec3f up)
{
	if(stereo)
	{
		Vec3f f = (center - eye).normalize();
		Vec3f r = (up.normalize()).cross(f);
		if(leftEye)
		{
			eye += r * interOcularDistance * 0.5;
			center += r * interOcularDistance * 0.5;
		}
		else
		{
			eye -= r * interOcularDistance * 0.5;
			center -= r * interOcularDistance * 0.5;
		}
	}
	GraphicsManager::lookAt(eye, center, up);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x,eye.y,eye.z,center.x,center.y,center.z,up.x,up.y,up.z);
}
void OpenGLgraphics::destroyWindow()
{
	if(renderTextures[0] != 0)		glDeleteTextures(1, renderTextures);
	if(renderTextures[1] != 0)		glDeleteTextures(1, renderTextures+1);

	if(depthTextures[0] != 0)		glDeleteTextures(1, depthTextures);
	if(depthTextures[1] != 0)		glDeleteTextures(1, depthTextures+1);

	if(colorRenderBuffers != 0)		glDeleteRenderbuffers(1, &colorRenderBuffers);
	if(depthRenderBuffers != 0)		glDeleteRenderbuffers(1, &depthRenderBuffers);

	if(FBOs[0] != 0)				glDeleteFramebuffersEXT(1, FBOs);
	if(FBOs[1] != 0)				glDeleteFramebuffersEXT(1, FBOs+1);


	//glDeleteRenderbuffersEXT(2, depthRenderBuffers);

	ChangeDisplaySettings(NULL,0);					// Switch Back To The Desktop
	ShowCursor(true);								// Show Mouse Pointer

	if (context->hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			//MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(context->hRC))						// Are We Able To Delete The RC?
		{
			//MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		context->hRC=NULL;										// Set RC To NULL
	}

	if (context->hDC && !ReleaseDC(context->hWnd,context->hDC))					// Are We Able To Release The DC
	{
		//MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		context->hDC=NULL;										// Set DC To NULL
	}

	if (context->hWnd && !DestroyWindow(context->hWnd))					// Are We Able To Destroy The Window?
	{
		//MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		context->hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(L"OpenGL",context->hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		context->hInstance=NULL;									// Set hInstance To NULL
	}
}
void OpenGLgraphics::setGamma(float gamma)
{
	/*if(hDC == NULL)
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
		assert(false);*/
}
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool OpenGLgraphics::createWindow(string title, Vec2i screenResolution)
{
	static bool arbMultisampleSupported=false;
	static int arbMultisampleFormat=0;
	bool fullscreenflag=true;

	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASSEX	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style

	context->hInstance	= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									//- No Extra Window Data
	wc.hInstance		= context->hInstance;					// Set The Instance
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

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = screenResolution.x;
	WindowRect.bottom = screenResolution.y;

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
	if (!(context->hWnd=CreateWindowExA(	dwExStyle,				// Extended Style For The Window
								"OpenGL",							// Class Name
								title.c_str(),						// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								context->hInstance,					// Instance
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
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | 0x00008000/*PFD_SUPPORT_COMPOSITION*/ | PFD_STEREO;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (!(context->hDC=GetDC(context->hWnd)))			// Did We Get A Device Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}
	if(!arbMultisampleSupported)
	{
		if (!(PixelFormat=ChoosePixelFormat(context->hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
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
	if(!SetPixelFormat(context->hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}


	if (!(context->hRC=wglCreateContext(context->hDC)))				// Are We Able To Get A Rendering Context?
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}


	if(!wglMakeCurrent(context->hDC,context->hRC))					// Try To Activate The Rendering Context
	{
		destroyWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

//	if(!arbMultisampleSupported && checkMultisample && wglChoosePixelFormatARB)
//	{
//		int pixelFormat;
//		UINT numFormats;
//		float fAttributes[] = {0,0};
//		int iAttributes[] = {
//			WGL_SAMPLES_ARB,			16,					// Check For 16x Multisampling
//			WGL_DRAW_TO_WINDOW_ARB,		GL_TRUE,
//			WGL_SUPPORT_OPENGL_ARB,		GL_TRUE,
//			WGL_ACCELERATION_ARB,		WGL_FULL_ACCELERATION_ARB,
//			//WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
//			WGL_COLOR_BITS_ARB,			24,
//			WGL_ALPHA_BITS_ARB,			8,
//			WGL_DEPTH_BITS_ARB,			24,
////			WGL_STENCIL_BITS_ARB,		0,
//			WGL_DOUBLE_BUFFER_ARB,		GL_TRUE,
//			WGL_SAMPLE_BUFFERS_ARB,		GL_TRUE,
//			0,0};
//
//		while(iAttributes[1] > 1)
//		{
//			if (wglChoosePixelFormatARB(context->hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats) && numFormats >= 1)
//			{
//				arbMultisampleSupported = true;
//				arbMultisampleFormat	= pixelFormat;
//				destroyWindow();
//				return graphics->createWindow(title, screenResolution, false);
//			}
//			iAttributes[1] >>= 1;
//		}
//	}

	ShowWindow(context->hWnd,SW_SHOW);				// Show The Window
	SetForegroundWindow(context->hWnd);				// Slightly Higher Priority
	SetFocus(context->hWnd);						// Sets Keyboard Focus To The Window
	graphics->resize(WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top);	// Set Up Our Perspective GL Screen
	//setGamma(0.0);

	if (!graphics->init())							// Initialize Our Newly Created GL Window
	{
		destroyWindow();					// Reset The Display
		MessageBox(NULL,L"Initialization Failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return false
	}

	RegisterHotKey(context->hWnd,IDHOT_SNAPWINDOW,0,VK_SNAPSHOT);
	RegisterHotKey(context->hWnd,IDHOT_SNAPDESKTOP,0,VK_SNAPSHOT);

	return true;									// Success
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
		SetWindowPos(context->hWnd,0,0,0,resolution.x,resolution.y,0);
		resize(resolution.x,resolution.y);
		return true;
	}
	return false;
}
void OpenGLgraphics::swapBuffers()
{
	SwapBuffers(context->hDC);
}
void OpenGLgraphics::takeScreenshot()
{
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);

	string filename = "screen shots\\FighterPilot ";
	filename += lexical_cast<string>(sTime.wYear) + "-" + lexical_cast<string>(sTime.wMonth) + "-" + lexical_cast<string>(sTime.wDay) + " " +
				lexical_cast<string>(sTime.wHour+1) + "-" + lexical_cast<string>(sTime.wMinute) + "-" + lexical_cast<string>(sTime.wSecond) + "-" + lexical_cast<string>(sTime.wMilliseconds) + ".png";

	//if(!filesystem::is_directory("screen shots"))
	//	filesystem::create_directory("screen shots");

	fileManager.createDirectory("screen shots");//(windows only) if it already exists then nothing happens


	int size = (3*sw+sw%4)*sh + 3*sw*sh%4;

	//see: http://zarb.org/~gc/html/libpng.html
	FILE *fp;
	if((fp=fopen(filename.c_str(), "wb")) != nullptr && fp)
	{
		unsigned char* colors = new unsigned char[size];
		memset(colors,0,size);
		glReadPixels(0, 0, sw, sh, GL_RGB, GL_UNSIGNED_BYTE, colors);
		png_bytepp rows = new unsigned char*[sh];
		for(int i=0;i<sh;i++) rows[i] = colors +  (3*sw+sw%4) * (sh-i);

		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,	NULL, NULL);
		if (!png_ptr)
		{
			fclose(fp);
			delete[] colors;
			return;
		}
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr || setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
			fclose(fp);
			delete[] colors;
			return;
		}
		png_init_io(png_ptr, fp);
		png_set_IHDR(png_ptr, info_ptr, sw, sh, 8, PNG_COLOR_TYPE_RGB , PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		png_set_rows(png_ptr,info_ptr,rows);

		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

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
void OpenGLgraphics::drawLine(Vec3f start, Vec3f end)
{
	shapes3D[0].position = start;
	shapes3D[1].position = end;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vertex3D), &shapes3D[0].position.x);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
}
void OpenGLgraphics::drawTriangle(Vec3f p1, Vec3f p2, Vec3f p3)
{
 	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vertex3D), &shapes3D[0].position.x);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}
void OpenGLgraphics::drawQuad(Vec3f p1, Vec3f p2, Vec3f p3, Vec3f p4)
{
	shapes3D[0].position = p1;
	shapes3D[1].position = p2;
	shapes3D[2].position = p3;
	shapes3D[3].position = p4;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vertex3D), &shapes3D[0].position.x);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
}
void OpenGLgraphics::drawModel(string model, Vec3f position, Quat4f rotation, Vec3f scale)
{
	auto m = dataManager.getModel(model);

	float s;
	if(abs(scale.x) >= abs(scale.y) && abs(scale.x) >= abs(scale.z))
		s = scale.x;
	else if(abs(scale.y) >= abs(scale.z))
		s = scale.y;
	else
		s = scale.z;

	if(m == nullptr || !sphereInFrustum(m->boundingSphere * s + position))
		return;

	dataManager.bind("white");
	dataManager.bind("model");
	dataManager.setUniform1i("tex",0);


	//Mat4f matV = views[currentView].modelViewMat  * Mat4f(rotation,position,scale);
	//Mat4f matP = views[currentView].projectionMat;
	//Mat4f mat =  matP * matV;
	//Mat4f matV2;	glGetFloatv(GL_MODELVIEW_MATRIX,matV2.ptr());
	//Mat4f matP2;	glGetFloatv(GL_PROJECTION_MATRIX,matP2.ptr());
	//dataManager.setUniformMatrix("modelviewProjection",mat);
	Mat4f cameraProjectionMat = getView().projectionMat * getView().modelViewMat;
	dataManager.setUniformMatrix("cameraProjection",cameraProjectionMat);

	dataManager.setUniformMatrix("modelTransform", Mat4f(rotation, position, scale));

	glBindBuffer(GL_ARRAY_BUFFER, m->id);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)0);
	glNormalPointer(	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(3*sizeof(float)));
	glTexCoordPointer(2,GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(6*sizeof(float)));

	bool dMask = true;

	for(auto i = m->materials.begin(); i!=m->materials.end(); i++)
	{
		dataManager.bind(i->tex == "" ? "white" : i->tex);
		glColor4f(i->color.r,i->color.g,i->color.b, i->color.a);

		if(i->color.a > 0.999 && !dMask)
		{
			dMask = true;
			glDepthMask(true);
		}
		else if(i->color.a <= 0.999 && dMask)
		{
			dMask = false;
			glDepthMask(false);
		}

		glDrawArrays(GL_TRIANGLES,i->indicesOffset, i->numIndices);
	}

	if(!dMask)	glDepthMask(true);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	dataManager.unbind("model");
	dataManager.unbindTextures();
	glColor3f(1,1,1);
}
void OpenGLgraphics::drawModelCustomShader(string model, Vec3f position, Quat4f rotation, Vec3f scale)
{
	auto m = dataManager.getModel(model);

	float s;
	if(abs(scale.x) >= abs(scale.y) && abs(scale.x) >= abs(scale.z))
		s = scale.x;
	else if(abs(scale.y) >= abs(scale.z))
		s = scale.y;
	else
		s = scale.z;

	if(m == nullptr || !sphereInFrustum(m->boundingSphere * s + position))
		return;

	Mat4f cameraProjectionMat = getView().projectionMat * getView().modelViewMat;
	dataManager.setUniformMatrix("cameraProjection",cameraProjectionMat);

	dataManager.setUniformMatrix("modelTransform", Mat4f(rotation, position, scale));

	glBindBuffer(GL_ARRAY_BUFFER, m->id);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)0);
	glNormalPointer(	GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(3*sizeof(float)));
	glTexCoordPointer(2,GL_FLOAT,	sizeof(texturedLitVertex3D), (void*)(6*sizeof(float)));

	bool dMask = true;

	for(auto i = m->materials.begin(); i!=m->materials.end(); i++)
	{
		dataManager.bind(i->tex == "" ? "white" : i->tex);
		glColor4f(i->color.r,i->color.g,i->color.b, i->color.a);

		if(i->color.a > 0.999 && !dMask)
		{
			dMask = true;
			glDepthMask(true);
		}
		else if(i->color.a <= 0.999 && dMask)
		{
			dMask = false;
			glDepthMask(false);
		}

		glDrawArrays(GL_TRIANGLES,i->indicesOffset, i->numIndices);
	}

	if(!dMask)	glDepthMask(true);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	dataManager.unbindTextures();
	glColor3f(1,1,1);
}
