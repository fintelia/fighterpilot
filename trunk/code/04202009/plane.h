
using namespace std;
//const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;

void spawn();
void die();


class planeBase:public entity
{
private:
public:
	void updatePos();
	virtual void Accelerate();
	virtual void Brake();
	virtual void Turn_Right();
	virtual void Turn_Left();
	virtual void Climb();
	virtual void Dive();
	virtual void Level();
	virtual void die();
	virtual void Shoot();
	virtual void ShootMissile();

	//float x;
	//float y;
	//float z;
	float angle;
	float acceleration;
	float climb; 
	float turn; // from -50 to 50
	Vec3f normal;
	bool hit;
	int player;
	int respawn;
	bool controled;
	int maneuver;
	int cooldown;
	int mCooldown;
	float altitude;
	bool hitGround;
	Terrain *terrain;
	vector<bullet> *bullets;
	vector<missile> *missiles;
	vector<spark> *particles;
	vector<planeBase*> *planes;

	virtual bool Update()=0;
	virtual void spawn()=0;
	planeBase(){}
};

class plane: public planeBase
{
private:
public:
	bool Update();
	void spawn();
	void die();
	plane(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<planeBase*> *p);
	plane();
};

class AIplane:public planeBase
{
public:
	int nextShot;//determines how long until the plane can fire again
	int rBullets;//remaining bullits
	
	AIplane();
	AIplane(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<planeBase*> *p);

	bool Update();
	void spawn();
	void calcMove();
};