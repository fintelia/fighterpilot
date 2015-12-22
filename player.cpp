
#include "engine.h"
#include "game.h"

float humanPlayer::controlMapping::operator() ()
{
	if(type == KEY)
	{
		return input.getKey(m) ? 1.0 : 0.0;
	}
#ifdef XINPUT
	else if(type == XBOX_BUTTON)
	{
		return input.getXboxController(controllerNum).getButton(m) ? 1.0 : 0.0;
	}
	else if(type == XBOX_AXIS)
	{
		return input.getXboxController(controllerNum).getAxis(m);
	}
	else if(type == XBOX_AXIS_POSITIVE)
	{
		return max(0.0, input.getXboxController(controllerNum).getAxis(m));
	}
	else if(type == XBOX_AXIS_NEGATIVE)
	{
		return -min(0.0, input.getXboxController(controllerNum).getAxis(m));
	}
#endif
#ifdef DIRECT_INPUT
	else if(type == DI_BUTTON)
	{
		if(input.getDirectInputController(controllerNum))
			return input.getDirectInputController(controllerNum)->getButton(m) ? 1.0 : 0.0;
		return 0.0;
	}
	else if(type == DI_AXIS)
	{
		if(input.getDirectInputController(controllerNum))
			return input.getDirectInputController(controllerNum)->getAxis(m);
		return 0.0;
	}
	else if(type == DI_AXIS_POSITIVE)
	{
		if(input.getDirectInputController(controllerNum))
			return max(0.0, input.getDirectInputController(controllerNum)->getAxis(m));
		return 0.0;
	}
	else if(type == DI_AXIS_NEGATIVE)
	{
		if(input.getDirectInputController(controllerNum))
			return -min(0.0, input.getDirectInputController(controllerNum)->getAxis(m));
		return 0.0;
	}
#endif
	debugBreak();
	return 0.0;
}

shared_ptr<plane> player::getObject()
{
	return dynamic_pointer_cast<plane>(world->getObjectById(mObjectNum));
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
#ifdef XINPUT
	xBoxControls[CON_CLIMB]		= controlMapping(XINPUT_THUMB_LY,controlMapping::XBOX_AXIS_NEGATIVE,mPlayerNum);
	xBoxControls[CON_DIVE]		= controlMapping(XINPUT_THUMB_LY,controlMapping::XBOX_AXIS_POSITIVE,mPlayerNum);

	xBoxControls[CON_LEFT]		= controlMapping(XINPUT_THUMB_LX,controlMapping::XBOX_AXIS_NEGATIVE,mPlayerNum);
	xBoxControls[CON_RIGHT]		= controlMapping(XINPUT_THUMB_LX,controlMapping::XBOX_AXIS_POSITIVE,mPlayerNum);

	xBoxControls[CON_ACCEL]		= controlMapping(XINPUT_THUMB_RY,controlMapping::XBOX_AXIS_NEGATIVE,mPlayerNum);
	xBoxControls[CON_BRAKE]		= controlMapping(XINPUT_THUMB_RY,controlMapping::XBOX_AXIS_POSITIVE,mPlayerNum);

	xBoxControls[CON_SHOOT]		= controlMapping(XINPUT_RIGHT_TRIGGER,controlMapping::XBOX_AXIS,mPlayerNum);
	xBoxControls[CON_MISSILE]	= controlMapping(XINPUT_LEFT_TRIGGER,controlMapping::XBOX_AXIS,mPlayerNum);
	xBoxControls[CON_BOMB]		= controlMapping(XINPUT_LEFT_SHOULDER,controlMapping::XBOX_BUTTON,mPlayerNum);
#endif

#ifdef DIRECT_INPUT
	directInputControls[CON_CLIMB]		= controlMapping(1,controlMapping::DI_AXIS_POSITIVE,mPlayerNum);
	directInputControls[CON_DIVE]		= controlMapping(1,controlMapping::DI_AXIS_NEGATIVE,mPlayerNum);

	directInputControls[CON_LEFT]		= controlMapping(0,controlMapping::DI_AXIS_NEGATIVE,mPlayerNum);
	directInputControls[CON_RIGHT]		= controlMapping(0,controlMapping::DI_AXIS_POSITIVE,mPlayerNum);

	directInputControls[CON_ACCEL]		= controlMapping(2,controlMapping::DI_AXIS_NEGATIVE,mPlayerNum);
	directInputControls[CON_BRAKE]		= controlMapping(2,controlMapping::DI_AXIS_POSITIVE,mPlayerNum);

	directInputControls[CON_SHOOT]		= controlMapping(1,controlMapping::DI_BUTTON,mPlayerNum);
	directInputControls[CON_MISSILE]	= controlMapping(0,controlMapping::DI_BUTTON,mPlayerNum);
	directInputControls[CON_BOMB]		= controlMapping(3,controlMapping::DI_BUTTON,mPlayerNum);
#endif
	if(mPlayerNum == 0)
	{
		controls[CON_CLIMB]		= controlMapping('S');
		controls[CON_DIVE]		= controlMapping('W');
		controls[CON_LEFT]		= controlMapping('A');
		controls[CON_RIGHT]		= controlMapping('D');
		controls[CON_ACCEL]		= controlMapping('U');
		controls[CON_BRAKE]		= controlMapping('J');
		controls[CON_SHOOT]		= controlMapping(VK_SHIFT);
		controls[CON_MISSILE]	= controlMapping(VK_SPACE);
		controls[CON_BOMB]		= controlMapping('F');
	}
	else if(mPlayerNum == 1)
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
	else
	{
		debugBreak();
		for(int i=0;i<9;i++)
			controls[i] = controlMapping();
	}
	setObject(objectId);
}
void humanPlayer::setObject(int objectId)
{
	mObjectNum = objectId;
	auto o = getObject();
	o->firstPerson = &firstPerson;
	o->thirdPerson = &thirdPerson;
}
humanPlayer::~humanPlayer()
{
	auto o = getObject();
	if(o)
	{
		o->firstPerson = nullptr;
		o->thirdPerson = nullptr;
	}
}
void humanPlayer::update()
{
	//setObject(3);

	auto maxControlVal = [this](int n) -> float
	{
#if defined(XINPUT) && defined(DIRECT_INPUT)
		return max(controls[n](),max(xBoxControls[n](),directInputControls[n]()));
#elif defined(XINPUT)
		return max(controls[n](),xBoxControls[n]());
#elif defined(DIRECT_INPUT)
		return max(controls[n](),directInputControls[n]());
#else
		return controls[n]();
#endif
	};

	auto p = getObject();
	if(p)
	{
		p->controls.accelerate	= lerp(p->controls.accelerate, maxControlVal(CON_ACCEL), 0.07);
		p->controls.brake		= lerp(p->controls.brake, maxControlVal(CON_BRAKE), 0.07);
		p->controls.climb		= lerp(p->controls.climb, maxControlVal(CON_CLIMB), 0.07);
		p->controls.dive		= lerp(p->controls.dive, maxControlVal(CON_DIVE), 0.07);
		p->controls.left		= lerp(p->controls.left, maxControlVal(CON_LEFT), 0.07);
		p->controls.right		= lerp(p->controls.right, maxControlVal(CON_RIGHT), 0.07);
		p->controls.shoot1		= maxControlVal(CON_SHOOT) > 0.75;
		p->controls.shoot2		= maxControlVal(CON_MISSILE) > 0.75;
		p->controls.shoot3		= maxControlVal(CON_BOMB) > 0.75;
	}
}
void humanPlayer::setVibrate(float amount)
{
#ifdef XINPUT
	input.setVibration(mPlayerNum, amount);
#endif
}
const camera& humanPlayer::getCamera(bool forceThirdPerson) const
{
	return (firstPersonView && !forceThirdPerson) ? firstPerson : thirdPerson;
}
AIplayer::AIplayer(int oNum): player(PLAYER_COMPUTER,oNum),missileCountDown(0.0),target(0),destination(0,0,0), state(STATE_NONE)
{
	auto p = getObject();
	if(p)
	{
		p->controlType = plane::CONTROL_TYPE_AI;
	}
}
void AIplayer::startPatrol()
{
	Vec2f p = random2<float>() * random<float>(world->bounds().radius) + world->bounds().center;
	float alt = random<float>(50.0,1000.0);
	destination = Vec3f(p.x,alt,p.y);
	state = STATE_PATROL;
}
void AIplayer::startHunt(int targetID)
{
	target = targetID;
	huntEndTime = world->time() + random<double>(1000,3000);
	state = STATE_HUNTING;
}
void AIplayer::flyTowardsPoint(shared_ptr<plane> p, Vec3f dest, float strength)
{
	strength = clamp(strength, -1.0, 1.0); //1 = fly towards point, -1 = fly away

	//Vec3f velocity = p->rotation * Vec3f(0,0,1);
	Vec3f destVector = dest - p->position;
	Vec3f destDirection = destVector.normalize();

	float diffAng = Angle(atan2(destDirection.x, destDirection.z) - p->direction).radians_plusMinusPI();
	float diffClimb = Angle(atan2(destVector.y, sqrt(destVector.x*destVector.x+destVector.z*destVector.z)) - p->climb).radians_plusMinusPI();

	p->direction += clamp(diffAng,1.2,-1.2) * world->time.length()/1000.0 * strength;
	p->climb += clamp(diffClimb,-0.7,0.7) * world->time.length()/1000.0 * strength;

	float rollError = (diffAng*0.5 + p->roll.radians_plusMinusPI()) / (PI/2);

	p->controls.climb		= 0.0;
	p->controls.dive		= 0.0;
	p->controls.left		= clamp(rollError*strength,0.0,1.0);
	p->controls.right		= clamp(-rollError*strength,0.0,1.0);
}
void AIplayer::update()
{
	shared_ptr<plane> p = getObject();
	if(!p)
	{
		debugBreak();
		return;
	}

	missileCountDown -= world->time.length();
	if(state == STATE_NONE)
	{
		startPatrol();
	}
	else if(state == STATE_PATROL)
	{
		//graphics->drawLine(o->position, destination);
		//Profiler.setOutput(lexical_cast<string>(o->id) + " distance", sqrt((destination.x - o->position.x) * (destination.x - o->position.x) + (destination.z - o->position.z) * (destination.z - o->position.z)));
		if((destination.x - p->position.x) * (destination.x - p->position.x) + (destination.z - p->position.z) * (destination.z - p->position.z) < 1000.0 * 1000.0)
		{
			startPatrol();
		}
		else
		{
			flyTowardsPoint(p, destination);
			p->controls.shoot1 = false;
			p->controls.shoot2 = false;
			p->controls.shoot3 = false;
		}


		Vec3f focus1 = p->position;
		Vec3f focus2 = p->position + p->rotation * Vec3f(0,0,2000.0);

		auto& planes = world->getAllOfType(PLANE);
		for(auto i=planes.begin(); i!=planes.end(); i++)
		{
			if(i->second->team != p->team)
			{
				//Profiler.setOutput(lexical_cast<string>(p->id)+"dist", focus1.distance(i->second->position) + focus2.distance(i->second->position));
				if(focus1.distance(i->second->position) + focus2.distance(i->second->position) < 3500.0)
				{
					startHunt(i->second->id);
					break;
				}
			}
		}
	}
	else if(state == STATE_HUNTING)
	{
		auto t = world->getObjectById(target);
		if(huntEndTime <= world->time() || !t || t->dead || !(t->type & PLANE))
		{
			target = 0;
			startPatrol();
		}		

		
		Vec3f direction = t->position-p->position;
		Vec3f velocity = p->rotation * Vec3f(0,0,1);
		Angle ang = acosA(velocity.dot(direction.normalize()));
		float distance = direction.magnitude();	
		
		//if(t->type & PLANE)
		//{
		//	Vec3f r = t->position - p->position;
		//	Vec3f v = t->rotation * Vec3f(0,0,static_pointer_cast<nPlane>(t)->speed);
		//
		//	float a = v.dot(v) - p->speed*p->speed;
		//	float b = 2.0 * v.dot(r);
		//	float c = r.dot(r);
		//	float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);
		//	if(b*b - 4.0*a*c >= 0.0 && t >= 0.0)
		//	{
		//		flyTowardsPoint(p, t->position + v * t);
		//	}
		//}
		//else
		//{
			float strength = 40.0 / pow(distance, 0.5f) - 4000.0 / pow(distance, 1.5f);
			flyTowardsPoint(p, t->position + t->rotation * Vec3f(0,0,max(distance-2000.0,0.0) / p->speed * dynamic_pointer_cast<plane>(t)->speed), strength);
		//}

	//	if(distance > 2000.0)
	//	{
			p->controls.accelerate = 1.0;
	//	}
	//	else
	//	{
	//		p->controls.accelerate = 0.0;
	//	}
		/*if(distance < 3000.0 && ang < PI/6 && missileCountDown < 0)
		{
			missileCountDown = 30000.0;
			p->controls.shoot1 = true;
			p->controls.shoot2 = true;
			p->controls.shoot3 = false;
		}
		else*/ if(distance < 3000.0 && ang < PI/6)
		{
			p->controls.shoot1 = true;
			p->controls.shoot2 = false;
			p->controls.shoot3 = false;
		}
		else
		{
			p->controls.shoot1 = false;
			p->controls.shoot2 = false;
			p->controls.shoot3 = false;
		}

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
void PlayerManager::removeAIplayer(int objectId)
{
	for(auto i = aiPlayers.begin(); i != aiPlayers.end(); i++)
	{
		if((*i)->objectNum() == objectId)
		{
			aiPlayers.erase(i);
			return;
		}
	}
}
void PlayerManager::resetPlayers()
{
	humanPlayers.clear();
	aiPlayers.clear();
}

void PlayerManager::update()
{
	for(auto i = humanPlayers.begin(); i != humanPlayers.end(); i++) //updates human players
	{
		(*i)->update();
	}
	for(auto i = aiPlayers.begin(); i != aiPlayers.end();) //updates AI players and deletes them if their object is gone
	{
		if((*i)->getObject() == nullptr)
		{
			i = aiPlayers.erase(i);
		}
		else
		{
			(*i)->update();
			i++;
		}
	}
}
#ifdef _DEBUG
	void PlayerManager::debugDraw()
	{
		for(auto i = aiPlayers.begin(); i!= aiPlayers.end(); i++)
		{
			//graphics->drawLine((*i)->getObject()->position, (*i)->destination);
		}
	}
#endif
shared_ptr<humanPlayer> PlayerManager::operator[] (unsigned int p)
{
	if(p < humanPlayers.size())
		return humanPlayers[p];

	return shared_ptr<humanPlayer>();
}
