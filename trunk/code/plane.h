
using namespace std;
//const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;

void spawn();
void die();


class planeBase:public entity
{
private:
public:
	void updatePos(int value);
	virtual void Accelerate(float value);
	virtual void Brake(float value);
	virtual void Turn_Right(float value);
	virtual void Turn_Left(float value);
	virtual void Climb(float value);
	virtual void Dive(float value);
	virtual void Level(float value);
	virtual void Shoot();
	virtual void ShootMissile();
	virtual void die();
	//float x;
	//float y;
	//float z;
	float angle;
	float acceleration;
	float climb; 
	float turn; // from -50 to 50
	Vec3f normal;
	Vec3f forward;
	int player;
	int team;
	int respawn;
	int maneuver;
	int cooldown;
	int mCooldown;
	float altitude;
	bool hitGround;
	bool controled;
	//vector<bullet> *bullets;
	//vector<missile> *missiles;
	//vector<spark> *particles;
	//map<int,planeBase*> *planes;

	virtual bool Update(float value)=0;
	virtual void spawn()=0;
	planeBase(){}
};

class plane: public planeBase
{
private:
public:
	bool Update(float value);
	void spawn();
	void die();
	plane(int _player,int _team);
	plane();
};

class AIplane:public planeBase
{
public:
	int nextShot;//determines how long until the plane can fire again
	int rBullets;//remaining bullits
	
	AIplane();
	AIplane(int _player,int _team);

	bool Update(float value);
	void spawn();
	void calcMove(int value);
};