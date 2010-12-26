
#include "main.h"
explosion::explosion(Vec3f Pos): pos(Pos), coreSize(0.0), fireballSize(0.0)
{
	StartTime=world.time();
	smokeParticle s;
	for(int i=0;i<10;i++)
	{
		s.initialVelocity=Vec3f((float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500).normalize()*((float)(rand()%2000)/7+25.0);
		//s.position=Pos+Vec3f((float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500).normalize()*10;
		smokeParticles.push_back(s);
	}
	for(int i=0;i<5;i++)
	{
		s.initialVelocity=Vec3f((float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500).normalize()*((float)(rand()%2000)/7+25.0);
		fireParticles.push_back(s);
	}
}
void explosion::render(bool flash)
{
	render3(); return;
	float time=world.time()-StartTime;//in ms
	if(time < 0) return;

	Vec3f up, right;
	l3dBillboardGetUpRightVector(up,right);
	dataManager.bind("explosion core");
	float s=max((1.0-pow(2.718f,-(time-100)/20))*200.0,0.0);
	/////////////////////////////////////////////CORE//////////////////////////////////////////////////////////
	//glColor4f(1,1,1,clamp(1.5-time/1000,0.0,1.0));
	//glBegin(GL_QUADS);
	//	glTexCoord2f(0,1);	glVertex3f(pos.x-up.x*s-right.x*s	,pos.y-up.y*s-right.y*s	,pos.z-up.z*s-right.z*s	);
	//	glTexCoord2f(0,0);	glVertex3f(pos.x+up.x*s-right.x*s	,pos.y+up.y*s-right.y*s	,pos.z+up.z*s-right.z*s	);
	//	glTexCoord2f(1,0);	glVertex3f(pos.x+up.x*s+right.x*s	,pos.y+up.y*s+right.y*s	,pos.z+up.z*s+right.z*s	);
	//	glTexCoord2f(1,1);	glVertex3f(pos.x-up.x*s+right.x*s	,pos.y-up.y*s+right.y*s	,pos.z-up.z*s+right.z*s	);
	//glEnd(); 

	//////////////////////////////////////////FIRE BALL/////////////////////////////////////////////////////////
	//glColor4f(1,1,1,0.4);
	//dataManager.bind("explosion fireball");
	//s=max((1.0-pow(2.718f,-(time-100)/20))*200.0,0.0);
	//glColor4f(1,1,1,clamp(1.5-time/1000,0.0,0.5));
	//glBegin(GL_QUADS);
	//	glTexCoord2f(0,1);	glVertex3f(pos.x-up.x*s-right.x*s	,pos.y-up.y*s-right.y*s	,pos.z-up.z*s-right.z*s	);
	//	glTexCoord2f(0,0);	glVertex3f(pos.x+up.x*s-right.x*s	,pos.y+up.y*s-right.y*s	,pos.z+up.z*s-right.z*s	);
	//	glTexCoord2f(1,0);	glVertex3f(pos.x+up.x*s+right.x*s	,pos.y+up.y*s+right.y*s	,pos.z+up.z*s+right.z*s	);
	//	glTexCoord2f(1,1);	glVertex3f(pos.x-up.x*s+right.x*s	,pos.y-up.y*s+right.y*s	,pos.z-up.z*s+right.z*s	);
	//glEnd();

	////////////////////////////////////////////FIRE////////////////////////////////////////////////////////////
	Vec3f loc;
	float sl;
	glColor4f(1,1,1,max(0.6-time/750*0.6,0.0));
	dataManager.bind("explosion fireball");
	for(vector<smokeParticle>::iterator i=fireParticles.begin();i!=fireParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100))*0.3;
		sl = 200.0*(1.0-pow(2.718f,-(time+50)/100));
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex3f(loc.x-(right*sl).x+(up*sl).x,loc.y-(right*sl).y+(up*sl).y,loc.z-(right*sl).z+(up*sl).z);
			glTexCoord2f(0,0);	glVertex3f(loc.x-(right*sl).x-(up*sl).x,loc.y-(right*sl).y-(up*sl).y,loc.z-(right*sl).z-(up*sl).z);
			glTexCoord2f(1,0);	glVertex3f(loc.x+(right*sl).x-(up*sl).x,loc.y+(right*sl).y-(up*sl).y,loc.z+(right*sl).z-(up*sl).z);
			glTexCoord2f(1,1);	glVertex3f(loc.x+(right*sl).x+(up*sl).x,loc.y+(right*sl).y+(up*sl).y,loc.z+(right*sl).z+(up*sl).z);
		glEnd();
	}
	//////////////////////////////////////////SMOKE/////////////////////////////////////////////////////////////
	glColor4f(1,1,1,max(0.4-time/2500*0.4,0.0));
	dataManager.bind("explosion smoke");
	for(vector<smokeParticle>::iterator i=smokeParticles.begin();i!=smokeParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100));
		sl = 120.0*(1.0-pow(2.718f,-(time+50)/100));
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex3f(loc.x-(right*sl).x+(up*sl).x,loc.y-(right*sl).y+(up*sl).y,loc.z-(right*sl).z+(up*sl).z);
			glTexCoord2f(0,0);	glVertex3f(loc.x-(right*sl).x-(up*sl).x,loc.y-(right*sl).y-(up*sl).y,loc.z-(right*sl).z-(up*sl).z);
			glTexCoord2f(1,0);	glVertex3f(loc.x+(right*sl).x-(up*sl).x,loc.y+(right*sl).y-(up*sl).y,loc.z+(right*sl).z-(up*sl).z);
			glTexCoord2f(1,1);	glVertex3f(loc.x+(right*sl).x+(up*sl).x,loc.y+(right*sl).y+(up*sl).y,loc.z+(right*sl).z+(up*sl).z);
		glEnd();
	}


	//if(time<270 && flash)
	//{
	//	glDisable(GL_DEPTH_TEST);
	//	dataManager.bind("explosion flash");
	//	s=time*6;
	//	if(time<0.1)
	//		glColor4f(1,1,1,clamp(time/100,0.0,1.0));
	//	if(time>0.2)
	//		glColor4f(1,1,1,clamp(6.71-time/70,0.0,1.0));
	//	glBegin(GL_QUADS);
	//		glTexCoord2f(0,1);	glVertex3f(pos.x-up.x*s-right.x*s	,pos.y-up.y*s-right.y*s	,pos.z-up.z*s-right.z*s	);
	//		glTexCoord2f(0,0);	glVertex3f(pos.x+up.x*s-right.x*s	,pos.y+up.y*s-right.y*s	,pos.z+up.z*s-right.z*s	);
	//		glTexCoord2f(1,0);	glVertex3f(pos.x+up.x*s+right.x*s	,pos.y+up.y*s+right.y*s	,pos.z+up.z*s+right.z*s	);
	//		glTexCoord2f(1,1);	glVertex3f(pos.x-up.x*s+right.x*s	,pos.y-up.y*s+right.y*s	,pos.z-up.z*s+right.z*s	);
	//	glEnd();
	//	glEnable(GL_DEPTH_TEST);
	//}
}
void explosion::render2()
{
	float time=world.time()-StartTime;//in ms
	if(time < 0) return;

	float s=max((1.0-pow(2.718f,-(time-100)/20))*200.0,0.0);
	Vec3f loc; 
	
	dataManager.bind("explosion fireball");
		//Make space for particle limits and fill it from OGL call.
		GLfloat sizes[2];
		glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes);
	
		//Enable point sprites and set params for points.
		glEnable(GL_POINT_SPRITE_ARB);
		float quadratic[] =  { 0.0f, 0.0f, 0.000001f };
		glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
		//glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 160.0f );
	
		//Tell it the max and min sizes we can use using our pre-filled array.
		glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1 );
		glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 8192 );
	
		//Tell OGL to replace the coordinates upon drawing.
		glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	
		//int viewport[4];
		//float mat[16];
		//glGetFloatv(GL_PROJECTION_MATRIX,	mat);
		//glGetIntegerv(GL_VIEWPORT,viewport);
		//float H = viewport[2];
		//float h = 2.0f/mat[0];
		//float D0 = sqrt(2.0f)*H/h;
		//float k = 1.0f/(1.0f+2.0*sqrt(1/mat[0]));
		//float atten[3] = {0,0,sqrt(1.0/D0)*k};
		//glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, atten);
		//glEnable(GL_POINT_SMOOTH);

	//glPointSize(   clamp( 200.0*(1.0-pow(2.718f,-(time+50)/100)), sizes[0], sizes[1])   );
	glPointSize(   100   );
	glColor4f(1,1,1,max(0.6-time/750*0.6,0.0));
	glBegin(GL_POINTS);
	for(vector<smokeParticle>::iterator i=fireParticles.begin();i!=fireParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100))*0.3;
		glVertex3f(loc.x,loc.y,loc.z);
	}
	glEnd();
	dataManager.bind("explosion smoke");
	glPointSize(150);
	glColor4f(1,1,1,max(0.4-time/1500*0.4,0.0));
	glBegin(GL_POINTS);
	for(vector<smokeParticle>::iterator i=smokeParticles.begin();i!=smokeParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100))*0.7;
		glVertex3f(loc.x,loc.y,loc.z);
	}
	glEnd();
}
void explosion::render3()
{
	float time=world.time()-StartTime;//in ms
	if(time < 0) return;
	Vec3f loc;
	for(vector<smokeParticle>::iterator i=fireParticles.begin();i!=fireParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100))*0.3;
		graphics->drawParticle(fireParticleEffect,loc,Color(1,1,1,max(0.6-time/750*0.6,0.0)));
	}
	for(vector<smokeParticle>::iterator i=smokeParticles.begin();i!=smokeParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100))*0.7;
		graphics->drawParticle(smokeParticleEffect,loc,Color(1,1,1,max(0.4-time/1500*0.4,0.0)));
	}
}