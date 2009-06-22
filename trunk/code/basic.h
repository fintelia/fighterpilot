
void drawCircle(float x, float y, float radius,int type)
{
	glBegin(type);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i * 3.14159f/180.0f;
		glVertex3f(x+cos(degInRad)*radius, y+sin(degInRad)*radius,1.0f);
	}
	glEnd();
}

//
//void DrawText(float x, float y, float z,string String)
//{
//	char c;
//	int i;
//	glRasterPos3f(x, y ,z);
//	for (i=0; i < (signed int)String.length(); i++) {
//		c=(char)String.at(i);
//		BitmapCharacter(c);
//	}
//}
//void DrawText(float x, float y, float z,int number)
//{
//	string String;
//	stringstream ssl;
//	ssl << number;
//	String=ssl.str();
//	char c;
//	int i;
//	glRasterPos3f(x, y ,z);
//	for (i=0; i < (signed int)String.length(); i++) {
//		c=(char)String.at(i);
//		BitmapCharacter(c);
//	}
//}
//void DrawText(float x, float y, float z,double number)
//{
//	string String;
//	stringstream ssl;
//	ssl << number;
//	String=ssl.str();
//	char c;
//	int i;
//	glRasterPos3f(x, y ,z);
//	for (i=0; i < (signed int)String.length(); i++) {
//		c=(char)String.at(i);
//		BitmapCharacter(c);
//	}
//}
//void DrawText(float x, float y,string String)
//{
//	char c;
//	int i;
//	glRasterPos3f(x, y ,1);
//	for (i=0; i < (signed int)String.length(); i++) {
//		c=(char)String.at(i);
//		BitmapCharacter(c);
//	}
//}
//void DrawText(float x, float y,int number)
//{
//	string String;
//	stringstream ssl;
//	ssl << number;
//	String=ssl.str();
//	glListBase (1000);
//	glCallList(24, GL_UNSIGNED_BYTE,String.c_str());
//	glListBase (0);
//}
//void DrawText(float x, float y,double number)
//{
//	string String;
//	stringstream ssl;
//	ssl << number;
//	String=ssl.str();
//	glListBase (1000);
//	glCallList(24, GL_UNSIGNED_BYTE,String.c_str());
//	glListBase (0);
//}

void RenderText(float x,float y,string String)
{
	glListBase(fontBase);
	glRasterPos2i(x,y);
	glCallLists(String.length(), GL_UNSIGNED_BYTE,String.c_str());
	glListBase(0);
}
void RenderText(float x,float y,int number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	glListBase(fontBase);
	glRasterPos2i(x,y);
	glCallLists(String.length(), GL_UNSIGNED_BYTE,String.c_str());
	glListBase(0);
}
void RenderText(float x,float y,float number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	glListBase(fontBase);
	glRasterPos2i(x,y);
	glCallLists(String.length(), GL_UNSIGNED_BYTE,String.c_str());
	glListBase(0);
}

char *textFileRead(char *fn) {


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fopen_s(&fp,fn,"rt");

		if (fp != NULL) {
      
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}