
using namespace std;
//const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;

void spawn();
void die();


class planeBase:public entity
{
private:
public:
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
	Terrain *terrain;
	vector<bullet> *bullets;
	vector<missile> *missiles;
	vector<spark> *particles;
	vector<planeBase*> *planes;
// displayed stats
	
	virtual void Accelerate()=0;
	virtual void Brake()=0;
	virtual void Turn_Right()=0;
	virtual void Turn_Left()=0;
	virtual void Climb()=0;
	virtual void Dive()=0;
	virtual void Level()=0;
	virtual void Shoot()=0;
	virtual void ShootMissile()=0;
	virtual bool Update()=0;
	virtual void spawn()=0;
	//virtual planeBase(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<plane> *p);
	planeBase(){}
	virtual void die()=0;
};

class plane: public planeBase
{
private:
public:
	void Accelerate();

	void Brake();
	void Turn_Right();
	void Turn_Left();
	void Climb();
	void Dive();
	void Level();
	void Shoot();
	void ShootMissile();
	bool Update();
	void spawn();
	plane(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<planeBase*> *p);
	plane();
	
	void die();
	//void Draw()//not practical
};

class AIplane:public planeBase
{
public:
	int nextShot;//determines how long until the plane can fire again
	int rBullets;//remaining bullits
	AIplane();
	AIplane(int _player,Terrain *T,vector<missile> *m,vector<bullet> *b,vector<spark> *s,vector<planeBase*> *p);
	void Accelerate();
	void Brake();
	void Turn_Right();
	void Turn_Left();
	void Climb();
	void Dive();
	void Level();
	void Shoot();
	void ShootMissile();
	bool Update();
	void spawn();
	void die();
	void calcMove();
};