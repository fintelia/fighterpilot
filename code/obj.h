////
////struct vert
////{
////	float x;
////	float y;
////	float z;
////};
////void normalize (vert *v)
////{
////    // calculate the length of the vector
////    float len = (float)(sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)));
////
////    // avoid division by 0
////    if (len == 0.0f)
////        len = 1.0f;
////
////    // reduce to unit size
////    v->x /= len;
////    v->y /= len;
////    v->z /= len;
////}
////
////
////// normal(); - finds a normal vector and normalizes it
////void normal (vert v[3], vert *normal)
////{
////    vert a, b;
////
////    // calculate the vectors A and B
////    // note that v[3] is defined with counterclockwise winding in mind
////    // a
////    a.x = v[0].x - v[1].x;
////    a.y = v[0].y - v[1].y;
////    a.z = v[0].z - v[1].z;
////    // b
////    b.x = v[1].x - v[2].x;
////    b.y = v[1].y - v[2].y;
////    b.z = v[1].z - v[2].z;
////
////    // calculate the cross product and place the resulting vector
////    // into the address specified by vertex_t *normal
////    normal->x = (a.y * b.z) - (a.z * b.y);
////    normal->y = (a.z * b.x) - (a.x * b.z);
////    normal->z = (a.x * b.y) - (a.y * b.x);
////
////    // normalize
////    normalize(normal);
////}
////
//
class uv
{
public:
	float u;
	float v;
	uv(){}
	uv(float u2,float v2)
	{
		u=u2;
		v=v2;
	}
};
class mtl
{
public:
	int tex;
	mtl(int t)
	{
		tex=t;
	}
	mtl(){}
};
class tri
{
public:
	int vs[6];
	string my_mtl;
	tri(){}
	tri(int v1,int v2,int v3,int t1,int t2,int t3,string _mtl)
	{
		vs[0]=v1;
		vs[1]=v2;
		vs[2]=v3;
		vs[3]=t1;
		vs[4]=t2;
		vs[5]=t3;
		my_mtl=_mtl;
	}
	int operator[](int index) const {
		return vs[index];
	}
};
map<string,mtl> load_mtl(string filename)
{
	string mstring;
	mstring="";
	map<string,mtl> m;
	ifstream fin(filename.c_str());
	string file(filename);
	int i=file.find_last_of("/");
	if(i==string::npos)
		file.assign("");
	else
		file=file.substr(0,i+1);
	while (!fin.eof())
	{
		string line;
		char l[256];
		string s[4];
		int i=0;
		int h=0;
		fin.getline(l,256);
		line.assign(l);
		h=line.find(" ");
		if(h!=string::npos)
		{
			s[0].assign(line.substr(0,h));
			s[1].assign(line.substr(h+1,line.size()-h-1));
		}
		if(s[0].compare("newmtl")==0)
		{
			mstring=s[1];
		}
		if(s[0].compare("map_Kd")==0)
		{
			assert(mstring.compare(""));
			m.insert(pair<string,mtl>(mstring,LoadBitMap( (char*)(file+s[1]).c_str())) );
			mstring="";
		}
	}
	return m;
}
int load_model(char *filename)
{
	ifstream fin(filename);
	string file(filename);
	int i=file.find_last_of("/");
	if(i==string::npos)
		file.assign("");
	else
		file=file.substr(0,i+1);
	vector<vert> vertices;
	vector<uv> UV;
	vector<tri> triangles;
	map<string,mtl> mtls;
	string currentMTL;
	while (!fin.eof())
	{
		string line;
		char l[256];
		string s[4];
		int i=0;
		int h=0;
		fin.getline(l,256);
		line.assign(l);
		h=line.find(" ");
		for(int n=0;n<4;n++)
		{
			s[n].assign(line.substr(i,h-i));
			i=h+1;
			if(i>line.size())
				i=line.size();
			h=line.find(" ",h+1);
			if(h==string::npos)
				h=line.size();
		}

		//fin >> s[0] >> s[1] >> s[2] >> s[3];
		if(s[0].compare("v")==0)
		{
			vert v(atof(s[1].c_str()),atof(s[2].c_str()),atof(s[3].c_str()));
			vertices.push_back(v);
		}
		else if(s[0].compare("vt")==0)
		{
			uv v(atof(s[1].c_str()),atof(s[2].c_str()));
			UV.push_back(v);
		}
		else if(s[0].compare("f")==0)
		{
			tri vs(atoi(s[1].substr(0,s[1].find("/")).c_str())-1,
					atoi(s[2].substr(0,s[2].find("/")).c_str())-1,
					atoi(s[3].substr(0,s[3].find("/")).c_str())-1,
					atoi(s[1].substr(s[1].find("/")+1,s[1].size()-s[1].find("/")).c_str())-1,
					atoi(s[2].substr(s[2].find("/")+1,s[2].size()-s[2].find("/")).c_str())-1,
					atoi(s[3].substr(s[3].find("/")+1,s[3].size()-s[3].find("/")).c_str())-1,
					currentMTL);
			triangles.push_back(vs);
		}
		else if(s[0].compare("mtllib")==0)
		{
			mtls=load_mtl(file + s[1]);
		}
		else if(s[0].compare("usemtl")==0)
		{
			currentMTL=s[1];
		}
	}
	i = glGenLists (1);
	glNewList (i, GL_COMPILE);
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	vert norm;
	for(vector<tri>::iterator v=triangles.begin();v!=triangles.end();v++)
	{
		glBindTexture(GL_TEXTURE_2D,mtls[(*v).my_mtl].tex);
		normal(vertices[(*v)[0]],vertices[(*v)[1]],vertices[(*v)[2]],&norm);
		glBegin(GL_TRIANGLES);
			glNormal3f(norm.x,norm.y,norm.z);
			glTexCoord2f(UV[(*v)[3]].u,UV[(*v)[3]].v);
			glVertex3f(vertices[(*v)[0]].x,vertices[(*v)[0]].y,vertices[(*v)[0]].z);
			glTexCoord2f(UV[(*v)[4]].u,UV[(*v)[4]].v);
			glVertex3f(vertices[(*v)[1]].x,vertices[(*v)[1]].y,vertices[(*v)[1]].z);
			glTexCoord2f(UV[(*v)[5]].u,UV[(*v)[5]].v);
			glVertex3f(vertices[(*v)[2]].x,vertices[(*v)[2]].y,vertices[(*v)[2]].z);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glEndList();
	return i;
}