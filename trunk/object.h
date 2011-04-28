//
//class entity
//{
//public:
//	const int	id;
//	int			type;
//	int			team;
//	bool		dead;
//	Vec3f		pos;
//
//	virtual bool Update(float value)=0;
//	entity(int Id, int Type, int Team, Vec3f Pos=Vec3f(0,0,0)): id(Id), type(Type), team(Team), dead(false), pos(Pos){} 
//};

typedef unsigned int objId;

class object
{
private:
	static objId currentId;

public:
	friend class objectList;

	const Vec3f startPos;
	const Quat4f startRot;
	const objectType type;	
	const objId id;


	Vec3f position;
	Quat4f rotation;
	bool dead;
	int team;
	bool awaitingDelete;

	object(Vec3f sPos, Quat4f sRot, objectType Type): startPos(sPos), startRot(sRot), type(Type), id(++currentId), position(sPos), rotation(sRot), dead(false), team(NEUTRAL), awaitingDelete(false){}
	virtual void update(double time, double ms)=0;
	virtual ~object(){}
};

class controlledObject: public object
{
private:
	bool ownsControl;

protected:
	objectController* control;
	controlledObject(Vec3f sPos, Quat4f sRot, objectType Type, objectController* c): object(sPos, sRot, Type), control(c), ownsControl(false)
	{
		if(control != NULL)
		{
			control->objectNum(id);
		}
	}
	controlledObject(Vec3f sPos, Quat4f sRot, objectType Type, controlType c): object(sPos, sRot, Type), control(NULL), ownsControl(true)
	{
		if(c == CONTROL_HUMAN)
		{
			control = new humanControl(id);
		}
		else if(c == CONTROL_COMPUTER)
		{
			control = new AIcontrol(id);
		}
	}
	~controlledObject()
	{
		if(ownsControl && control != NULL)
			delete control;
		else if(control != NULL)
			control->objectNum(0);
	}

public:
	void setController(objectController* c)
	{
		if(ownsControl && control != NULL)
			delete control;

		control = c;
		ownsControl = false;

		if(control != NULL)
		{
			control->objectNum(id);
		}
	}
};

class selfControlledObject: public object
{
public:
	selfControlledObject(Vec3f sPos, Quat4f sRot, objectType Type): object(sPos, sRot, Type){}
};

class staticObject: public object
{
public:
	staticObject(Vec3f sPos, Quat4f sRot, objectType Type): object(sPos, sRot, Type){}
};