
class setControls: public modes
{
public:
	int selected;
	int side;//0,1;
	bool choosing;
	int pressCount;
	setControls()
	{
		selected=1;
		side=0;
		glutPostRedisplay();
		newMode=0;
		choosing=false;
	}
	void updateMove()
	{
		static int repeat=0;
		static float up=1;
		static float down=1;
		static int sideRepeat=1;
		static float left=1;
		static float right=1;
		int move=0;

		if(input->keys[LEFT]&&!input->keys[RIGHT])
		{
			if(sideRepeat!=-1||left<=0)
			{
				move=-1;
				left=1;
			}
			else
			{
				left-=0.2;
			}
			sideRepeat=-1;
			right=1;
		}
		else if(!input->keys[LEFT]&&input->keys[RIGHT])
		{
			if(sideRepeat!=1||right<=0)
			{
				move=1;
				right=1;
			}
			else
			{
				right-=0.2;
			}
			sideRepeat=1;
			left=1;
		}
		else if(!input->keys[LEFT]&&!input->keys[RIGHT])
		{
			right=1;
			left=1;
			sideRepeat=0;
		}
	
		if(move!=0)
		{
			side+=move;
			glutPostRedisplay();
		}

		move=0;

		if(input->keys[UP]&&!input->keys[DOWN])
		{
			if(repeat!=-1||up<=0)
			{
				move=-1;
				up=1;
			}
			else
			{
				up-=0.2;
			}
			repeat=-1;
			down=1;
		}
		else if(!input->keys[UP]&&input->keys[DOWN])
		{
			if(repeat!=1||down<=0)
			{
				move=1;
				down=1;
			}
			else
			{
				down-=0.2;
			}
			repeat=1;
			up=1;
		}
		else if(!input->keys[UP]&&!input->keys[DOWN])
		{
			down=1;
			up=1;
			repeat=0;
		}
	
		if(move!=0)
		{
			selected+=move;
			glutPostRedisplay();
		}

		if(selected>9)
			selected-=9;
		if(selected<1)
			selected+=9;
		if(side>=2)
			side-=2;
		if(side<0)
			side+=2;
	}
	int update(int value)
	{
		if(choosing)
		{
			if(input->tPresses>pressCount)
			{
				controls[side].c[selected-1].newControl(input->lastKey,input->lastAscii);
				*input->lastKey=false;
				choosing=false;
				glutPostRedisplay();
			}
		}
		else
		{
			updateMove();
			if(input->keys[SPACE] || input->keys[ENTER])
			{
				input->keys[SPACE]=false;
				input->keys[ENTER]=false;
				if(selected!=9)
				{
					choosing=true;
					pressCount=input->tPresses;
					glutPostRedisplay();
				}
				else
				{
					newMode=2;
					glutPostRedisplay();
				}
			}
		}
		return 30;
	}
	void draw()
	{
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.4f,0.4f,0.4f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0 , sw, sh);
		glLoadIdentity();
		gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 10000.0);
		ViewOrtho(sw,sh);
		glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
		glEnable(GL_BLEND);
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2f(0,0);
			glTexCoord2f(0,0);	glVertex2f(0,600);
			glTexCoord2f(1,0);	glVertex2f(800,600);		
			glTexCoord2f(1,1);	glVertex2f(800,0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, menus[1]);
		glBegin(GL_QUADS);
		for(int i=1;i<=8;i++)
		{
				glTexCoord2f(0,1.077-0.077*i);		glVertex2f(202,i*50+50);
				glTexCoord2f(1,1.077-0.077*i);		glVertex2f(202+395,i*50+50);
				glTexCoord2f(1,1.000-0.077*i);		glVertex2f(202+395,i*50+102);
				glTexCoord2f(0,1.000-0.077*i);		glVertex2f(202,i*50+102);			
		}
		glEnd();
		glBindTexture(GL_TEXTURE_2D, tex[12]);
		for(int l=0;l<2;l++)
		{
			for(int i=1;i<=8;i++)
			{
				if(i==selected && l==side && !choosing)
					glColor3f(1,1,1);
				else if(i==selected && l==side)
					glColor3f(1,0.3,0.3);
				else
					glColor3f(0.5,0.5,0.5);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2f(228+280*l,i*50+50);
					glTexCoord2f(1,0);	glVertex2f(228+48+280*l,i*50+50);
					glTexCoord2f(1,1);	glVertex2f(228+48+280*l,i*50+98);
					glTexCoord2f(0,1);	glVertex2f(228+280*l,i*50+98);
				glEnd();
			}
		}
		float texX=0.125;
		float texY=0.875;
		int asciiV;
		glBindTexture(GL_TEXTURE_2D,tex[13]);
		for(int l=0;l<2;l++)
		{
			for(int i=1;i<=8;i++)
			{
				texX=0.875;
				texY=0.125;
				asciiV=controls[l].c[i-1].my_char;
				if(asciiV>=97 && asciiV<=122)
				{
					asciiV-=97;
					texX=(asciiV%8);
					texY=(asciiV-asciiV%8)/8;
					texX=(texX/8.0);
					texY=1.0-(texY/8.0);
				}
				else if(asciiV>=48 && asciiV<=57)
				{
					asciiV-=20;
					texX=(asciiV%8);
					texY=(asciiV-asciiV%8)/8;
					texX=(texX/8.0);
					texY=1.0-(texY/8.0);
					if(asciiV==28)
					{
						texX=0.75;
						texY=0.5;
					}
				}
				else if(asciiV>=228 && asciiV<=231)
				{
					asciiV=abs(asciiV-231)+39;
					texX=(asciiV%8);
					texY=(asciiV-asciiV%8)/8;
					texX=(texX/8.0);
					texY=1.0-(texY/8.0);
				}

				glBegin(GL_QUADS);
					glTexCoord2f(texX,texY);				glVertex2f(228+280*l,i*50+50);
					glTexCoord2f(texX+0.125,texY);			glVertex2f(228+48+280*l,i*50+50);
					glTexCoord2f(texX+0.125,texY-0.125);	glVertex2f(228+48+280*l,i*50+98);
					glTexCoord2f(texX,texY-0.125);			glVertex2f(228+280*l,i*50+98);
				glEnd();
			}
		}
		int i=9;
		if(selected==9)
			i=10;
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D,menus[1]);
		glBegin(GL_QUADS);
				glTexCoord2f(0,1.077-0.077*i);		glVertex2f(202,450+50);
				glTexCoord2f(1,1.077-0.077*i);		glVertex2f(202+395,450+50);
				glTexCoord2f(1,1.000-0.077*i);		glVertex2f(202+395,450+102);
				glTexCoord2f(0,1.000-0.077*i);		glVertex2f(202,450+102);		
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		ViewPerspective();
		glutSwapBuffers();
	}
};