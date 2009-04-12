
class modes
{
public:
	int newMode;
	virtual int update(int value)=0;
	void ViewOrtho(int x, int y)				// Set Up An Ortho View
	{
		glMatrixMode(GL_PROJECTION);			// Select Projection
		glPushMatrix();							// Push The Matrix
		glLoadIdentity();						// Reset The Matrix
		glOrtho( 0, x , y , 0, -1, 1 );			// Select Ortho Mode
		glMatrixMode(GL_MODELVIEW);				// Select Modelview Matrix
		glPushMatrix();							// Push The Matrix
		glLoadIdentity();						// Reset The Matrix
	}
	void ViewPerspective(void)					// Set Up A Perspective View
	{
		glMatrixMode( GL_PROJECTION );			// Select Projection
		glPopMatrix();							// Pop The Matrix
		glMatrixMode( GL_MODELVIEW );			// Select Modelview
		glPopMatrix();							// Pop The Matrix
	}
	virtual void draw()=0;
};