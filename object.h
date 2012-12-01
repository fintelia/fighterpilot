
typedef unsigned int objId;

struct camera
{
	Vec3f eye;
	Vec3f center;
	Vec3f up;
};
typedef camera* cameraPtr;

class object
{
private:
	static objId currentId;

public:
	friend class objectList;

	const objectType	type;
	int					team;

	const objId			id;

	Vec3f				position,
						lastPosition;
	Quat4f				rotation,
						lastRotation;
	bool				dead,
						awaitingDelete;
	meshInstancePtr		meshInstance;

	object(objectType Type, int Team=NEUTRAL): type(Type), team(Team),  id(++currentId), dead(false), awaitingDelete(false){}
	virtual void init(){}
	virtual ~object(){}
	virtual void draw(){}

	virtual void updateSimulation(double time, double ms) {}
	virtual void updateFrame(float interpolation) const {}

	virtual void loseHealth(float healthLoss){}
};