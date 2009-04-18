
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


void DrawText(float x, float y, float z,string String)
{
	char c;
	int i;
	glRasterPos3f(x, y ,z);
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(float x, float y, float z,int number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	char c;
	int i;
	glRasterPos3f(x, y ,z);
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(float x, float y, float z,double number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	char c;
	int i;
	glRasterPos3f(x, y ,z);
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(float x, float y,string String)
{
	char c;
	int i;
	glRasterPos3f(x, y ,1);
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(float x, float y,int number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	char c;
	int i;
	glRasterPos3f(x, y ,1);
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(float x, float y,double number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	char c;
	int i;
	glRasterPos3f(x, y ,1);
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(string String)
{
	char c;
	int i;
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(int number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	char c;
	int i;
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}
void DrawText(double number)
{
	string String;
	stringstream ssl;
	ssl << number;
	String=ssl.str();
	char c;
	int i;
	for (i=0; i < (signed int)String.length(); i++) {
		c=(char)String.at(i);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}