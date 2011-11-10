

class bullet
{
public:
	double life;
	double startTime;
	Vec3f startPos;
	Vec3f velocity;//in m/s
	int owner;
	bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime);
	bullet(Vec3f pos,Vec3f vel,int Owner);
};

class bulletCloud: public object
{
private:
	texturedVertex3D* vertices;
	unsigned int totalVertices; // total number of vertices allocated

	unsigned int VBO;

public:
	vector<bullet> bullets;

	friend class Game;

	bulletCloud(): object(Vec3f(), Quat4f(), BULLET_CLOUD), vertices(NULL), totalVertices(0), VBO(0){}
	~bulletCloud(){if(vertices) delete[] vertices;}

	void init();
	void draw();

	void addBullet(Vec3f pos,Vec3f vel,int Owner, double StartTime);
	void addBullet(Vec3f pos,Vec3f vel,int Owner);

	void updateSimulation(double time, double ms);
	void updateFrame(float interpolation) const;
};
