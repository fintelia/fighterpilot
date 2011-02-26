
class missile: public selfControlledObject
{
public:
	float life;//life in seconds
	int target;
	Angle difAng;
	Angle lastAng;
	Vec3f velocity;
	Vec3f accel;
	int displayList;
	int owner;

	missile(missileType Type, teamNum Team,Vec3f sPos,Vec3f Vel, int dispList, int owner):selfControlledObject(sPos, Quat4f(), Type), life(15.0), target(0), difAng(0), lastAng(0), velocity(Vel), accel(Vel.normalize()*0.01), displayList(dispList){}

	void findTarget();
	void update(double time, double ms);
};