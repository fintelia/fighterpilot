
struct vert
{
	float x;
	float y;
	float z;
};

void normalize (vert *v)
{
    // calculate the length of the vector
    float len = (float)(sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)));

    // avoid division by 0
    if (len == 0.0f)
        len = 1.0f;

    // reduce to unit size
    v->x /= len;
    v->y /= len;
    v->z /= len;
}


// normal(); - finds a normal vector and normalizes it
void normal (vert v[3], vert *normal)
{
    vert a, b;

    // calculate the vectors A and B
    // note that v[3] is defined with counterclockwise winding in mind
    // a
    a.x = v[0].x - v[1].x;
    a.y = v[0].y - v[1].y;
    a.z = v[0].z - v[1].z;
    // b
    b.x = v[1].x - v[2].x;
    b.y = v[1].y - v[2].y;
    b.z = v[1].z - v[2].z;

    // calculate the cross product and place the resulting vector
    // into the address specified by vertex_t *normal
    normal->x = (a.y * b.z) - (a.z * b.y);
    normal->y = (a.z * b.x) - (a.x * b.z);
    normal->z = (a.x * b.y) - (a.y * b.x);

    // normalize
    normalize(normal);
}


int load_texture(char *filename)
{
	
	ifstream fin(filename);
	vert v[4];
	int i = glGenLists (1);
	glNewList (i, GL_COMPILE);
	glScalef(0.015f,0.015f,0.015f);
	//glTranslatef(0,0,50);
	glRotatef(-90,1,0,0);
	vert norm;
	while (!fin.eof())
	{
		fin >> v[0].x >> v[0].y >> v[0].z;
		fin	>> v[1].x >> v[1].y >> v[1].z; 
		fin >> v[2].x >> v[2].y >> v[2].z; 
		//fin >> v[3].x >> v[3].y >> v[3].z;
		//glColor3f(v[0].x,v[0].y,v[0].z);
		normal(v,&norm);
		glBegin(GL_TRIANGLES);
			glNormal3f(norm.x,norm.y,norm.z);
			glVertex3f(v[0].x,v[0].y,v[0].z);
			glVertex3f(v[1].x,v[1].y,v[1].z);
			glVertex3f(v[2].x,v[2].y,v[2].z);
			//glVertex3f(v[3].x,v[3].y,v[3].z);
		glEnd();
	}
	glEndList();
	return i;
}