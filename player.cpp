

#include "game.h"
#include <Windows.h>
#include <Xinput.h>
//
//playerControls::playerControls(int p)//default controls for player p
//{
//	#ifdef USING_XINPUT
//		if(input.getXboxController(p-1).connected)
//		{
//			c[CON_CLIMB]=nControl		(XINPUT_THUMB_LY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::NEGATIVE);
//			c[CON_DIVE]=nControl		(XINPUT_THUMB_LY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
//			c[CON_LEFT]=nControl		(XINPUT_THUMB_LX +	XBOX_GAMEPAD_OFFSET[p-1], nControl::NEGATIVE);
//			c[CON_RIGHT]=nControl		(XINPUT_THUMB_LX +	XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
//			c[CON_ACCEL]=nControl		(XINPUT_THUMB_RY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
//			c[CON_BRAKE]=nControl		(XINPUT_THUMB_RY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::NEGATIVE);
//			c[CON_SHOOT]=nControl		(XINPUT_RIGHT_TRIGGER + XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
//			c[CON_MISSILE]=nControl		(XINPUT_LEFT_TRIGGER + XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
//			c[CON_BOMB]=nControl		(XINPUT_LEFT_SHOULDER + XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
//		}
//		else
//	#endif
//	if(p==1)
//	{
//		c[CON_CLIMB]=nControl(VK_DOWN);
//		c[CON_DIVE]=nControl(VK_UP);
//		c[CON_LEFT]=nControl(VK_LEFT);
//		c[CON_RIGHT]=nControl(VK_RIGHT);
//		c[CON_ACCEL]=nControl(VK_NUMPAD5);
//		c[CON_BRAKE]=nControl(VK_NUMPAD2);
//		c[CON_SHOOT]=nControl(VK_NUMPAD0);
//		c[CON_MISSILE]=nControl(VK_NUMPAD9);
//		c[CON_BOMB]=nControl(VK_NUMPAD8);
//	}
//	else if(p==2)
//	{
//		c[CON_CLIMB]=nControl(0x57);	//U
//		c[CON_DIVE]=nControl(0x53);		//J
//		c[CON_LEFT]=nControl(0x41);		//A
//		c[CON_RIGHT]=nControl(0x44);	//D
//		c[CON_ACCEL]=nControl(0x55);	//W
//		c[CON_BRAKE]=nControl(0x4A);	//S
//		c[CON_SHOOT]=nControl(0x20);	//SPACE
//		c[CON_MISSILE]=nControl(0x42);	//B
//		c[CON_BOMB]=nControl(0x56);		//V
//	}
//}
//nControl playerControls::getCotrol(int con)
//{
//
//	if(con>=0 && con<8)
//		return c[con];
//	else
//	{
//		return nControl();
//	}
//}
//void playerControls::setControl(int con,nControl k)
//{
//	if(con>=0 && con<8)
//		c[con]=k;
//}
//float playerControls::operator[](int index) {
//	return c[index]();
//}
//


//humanPlayer::controlMapping::controlMapping(int playerNum)
//{
//#ifdef USING_XINPUT
//	if(input.getXboxController(playerNum).connected)
//	{
//		type = XBOX_CONTROLLER;
//		mControllerNum = playerNum;
//		mappings[CON_CLIMB]		= mapping(XINPUT_THUMB_LY,mapping::AXIS_POSITIVE);
//		mappings[CON_DIVE]		= mapping(XINPUT_THUMB_LY,mapping::AXIS_NEGATIVE);
//		mappings[CON_LEFT]		= mapping(XINPUT_THUMB_LX,mapping::AXIS_POSITIVE);
//		mappings[CON_RIGHT]		= mapping(XINPUT_THUMB_LX,mapping::AXIS_NEGATIVE);
//		mappings[CON_ACCEL]		= mapping(XINPUT_THUMB_RY,mapping::AXIS_POSITIVE);
//		mappings[CON_BRAKE]		= mapping(XINPUT_THUMB_RY,mapping::AXIS_NEGATIVE);
//		mappings[CON_SHOOT]		= mapping(XINPUT_RIGHT_TRIGGER,mapping::AXIS);
//		mappings[CON_MISSILE]	= mapping(XINPUT_LEFT_TRIGGER,mapping::AXIS);
//		mappings[CON_BOMB]		= mapping(XINPUT_LEFT_SHOULDER,mapping::BUTTON);
//	}else
//#endif
//	if(playerNum == 0)
//	{
//		type = KEYBOARD;
//		mControllerNum = 0;
//		
//		mappings[CON_CLIMB]		= mapping(VK_DOWN);
//		mappings[CON_DIVE]		= mapping(VK_UP);
//		mappings[CON_LEFT]		= mapping(VK_LEFT);
//		mappings[CON_RIGHT]		= mapping(VK_RIGHT);
//		mappings[CON_ACCEL]		= mapping(VK_NUMPAD5);
//		mappings[CON_BRAKE]		= mapping(VK_NUMPAD2);
//		mappings[CON_SHOOT]		= mapping(VK_NUMPAD0);
//		mappings[CON_MISSILE]	= mapping(VK_NUMPAD9);
//		mappings[CON_BOMB]		= mapping(VK_NUMPAD8);
//	}
//	else if(playerNum == 1)
//	{
//		type = KEYBOARD;
//		mControllerNum = 0;
//		mappings[CON_CLIMB]		= mapping(0x57);  //U
//		mappings[CON_DIVE]		= mapping(0x53);  //J
//		mappings[CON_LEFT]		= mapping(0x41);  //A
//		mappings[CON_RIGHT]		= mapping(0x44);  //D
//		mappings[CON_ACCEL]		= mapping(0x55);  //W
//		mappings[CON_BRAKE]		= mapping(0x4A);  //S
//		mappings[CON_SHOOT]		= mapping(0x20);  //SPACE
//		mappings[CON_MISSILE]	= mapping(0x42);  //B
//		mappings[CON_BOMB]		= mapping(0x56);  //V
//	}
//	else
//	{
//		debugBreak();
//		type = NONE;
//		mControllerNum = 0;
//		for(int i=0;i<9;i++)
//			mappings[i] = mapping();
//
//	}
//}
float humanPlayer::controlMapping::operator() ()
{
	if(type == KEY)
	{
		return input.getKey(m) ? 1.0 : 0.0;
	}
#ifdef USING_XINPUT
	else if(type == BUTTON)
	{
		return input.getXboxController(controllerNum).getButton(m) ? 1.0 : 0.0;
	}
	else if(type == AXIS)
	{
		return input.getXboxController(controllerNum).getAxis(m);
	}
	else if(type == AXIS_POSITIVE)
	{
		return max(0.0, input.getXboxController(controllerNum).getAxis(m));
	}
	else if(type == AXIS_NEGATIVE)
	{
		return -min(0.0, input.getXboxController(controllerNum).getAxis(m));
	}
#endif

	debugBreak();
	return 0.0;
}

object* player::getObject()
{
	return world[mObjectNum].get();
}
////
//void AIcontrol::update()
//{
//	if(getObject()->type & PLANE)
//	{	
//		// ...
//	}
//}

humanPlayer::humanPlayer(int playerNumber, int objectId): player(PLAYER_HUMAN, objectId), mPlayerNum(playerNumber), firstPersonView(false)
{
#ifdef USING_XINPUT
	if(input.getXboxController(mPlayerNum).connected)
	{
		controls[CON_CLIMB]		= controlMapping(XINPUT_THUMB_LY,controlMapping::AXIS_NEGATIVE,mPlayerNum);
		controls[CON_DIVE]		= controlMapping(XINPUT_THUMB_LY,controlMapping::AXIS_POSITIVE,mPlayerNum);

		controls[CON_LEFT]		= controlMapping(XINPUT_THUMB_LX,controlMapping::AXIS_NEGATIVE,mPlayerNum);
		controls[CON_RIGHT]		= controlMapping(XINPUT_THUMB_LX,controlMapping::AXIS_POSITIVE,mPlayerNum);

		controls[CON_ACCEL]		= controlMapping(XINPUT_THUMB_RY,controlMapping::AXIS_POSITIVE,mPlayerNum);
		controls[CON_BRAKE]		= controlMapping(XINPUT_THUMB_RY,controlMapping::AXIS_NEGATIVE,mPlayerNum);

		controls[CON_SHOOT]		= controlMapping(XINPUT_RIGHT_TRIGGER,controlMapping::AXIS,mPlayerNum);
		controls[CON_MISSILE]	= controlMapping(XINPUT_LEFT_TRIGGER,controlMapping::AXIS,mPlayerNum);
		controls[CON_BOMB]		= controlMapping(XINPUT_LEFT_SHOULDER,controlMapping::BUTTON,mPlayerNum);
	}else
#endif
	if(mPlayerNum == 0)
	{
		controls[CON_CLIMB]		= controlMapping(VK_DOWN);
		controls[CON_DIVE]		= controlMapping(VK_UP);
		controls[CON_LEFT]		= controlMapping(VK_LEFT);
		controls[CON_RIGHT]		= controlMapping(VK_RIGHT);
		controls[CON_ACCEL]		= controlMapping(VK_NUMPAD5);
		controls[CON_BRAKE]		= controlMapping(VK_NUMPAD2);
		controls[CON_SHOOT]		= controlMapping(VK_NUMPAD0);
		controls[CON_MISSILE]	= controlMapping(VK_NUMPAD9);
		controls[CON_BOMB]		= controlMapping(VK_NUMPAD8);
	}
	else if(mPlayerNum == 1)
	{
		controls[CON_CLIMB]		= controlMapping(0x57);  //U
		controls[CON_DIVE]		= controlMapping(0x53);  //J
		controls[CON_LEFT]		= controlMapping(0x41);  //A
		controls[CON_RIGHT]		= controlMapping(0x44);  //D
		controls[CON_ACCEL]		= controlMapping(0x55);  //W
		controls[CON_BRAKE]		= controlMapping(0x4A);  //S
		controls[CON_SHOOT]		= controlMapping(0x20);  //SPACE
		controls[CON_MISSILE]	= controlMapping(0x42);  //B
		controls[CON_BOMB]		= controlMapping(0x56);  //V
	}
	else
	{
		debugBreak();
		for(int i=0;i<9;i++)
			controls[i] = controlMapping();
	}
	object* o = getObject();
	o->firstPerson = &firstPerson;
	o->thirdPerson = &thirdPerson;
}
humanPlayer::~humanPlayer()
{
	object* o = getObject();
	if(o != nullptr)
	{
		o->firstPerson = nullptr;
		o->thirdPerson = nullptr;
	}
}
void humanPlayer::update()
{
	object* o = getObject();
	if(o != nullptr && o->type & PLANE)
	{
		nPlane* p = (nPlane*)o;

		p->controls.accelerate	= controls[CON_ACCEL]();
		p->controls.brake		= controls[CON_BRAKE]();
		p->controls.climb		= controls[CON_CLIMB]();
		p->controls.dive		= controls[CON_DIVE]();
		p->controls.left		= controls[CON_LEFT]();
		p->controls.right		= controls[CON_RIGHT]();
		p->controls.shoot1		= controls[CON_SHOOT]() > 0.75;
		p->controls.shoot2		= controls[CON_MISSILE]() > 0.75;
		p->controls.shoot3		= controls[CON_BOMB]() > 0.75;
	}
}
const camera& humanPlayer::getCamera(bool forceThirdPerson) const
{
	return (firstPersonView && !forceThirdPerson) ? firstPerson : thirdPerson;
}
void AIplayer::update()
{
	object* o = getObject();

	if(o != nullptr && o->type & PLANE)
	{
		nPlane* p = (nPlane*)o;

		p->controls.accelerate	= 0.0;
		p->controls.brake		= 0.0;
		p->controls.climb		= 0.0;
		p->controls.dive		= 0.0;
		p->controls.left		= 0.0;
		p->controls.right		= 0.0;
		p->controls.shoot1		= false;
		p->controls.shoot2		= false;
		p->controls.shoot3		= false;
	}
}
PlayerManager& PlayerManager::getInstance()
{
	static PlayerManager* pInstance = new PlayerManager();
	return *pInstance;
}

void PlayerManager::addHumanPlayer(int objectId)
{
	shared_ptr<humanPlayer> p(new humanPlayer(humanPlayers.size(), objectId));
	humanPlayers.push_back(p);
}
void PlayerManager::addAIplayer(int objectId)
{
	shared_ptr<AIplayer> p(new AIplayer(objectId));
	aiPlayers.push_back(p);
}
void PlayerManager::resetPlayers()
{
	humanPlayers.clear();
	aiPlayers.clear();
}

void PlayerManager::update()
{
	for(auto i = humanPlayers.begin(); i != humanPlayers.end(); i++)
	{
		(*i)->update();
	}
	for(auto i = aiPlayers.begin(); i != aiPlayers.end(); i++)
	{
		(*i)->update();
	}
}

shared_ptr<humanPlayer> PlayerManager::operator[] (unsigned int p)
{
	if(p < humanPlayers.size())
		return humanPlayers[p];

	return shared_ptr<humanPlayer>();
}