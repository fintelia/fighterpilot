

class bullet
{
public:

	static const float bulletSpeed;

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
//	texturedVertex3D* vertices;
//	unsigned int totalVertices; // total number of vertices allocated

//	GraphicsManager::vertexBuffer* VBO;

public:
	vector<bullet> bullets;

	friend class Game;

	bulletCloud();

	void addBullet(Vec3f pos,Vec3f vel,int Owner, double StartTime);
	void addBullet(Vec3f pos,Vec3f vel,int Owner);

	void updateSimulation(double time, double ms);
	void updateFrame(float interpolation) const;
};
