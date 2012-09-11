
class ship: public object
{
public:
	//bool dead; (from entity)
	float health;

	void updateFrame(float interpolation) const;
	//void initArmaments();
	void updateSimulation(double time, double ms);
	void die();
	void loseHealth(float healthLoss);
	ship(Vec3f sPos, Quat4f sRot, objectType Type, int Team);
};