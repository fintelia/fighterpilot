
#include "main.h"
explosion::explosion(Vec3f Pos): pos(Pos), coreSize(0.0), fireballSize(0.0)
{
	StartTime=gameTime();
	smokeParticle s;
	for(int i=0;i<20;i++)
	{
		s.initialVelocity=Vec3f((float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500).normalize()*((float)(rand()%2000)/7+25.0);
		//s.position=Pos+Vec3f((float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500,(float)(rand()%1000-500)/500).normalize()*10;
		smokeParticles.push_back(s);
	}
}
void explosion::render(bool flash)
{
	float time=gameTime()-StartTime;//in ms
	if(time < 0) return;

	Vec3f up, right;
	l3dBillboardGetUpRightVector(up,right);
	dataManager.bind("explosion core");
	//float s=(1.0-pow(2.718f,-(time)/42))*200.0;
	float s=max((1.0-pow(2.718f,-(time-100)/20))*200.0,0.0);
	//glColor4f(1,1,1,clamp(2.0-time/1000,0.0,1.0));
	glColor4f(1,1,1,clamp(1.5-time/1000,0.0,1.0));
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex3f(pos.x-up.x*s-right.x*s	,pos.y-up.y*s-right.y*s	,pos.z-up.z*s-right.z*s	);
		glTexCoord2f(0,0);	glVertex3f(pos.x+up.x*s-right.x*s	,pos.y+up.y*s-right.y*s	,pos.z+up.z*s-right.z*s	);
		glTexCoord2f(1,0);	glVertex3f(pos.x+up.x*s+right.x*s	,pos.y+up.y*s+right.y*s	,pos.z+up.z*s+right.z*s	);
		glTexCoord2f(1,1);	glVertex3f(pos.x-up.x*s+right.x*s	,pos.y-up.y*s+right.y*s	,pos.z-up.z*s+right.z*s	);
	glEnd(); 
	glColor4f(1,1,1,0.4);
	dataManager.bind("explosion fireball");
	s=max((1.0-pow(2.718f,-(time-100)/20))*200.0,0.0);
	glColor4f(1,1,1,clamp(1.5-time/1000,0.0,0.5));
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex3f(pos.x-up.x*s-right.x*s	,pos.y-up.y*s-right.y*s	,pos.z-up.z*s-right.z*s	);
		glTexCoord2f(0,0);	glVertex3f(pos.x+up.x*s-right.x*s	,pos.y+up.y*s-right.y*s	,pos.z+up.z*s-right.z*s	);
		glTexCoord2f(1,0);	glVertex3f(pos.x+up.x*s+right.x*s	,pos.y+up.y*s+right.y*s	,pos.z+up.z*s+right.z*s	);
		glTexCoord2f(1,1);	glVertex3f(pos.x-up.x*s+right.x*s	,pos.y-up.y*s+right.y*s	,pos.z-up.z*s+right.z*s	);
	glEnd();

	//float d, d2, width, h1, h2;
	//Angle theta, alpha;
	//Vec3f direction,v1,v2,v3,v4;
	//glColor4f(0.9,0.9,0.9,clamp(1.15-time/2000,0.0,1.0));
	//
	//dataManager.bind("explosion smoke");
	//for(vector<smokeParticle>::iterator i=smokeParticles.begin();i!=smokeParticles.end();i++)
	//{	
	//	direction=i->initialVelocity.normalize();
	//	if(direction.dot(up.cross(right))>=0)
	//	{
	//		theta=atan2A(direction.dot(right),direction.dot(up));
	//		d=(1.0-pow(2.718f,-(time)/15))*i->initialVelocity.magnitude();
	//		d2=max(d-160,0.01);
	//		width=(time+3500)/20;		
	//		h1=sqrt(d*d+width*width/4);
	//		h2=sqrt(d2*d2+width*width/4);
	//	

	//		v1=pos+right*cos(theta-atan2A(width/2,d))*h1	+up*sin(theta-atan2A(width/2,d))*h1;
	//		v2=pos+right*cos(theta+atan2A(width/2,d))*h1	+up*sin(theta+atan2A(width/2,d))*h1;
	//		v3=pos+right*cos(theta+atan2A(width/2,d2))*h2	+up*sin(theta+atan2A(width/2,d2))*h2;
	//		v4=pos+right*cos(theta-atan2A(width/2,d2))*h2	+up*sin(theta-atan2A(width/2,d2))*h2;
	//		glBegin(GL_QUADS);
	//			glTexCoord2f(0,1);	glVertex3f(v1.x,v1.y,v1.z);
	//			glTexCoord2f(0,0);	glVertex3f(v2.x,v2.y,v2.z);
	//			glTexCoord2f(1,0);	glVertex3f(v3.x,v3.y,v3.z);
	//			glTexCoord2f(1,1);	glVertex3f(v4.x,v4.y,v4.z);
	//		glEnd();
	//	}
	//}
	Vec3f loc;
	float sl;
	dataManager.bind("explosion smoke");
	for(vector<smokeParticle>::iterator i=smokeParticles.begin();i!=smokeParticles.end();i++)
	{
		loc = pos + i->initialVelocity*(1.0-pow(2.718f,-(time)/100))*0.7;
		sl = 100.0*(1.0-pow(2.718f,-(time+50)/100));
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