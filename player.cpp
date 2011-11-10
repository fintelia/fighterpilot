

#include "game.h"
#include <Windows.h>
#include <Xinput.h>
int humanControl::TotalPlayers=1;

playerControls::playerControls(int p)//default controls for player p
{
	#ifdef USING_XINPUT
		if(input->getXboxController(p-1).connected)
		{
			c[CON_CLIMB]=nControl		(XINPUT_THUMB_LY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::NEGATIVE);
			c[CON_DIVE]=nControl		(XINPUT_THUMB_LY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
			c[CON_LEFT]=nControl		(XINPUT_THUMB_LX +	XBOX_GAMEPAD_OFFSET[p-1], nControl::NEGATIVE);
			c[CON_RIGHT]=nControl		(XINPUT_THUMB_LX +	XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
			c[CON_ACCEL]=nControl		(XINPUT_THUMB_RY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
			c[CON_BRAKE]=nControl		(XINPUT_THUMB_RY +	XBOX_GAMEPAD_OFFSET[p-1], nControl::NEGATIVE);
			c[CON_SHOOT]=nControl		(XINPUT_RIGHT_TRIGGER + XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
			c[CON_MISSILE]=nControl		(XINPUT_LEFT_TRIGGER + XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
			c[CON_BOMB]=nControl		(XINPUT_LEFT_SHOULDER + XBOX_GAMEPAD_OFFSET[p-1], nControl::POSITIVE);
		}
		else
	#endif
	if(p==1)
	{
		c[CON_CLIMB]=nControl(VK_DOWN);
		c[CON_DIVE]=nControl(VK_UP);
		c[CON_LEFT]=nControl(VK_LEFT);
		c[CON_RIGHT]=nControl(VK_RIGHT);
		c[CON_ACCEL]=nControl(VK_NUMPAD5);
		c[CON_BRAKE]=nControl(VK_NUMPAD2);
		c[CON_SHOOT]=nControl(VK_NUMPAD0);
		c[CON_MISSILE]=nControl(VK_NUMPAD9);
		c[CON_BOMB]=nControl(VK_NUMPAD8);
	}
	else if(p==2)
	{
		c[CON_CLIMB]=nControl(0x57);	//U
		c[CON_DIVE]=nControl(0x53);		//J
		c[CON_LEFT]=nControl(0x41);		//A
		c[CON_RIGHT]=nControl(0x44);	//D
		c[CON_ACCEL]=nControl(0x55);	//W
		c[CON_BRAKE]=nControl(0x4A);	//S
		c[CON_SHOOT]=nControl(0x20);	//SPACE
		c[CON_MISSILE]=nControl(0x42);	//B
		c[CON_BOMB]=nControl(0x56);		//V
	}
}
nControl playerControls::getCotrol(int con)
{

	if(con>=0 && con<8)
		return c[con];
	else
	{
		return nControl();
	}
}
void playerControls::setControl(int con,nControl k)
{
	if(con>=0 && con<8)
		c[con]=k;
}
float playerControls::operator[](int index) {
	return c[index]();
}

object* objectController::getObject()
{
	return world[mObjectNum].get();
}

void AIcontrol::update()
{
	if(getObject()->type & PLANE)
	{	
		// ...
	}
}



keyboardControls::keyboardControls(int player)
{
	if(player == 0)
	{
		virtualKeys[CON_CLIMB]		= VK_DOWN;
		virtualKeys[CON_DIVE]		= VK_UP;
		virtualKeys[CON_LEFT]		= VK_LEFT;
		virtualKeys[CON_RIGHT]		= VK_RIGHT;
		virtualKeys[CON_ACCEL]		= VK_NUMPAD5;
		virtualKeys[CON_BRAKE]		= VK_NUMPAD2;
		virtualKeys[CON_SHOOT]		= VK_NUMPAD0;
		virtualKeys[CON_MISSILE]	= VK_NUMPAD9;
		virtualKeys[CON_BOMB]		= VK_NUMPAD8;
	}
	else if(player == 2)
	{
		virtualKeys[CON_CLIMB]		= 0x57;		//U
		virtualKeys[CON_DIVE]		= 0x53;		//J
		virtualKeys[CON_LEFT]		= 0x41;		//A
		virtualKeys[CON_RIGHT]		= 0x44;		//D
		virtualKeys[CON_ACCEL]		= 0x55;		//W
		virtualKeys[CON_BRAKE]		= 0x4A;		//S
		virtualKeys[CON_SHOOT]		= 0x20;		//SPACE
		virtualKeys[CON_MISSILE]	= 0x42;		//B
		virtualKeys[CON_BOMB]		= 0x56;		//V
	}
	else
	{
		debugBreak();
		for(int i=0;i<9;i++)
			virtualKeys[i] = 0;

	}
}
#ifdef USING_XINPUT
float keyboardControls::operator[] (int control)
{
	if(control >= 0 && control < 9)
		return input->getKey(virtualKeys[control]) ? 0 : 1.0;
	return 0;
}

xboxControls::xboxControls(int player): mControllerNum(player)
{
	mappings[CON_CLIMB]		= mapping(XINPUT_THUMB_LY,mapping::AXIS_POSITIVE);
	mappings[CON_DIVE]		= mapping(XINPUT_THUMB_LY,mapping::AXIS_NEGATIVE);
	mappings[CON_LEFT]		= mapping(XINPUT_THUMB_LX,mapping::AXIS_POSITIVE);
	mappings[CON_RIGHT]		= mapping(XINPUT_THUMB_LX,mapping::AXIS_NEGATIVE);
	mappings[CON_ACCEL]		= mapping(XINPUT_THUMB_RY,mapping::AXIS_POSITIVE);
	mappings[CON_BRAKE]		= mapping(XINPUT_THUMB_RY,mapping::AXIS_NEGATIVE);
	mappings[CON_SHOOT]		= mapping(XINPUT_RIGHT_TRIGGER,mapping::AXIS);
	mappings[CON_MISSILE]	= mapping(XINPUT_LEFT_TRIGGER,mapping::AXIS);
	mappings[CON_BOMB]		= mapping(XINPUT_LEFT_SHOULDER,mapping::BUTTON);
}
float xboxControls::mapping::operator() (int controllerNum)
{
	if(type == BUTTON)
	{
		return (input->getXboxController(controllerNum).state->Gamepad.wButtons & m != 0) ? 1.0 : 0.0;
	}
	else if(type == AXIS)
	{
		if(m == XINPUT_LEFT_TRIGGER)		return 1.0/255 * input->getXboxController(controllerNum).state->Gamepad.bLeftTrigger;
		if(m == XINPUT_RIGHT_TRIGGER)		return 1.0/255 * input->getXboxController(controllerNum).state->Gamepad.bRightTrigger;
		if(m == XINPUT_THUMB_LX)			return 1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbLX;
		if(m == XINPUT_THUMB_LY)			return 1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbLY;
		if(m == XINPUT_THUMB_RX)			return 1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbRX;
		if(m == XINPUT_THUMB_RY)			return 1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbRY;
	}
	else if(type == AXIS_POSITIVE)
	{
		if(m == XINPUT_THUMB_LX)			return max(0.0,  1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbLX);
		if(m == XINPUT_THUMB_LY)			return max(0.0,  1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbLY);
		if(m == XINPUT_THUMB_RX)			return max(0.0,  1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbRX);
		if(m == XINPUT_THUMB_RY)			return max(0.0,  1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbRY);
	}
	else if(type == AXIS_NEGATIVE)
	{
		if(m == XINPUT_THUMB_LX)			return min(0.0,  -1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbLX);
		if(m == XINPUT_THUMB_LY)			return min(0.0,  -1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbLY);
		if(m == XINPUT_THUMB_RX)			return min(0.0,  -1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbRX);
		if(m == XINPUT_THUMB_RY)			return min(0.0,  -1.0/32767 * input->getXboxController(controllerNum).state->Gamepad.sThumbRY);
	}

	debugBreak();
	return 0.0;
}
float xboxControls::operator[] (int control)
{
	if(mappings.find(control) != mappings.end())
		return mappings[control](mControllerNum);
	
	return 0.0;
}
#endif