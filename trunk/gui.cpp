
#include <string>
#include <fstream>
#include <map>

#include <boost\lexical_cast.hpp>
#include <boost\tokenizer.hpp>

#include "GL/glee.h"
#include <GL/glu.h>

using namespace std;
using namespace boost;

#include "enums.h"
#include "text.h"
#include "gui.h"

extern int sw;
extern int sh;


guiBase::handle guiBase::newLabel(string t, float x, float y)
{
	label* l=new label;
	l->id=++currentId;
	l->type=LABEL;
	l->x=x;
	l->y=y;
	l->text=t;
	elements.insert(pair<handle,element*>(currentId,l));
	return currentId;
}
guiBase::handle guiBase::newButton(string t, float x, float y,float width, float height, functor<void,callBack>* callback)
{
	button* b = new button;
	b->id=++currentId;
	b->type=BUTTON;
	b->x=x;
	b->y=y;
	b->width=width;
	b->height=height;
	b->callback=callback;
	b->text=t;
	elements.insert(pair<handle,element*>(currentId,b));
	return currentId;
}
guiBase::handle guiBase::newMsgBox(string t)
{
	msgBox* m = new msgBox;
	m->id = m->id=++currentId;
	m->type=MSGBOX;
	m->width=(float)max(textManager->getTextWidth(t)+10,110);
	m->height=(float)textManager->getTextHeight(t)+50;
	m->x=(sw-m->width)/2;
	m->y=(sh-m->height)/2;
	m->text=t;

	button b;
	b.x=m->x+m->width/2-50;
	b.y=m->y+m->height-40;
	b.height=35;
	b.width=100;
	b.text="OK";
	m->buttons.push_back(b);
	elements.insert(pair<handle,element*>(currentId,m));
	currentFocus=currentId;
	draggingMsg=false;
	return currentId;
}
void guiBase::draw()
{
	POINT cursorPos;
    GetCursorPos(&cursorPos);
	if(draggingMsg)
	{

		((msgBox*)elements[currentFocus])->x+=cursorPos.x-dragX;
		((msgBox*)elements[currentFocus])->y+=cursorPos.y-dragY;
		((msgBox*)elements[currentFocus])->buttons.front().x+=cursorPos.x-dragX;
		((msgBox*)elements[currentFocus])->buttons.front().y+=cursorPos.y-dragY;
		dragX=cursorPos.x;
		dragY=cursorPos.y;
	}


	glEnable(GL_BLEND);
	glColor4f(1,1,1,1);
	float x,y;
	for(map<handle,element*>::iterator i = elements.begin(); i != elements.end();i++)
	{
		if(i->second->type == BUTTON)
		{
			glColor4f(1,1,1,1);
			button* b=(button*)i->second;
			glBegin(GL_QUADS);
				
				glVertex2f(b->x,b->y);
				glVertex2f(b->x+b->width,b->y);
				glVertex2f(b->x+b->width,b->y+b->height);
				glVertex2f(b->x,b->y+b->height);
			glEnd();

			glColor4f(0,0,0,1);
			x = b->x + (b->width - textManager->getTextWidth(b->text))/2;
			y = b->y + (b->height - textManager->getTextHeight(b->text))/2;
			textManager->renderText(b->text,(int)x,(int)y);
		}
		else if(i->second->type == LABEL)
		{
			label* b=(label*)i->second;
			glColor4f(0,0,0,1);
			textManager->renderText(b->text,(int)b->x,(int)b->y);
		}
		else if(i->second->type == MSGBOX)
		{
			msgBox* n=(msgBox*)i->second;
			glColor4f(1,1,1,1);

			glBegin(GL_QUADS);
				glVertex2f(n->x,n->y);
				glVertex2f(n->x+n->width,n->y);
				glVertex2f(n->x+n->width,n->y+n->height);
				glVertex2f(n->x,n->y+n->height);
			glEnd();

			
			for(vector<button>::iterator l = n->buttons.begin();l!=n->buttons.end();l++)
			{
				glColor4f(0,1,0,1);
				glBegin(GL_QUADS);
					glVertex2f(l->x,l->y);
					glVertex2f(l->x+l->width,l->y);
					glVertex2f(l->x+l->width,l->y+l->height);
					glVertex2f(l->x,l->y+l->height);
				glEnd();

				glColor4f(0,0,0,1);
				x = l->x + (l->width - textManager->getTextWidth(l->text))/2;
				y = l->y + (l->height - textManager->getTextHeight(l->text))/2;
				textManager->renderText(l->text,(int)x,(int)y);

			}

			glColor4f(0,0,0,1);
			x = n->x + (n->width - textManager->getTextWidth(n->text))/2;
			//y = n->y + (n->height - text.getTextHeight(n->text))/2;
			textManager->renderText(n->text,(int)x,(int)n->y+5);
		}



	}
	glBegin(GL_TRIANGLES);
		glVertex2f((float)cursorPos.x,(float)cursorPos.y);
		glVertex2f((float)cursorPos.x,(float)cursorPos.y+20);
		glVertex2f((float)cursorPos.x+10,(float)cursorPos.y+16);
	glEnd();

}
void guiBase::mouseUpdate(bool left,bool right, int x, int y)
{
	static bool lLeft=left;
	static bool lRight=right;
	static int lX=x;
	static int lY=y;

	if(left && !lLeft)
	{
		if(currentFocus != 0)
		{
			if(elements[currentFocus]->type == MSGBOX)
			{
				button b=((msgBox*)elements[currentFocus])->buttons.front();
				msgBox m=*(msgBox*)elements[currentFocus];
				if(b.x < x && b.y < y && b.x+b.width > x && b.y+b.height > y)
				{
					delete elements[currentFocus];
					elements.erase(elements.find(currentFocus));
					currentFocus=0;
				}
				else if(m.x < x && m.y < y && m.x+m.width > x && m.y+m.height > y)
				{
					draggingMsg=true;
					dragX=x;
					dragY=y;
				}
			}
		}
		else
		{
			for(map<handle,element*>::iterator i = elements.begin(); i != elements.end();i++)
			{
				if(i->second->type == BUTTON)
				{
					button* b=(button*)i->second;
					if(b->x < x && b->y < y && b->x+b->width > x && b->y+b->height > y)
						b->issueCallBack();
				}
			}
		}
	}
	if(!left)	draggingMsg=false;

	lLeft=left;
	lRight=right;
	lX=x;
	lY=y;
}

void guiBase::editElement(handle id, string t)
{
	if(elements.find(id) != elements.end())
	{
		elements[id]->text=t;
	}
}
void guiBase::editElement(handle id, float x, float y)
{
	if(elements.find(id) != elements.end())
	{
		elements[id]->x=x;
		elements[id]->y=y;
	}
}
void guiBase::editElement(handle id, string t, float x, float y)
{
	if(elements.find(id) != elements.end())
	{
		elements[id]->x=x;
		elements[id]->y=y;
		elements[id]->text=t;
	}
}
void guiBase::editElement(handle id, float x, float y, float width, float height)
{
	if(elements.find(id) != elements.end())
	{
		elements[id]->x=x;
		elements[id]->y=y;
		elements[id]->width=width;
		elements[id]->height=height;
	}
}
void guiBase::editElement(handle id, string t, float x, float y, float width, float height)
{
	if(elements.find(id) != elements.end())
	{
		elements[id]->x=x;
		elements[id]->y=y;
		elements[id]->width=width;
		elements[id]->height=height;
		elements[id]->text=t;
	}
}
