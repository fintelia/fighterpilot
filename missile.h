
class missile: public entity
{
public:
	float life;//life in seconds
	int target;
	Angle difAng;
	Angle lastAng;
	Vec3f velocity;
	Vec3f accel;
	int displayList;
	missile(int Id, missileType Type, teamNum Team,Vec3f Pos,Vec3f Vel, int dispList):entity(Id,Type,Team,Pos), life(15.0), target(0), difAng(0), lastAng(0), velocity(Vel), accel(Vel.normalize()*0.01), displayList(dispList){}

	void findTarget();
	bool Update(float value);
};