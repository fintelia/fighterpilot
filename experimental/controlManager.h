

const int CON_CLIMB		= 0;
const int CON_DIVE		= 1;
const int CON_RIGHT		= 2;
const int CON_LEFT		= 3;
const int CON_ACCEL		= 4;
const int CON_BRAKE		= 5;
const int CON_SHOOT		= 6;
const int CON_MISSILE	= 7;
const int CON_BOMB		= 8;

//class controller
//{
//public:
//	int xboxControllerNum;
//	struct mapping{
//		enum Type{NONE, KEY, BUTTON,AXIS,AXIS_POSITIVE,AXIS_NEGATIVE}type;
//		int m;
//
//		float operator() (int controllerNum);
//		mapping(int M, Type t=KEY): type(t), m(M){}
//		mapping():type(NONE),m(0){}
//	};
//	map<int,mapping> mappings;
//
////	enum Type{NONE,KEYBOARD,XBOX_CONTROLLER}type;
//
//	controller():xboxControllerNum(-1){}
//	void mapControl(int control, int M, mapping::Type t = mapping::KEY);
//	virtual float operator[] (int control);
//};


//
//class keyboardController: public controller
//{
//private:
//	unsigned char virtualKeys[9];
//
//
//public:
//	KeyboardController(int player);
//	float operator[] (int control);
//};
//
//#ifdef USING_XINPUT
//class xboxController: public controller
//{
//
//public:
//	XboxController(int player);
//	int getControllerNum(){return mControllerNum;}
//	float operator[] (int control);
//};
//#endif

//class ControlManager
//{
//private:
//	map<int,shared_ptr<controller>> controllers;
//	int numControllers;
//
//	ControlManager():numControllers(0){}
//
//public:
//	static ControlManager& getInstance();
//	int addController(shared_ptr<controller> c);
//	float getControl(int controllerNum, int control);
//};
//extern ControlManager& controlManager;