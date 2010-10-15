
class bullet
{
public:
	double life;
	double startTime;
	Vec3f startPos;
	Vec3f velocity;//in units per second
	int owner;
	bullet(Vec3f pos,Vec3f vel,int Owner, double StartTime=gameTime()): life(1000), startTime(StartTime), startPos(pos), velocity(vel.normalize()*5000), owner(Owner){}
	bool update(float length)
	{
		return life+startTime-gameTime() >= 0;
	}
};
