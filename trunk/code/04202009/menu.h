
class menu:public modes
{
public:
	int selected;
	menu()
	{
		selected=1;
		glutPostRedisplay();
		newMode=0;
	}
	int update(int value)
	{
		static int repeat=0;
		static float up=1;
		static float down=1;

		int move=0;
		if(input->keys[UP]&&input->keys[DOWN]){}
			//do nothing
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

		if(selected>5)
			selected-=5;
		if(selected<1)
			selected+=5;

		if(input->keys[SPACE] || input->keys[ENTER])
		{
			input->keys[SPACE]=false;
			input->keys[ENTER]=false;
			switch(selected)
			{
			case 1:
				newMode=3;
				glutPostRedisplay();
				break;
			//case 2:
			//	newMode=4;
			//	glutPostRedisplay();
			//	break;
			case 5:
				newMode=7;
				glutPostRedisplay();
				break;
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
		glBindTexture(GL_TEXTURE_2D, menus[0]);
		glBegin(GL_QUADS);
		for(int i=1;i<=5;i++)
		{
			if(i==selected)
			{		
				glTexCoord2f(0.5,1.2-0.2*i);	glVertex2f(202,i*100);
				glTexCoord2f(1  ,1.2-0.2*i);	glVertex2f(202+395,i*100);
				glTexCoord2f(1  ,1.0-0.2*i);	glVertex2f(202+395,i*100+52);
				glTexCoord2f(0.5,1.0-0.2*i);	glVertex2f(202,i*100+52);
			}
			else
			{
				glTexCoord2f(0.01  ,1.2-0.2*i);	glVertex2f(202,i*100);
				glTexCoord2f(0.49,1.2-0.2*i);	glVertex2f(202+395,i*100);
				glTexCoord2f(0.49,1.0-0.2*i);	glVertex2f(202+395,i*100+52);
				glTexCoord2f(0.01  ,1.0-0.2*i);	glVertex2f(202,i*100+52);
			}
		}
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		ViewPerspective();
		glutSwapBuffers();
	}
};