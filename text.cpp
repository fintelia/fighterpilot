
#include "engine.h"

void TextManager::renderCharacter(char c, int x, int y)
{
	if(c<32 || c>=128) return;

	double	cx=(double)chars[c].x/textureWidth+0.001,
			cy=(double)chars[c].y/textureWidth+0.001,
			cwidth=(double)chars[c].width/textureWidth-0.002,
			cheight=(double)chars[c].height/textureWidth-0.002;

	graphics->drawPartialOverlay(Rect::XYWH(x,y,chars[c].width,chars[c].height),Rect::XYWH(cx,cy,cwidth,cheight));
}

void TextManager::renderText(string text, int x, int y)
{
	dataManager.bindTex(textureId);

	int xpos=x;
	int height=maxCharHeight(text);
	int line=0;
	for(string::iterator i = text.begin();i != text.end();i++)
	{
		if(*i>=32 && *i < 128)
		{
			renderCharacter(*i,xpos,y+height*line);
			xpos+=chars[*i].width;
		}
		else if(*i == '\n')
		{
			xpos = x;
			line++;
		}
	}

	dataManager.bindTex(0);
}