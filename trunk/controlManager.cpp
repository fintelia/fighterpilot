
#include "engine.h"
//
//controller::controller(int playerNum)
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
//float controller::mapping::operator() (int controllerNum)
//{
//	if(type == KEY)
//	{
//		return input.getKey(m) ? 1.0 : 0.0;
//	}
//#ifdef USING_XINPUT
//	else if(type == BUTTON)
//	{
//		return input.getXboxController(controllerNum).getButton(m) ? 1.0 : 0.0;
//	}
//	else if(type == AXIS)
//	{
//		return input.getXboxController(controllerNum).getAxis(m);
//	}
//	else if(type == AXIS_POSITIVE)
//	{
//		return max(0.0, input.getXboxController(controllerNum).getAxis(m));
//	}
//	else if(type == AXIS_NEGATIVE)
//	{
//		return min(0.0, -input.getXboxController(controllerNum).getAxis(m));
//	}
//#endif
//
//	debugBreak();
//	return 0.0;
//}
//float controller::operator[] (int control)
//{
//	if(mappings.find(control) != mappings.end())
//		return mappings[control](mControllerNum);
//	
//	return 0.0;
//}
//
//
//ControlManager& ControlManager::getInstance()
//{
//	static ControlManager* pInstance = new ControlManager();
//	return *pInstance;
//}
//
//int ControlManager::addController(shared_ptr<controller> c)
//{
//	controllers[numControllers] = c;
//	return numControllers++;
//}
//float ControlManager::getControl(int controllerNum, int control)
//{
//	auto c = controllers.find(controllerNum);
//	if(c != controllers.end())
//		return c->second->operator[](control);
//	else
//		return 0.0f;
//}