
class setControls: public modes
{
public:
	int selected;
	int side;//0,1;
	setControls()
	{
		selected=1;
		side=0;
		glutPostRedisplay();
		newMode=0;
	}
	int update(int value)
	{
		static int repeat=0;
		static float up=1;
		static float down=1;

		int move=0;

		if(input->keys[RIGHT])
		{
			glutPostRedisplay();
			side++;
		}
		if(input->keys[LEFT])
		{
			glutPostRedisplay();
			side++;
		}
		if(side>=2)
			side-=2;

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

		//if(input->keys[SPACE] || input->keys[ENTER])
		//{
		//	switch(selected)
		//	{
		//	case 1:
		//		newMode=3;
		//		glutPostRedisplay();
		//		break;
		//	case 2:
		//		newMode=4;
		//		glutPostRedisplay();
		//		break;
		//	}
		//}
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
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		for(int l=0;l<2;l++)
		{
			for(int i=1;i<=5;i++)
			{
				if(i==selected && l==side)
					glColor3f(1,0.5,0.5);
				else
					glColor3f(0.5,0.5,0.5);
				glBegin(GL_QUADS);
					glVertex2f(52+400*l,i*100);
					glVertex2f(52+295+400*l,i*100);
					glVertex2f(52+295+400*l,i*100+52);
					glVertex2f(52+400*l,i*100+52);
				glEnd();
			}
		}
		ViewPerspective();
		glutSwapBuffers();
	}
};