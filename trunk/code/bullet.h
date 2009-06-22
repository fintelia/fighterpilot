
const float speed=60;
class bullet
{
public:
	int life;
	float x,y,z;
	float vx, vy, vz;
	int owner;
	bullet()
	{
		x=0;
		y=0;
		z=0;
		vx=0;
		vy=0;
		vz=0;
		life=100;
		cout << "error" << endl;
	}
	bullet(float _x,float _y,float _z,float _vx,float _vy,float _vz,int _owner)
	{
		x=_x;
		y=_y;
		z=_z;
		vx=_vx;
		vy=_vy;
		vz=_vz;
		life=100;
		owner=_owner;
	}
	bool update(float length)
	{
		x+=vx*length*speed;
		y+=vy*length*speed;
		z+=vz*length*speed;
		return --life >= 0 ? true : false;
	}
};
