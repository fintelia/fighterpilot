
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

	//const Vec3f			startPos;
	//const Quat4f		startRot;
	const objId			id;

	Vec3f				position,
						lastPosition;
	Quat4f				rotation,
						lastRotation;
	bool				dead,
						awaitingDelete;
	meshInstancePtr		meshInstance;
//	physicsInstancePtr	physicsInstance;
	cameraPtr			firstPerson;
	cameraPtr			thirdPerson;

	object(objectType Type, int Team=NEUTRAL): type(Type), team(Team),  id(++currentId), dead(false), awaitingDelete(false),firstPerson(nullptr),thirdPerson(nullptr){}
	virtual void init(){}
	virtual ~object(){}
	virtual void draw(){}

	virtual void updateSimulation(double time, double ms) {}
	virtual void updateFrame(float interpolation) const {}

	virtual void loseHealth(float healthLoss){}
}; 

//class controlledObject: public object
//{
//private:
//	bool ownsControl;
//
//protected:
//	objectController* control;
//	controlledObject(Vec3f sPos, Quat4f sRot, objectType Type, objectController* c): object(sPos, sRot, Type), ownsControl(false), control(c)
//	{
//		if(control != NULL)
//		{
//			control->objectNum(id);
//		}
//	}
//	controlledObject(Vec3f sPos, Quat4f sRot, objectType Type, controlType c): object(sPos, sRot, Type), ownsControl(true), control(NULL)
//	{
//		if(c == CONTROL_HUMAN)
//		{
//			control = new humanControl(id);
//		}
//		else if(c == CONTROL_COMPUTER)
//		{
//			control = new AIcontrol(id);
//		}
//	}
//	~controlledObject()
//	{
//		if(ownsControl && control != NULL)
//			delete control;
//		else if(control != NULL)
//			control->objectNum(0);
//	}
//
//public:
//	void setController(objectController* c)
//	{
//		if(ownsControl && control != NULL)
//			delete control;
//
//		control = c;
//		ownsControl = false;
//
//		if(control != NULL)
//		{
//			control->objectNum(id);
//		}
//	}
//};
//
//class selfControlledObject: public object
//{
//public:
//	selfControlledObject(Vec3f sPos, Quat4f sRot, objectType Type): object(sPos, sRot, Type){}
//};
//
//class staticObject: public object
//{
//public:
//	staticObject(Vec3f sPos, Quat4f sRot, objectType Type): object(sPos, sRot, Type){}
//};
