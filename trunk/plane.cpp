

#include "game.h"

//plane::plane(int Team, Vec3f sPos, Quat4f sRot, objectType Type, objectController* c):controlledObject(sPos, sRot, Type, c), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0), lockRollRange(true), maxHealth(100)
//{
//	team = Team;
//	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
//}
plane::plane(Vec3f sPos, Quat4f sRot, objectType Type, int Team):object(Type, Team), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0), lockRollRange(true), cameraRotation(rotation), cameraShake(0.0),firstPerson(nullptr),thirdPerson(nullptr), controlType(CONTROL_TYPE_ADVANCED)
{
	//soundManager.playSound("sound1");

	lastPosition = position = sPos;
	lastRotation = rotation = sRot;
	meshInstance = objectInfo[type]->newMeshInstance(Mat4f(rotation, position));
	collisionInstance = objectInfo[type]->newCollisionInstance(Mat4f(rotation, position));

	debugAssert(meshInstance);

	cameraRotation = rotation;
	cameraOffset = rotation * Vec3f(0,sin(15.0*PI/180),-cos(15.0*PI/180));

	initArmaments();

	double altitude = world.altitude(position);
	if(altitude < 35)
		position.y -= altitude - 35;


	Vec3f f = (rotation * Vec3f(0,0,1)).normalize();
	Vec3f u = (rotation * Vec3f(0,1,0)).normalize();
	if(f.magnitudeSquared() < 0.001)
	{
		rotation = Quat4f();
		climb = 0;
		direction = 0;
	}
	else if(abs(f.y) > 0.999)
	{
		climb = PI/2 * abs(f.y)/f.y;
		direction = PI+atan2(u.x,u.z);
	}
	else//only condition that *should* happen
	{
		direction = atan2(f.x,f.z);
		climb = atan2(f.y,sqrt(f.x*f.x+f.z*f.z));
	}
	roll = 0;
	speed=400.0;

	controled=false;
	death = DEATH_NONE;
	health = 100.0;
	shotsFired = 0;

	target=0;
	targetLocked=false;

	smoothCamera(); //set up the camera
	observer.lastFrame = observer.currentFrame;
}
void plane::init()
{
	auto& engines = objectInfo.planeData(type)->engines;
	for(auto i=engines.begin(); i!=engines.end(); i++)
	{
		particle::planeEngines* e = new particle::planeEngines;
		particleManager.addEmitter(e, id, i->offset);
		e->setPositionAndRadius(position, i->radius);
	}

	smokeTrail = shared_ptr<particle::smokeTrail>(new particle::smokeTrail);
	particleManager.addEmitter(smokeTrail, id);
	smokeTrail->setActive(false);
}
void plane::updateSimulation(double time, double ms)
{
	//control->update();
	//controlState controller=control->getControlState();
	lastPosition = position;
 	lastRotation = rotation;

	observer.lastFrame = observer.currentFrame;
	cameraShake *= pow(0.1, world.time.length()/1000.0);


	if(!dead)
	{
		/////update rockets/////////////////////
		if(rockets.left<rockets.max)
			rockets.rechargeLeft-=ms;
		if(rockets.rechargeLeft<0)
		{
			rockets.left++;
			if(rockets.left==rockets.max)
				rockets.rechargeLeft=rockets.rechargeTime;
			else
				rockets.rechargeLeft+=rockets.rechargeTime;
		}
		rockets.coolDownLeft-=ms;
		/////update machine gun/////////////////
		if(machineGun.roundsLeft==0)
			machineGun.rechargeLeft-=ms;
		if(machineGun.rechargeLeft<0)
		{
			machineGun.left+=machineGun.roundsMax;
			machineGun.roundsLeft=machineGun.roundsMax;
			machineGun.rechargeLeft=machineGun.rechargeTime;
			extraShootTime = 0.0;
		}
		machineGun.coolDownLeft-=ms;
		////////////////////////////////////////
		if(controled)
		{
			if(wayPoints.size()>0)
				autoPilotUpdate(ms);
			else
				controled=false;
		}
		else
		{
			float Thrust = 108000.0f + 100000.0f*controls.accelerate - 80000.0f*controls.brake;
			float Drag = 0.70743 * speed*speed;
			float Lift = 14.1585 * speed*speed;
			float mass = 29300;

			float acceleration = (Thrust - Drag) / (mass*0.1) + 9.8*sin(climb) * 3.0;
			float jerk = -2.0f * (Drag/(speed*speed)) / (mass*0.1) * speed * acceleration;
			speed = taylor<float>(ms/1000, speed, acceleration, jerk);

			cameraShake = max(cameraShake, 0.025 * (controls.accelerate-controls.brake) * (speed-400.0)/200.0);

			if(controlType == CONTROL_TYPE_SIMPLE)
			{
				float deltaRoll = 1.5*controls.right*(ms/1000) - 1.5*controls.left*(ms/1000);
				float rollAng = roll.getAngle();
				if(rollAng > PI) rollAng -= PI*2;

				if(roll.inRange(-0.001,0.001) || (rollAng < 0.0 && rollAng + deltaRoll > 0.0) || (rollAng > 0.0 && rollAng + deltaRoll < 0.0))
				{
					lockRollRange = true;
				}
				else if(!roll.inRange(-PI/2,PI/2) && lockRollRange)//if roll is not in range but should be
				{
					if(roll.getAngle() < PI)
						roll = PI/2;
					else
						roll = -PI/2;
				}

				if(lockRollRange)
				{
					if(lockRollRange && deltaRoll > 0.0 && deltaRoll+rollAng > PI/2.0)
					{
						deltaRoll -= PI/2 - rollAng;
						roll = PI/2;
						//direction -= deltaRoll * 0.5;
					}
					else if(lockRollRange && deltaRoll < 0.0 && deltaRoll+rollAng < -PI/2.0)
					{
						deltaRoll -= -PI/2 - rollAng;
						roll = -PI/2;
						//direction -= deltaRoll * 0.5;
					}
					else
					{
						roll += deltaRoll;
						rollAng += deltaRoll;
					}

					if((roll.inRange(0.0,PI,false) && (controls.right-controls.left > 0)) || (roll.inRange(PI,2.0*PI,false) && (controls.right-controls.left < 0)))
					{
						direction -= Lift / (mass*0.2*speed) * sin(roll)/*cos(climb)*/ * (ms/1000);
					}
					else
					{
						direction -= Lift / (mass*0.2*speed) * sin(roll)/*cos(climb)*/ * (ms/1000) * 0.2;
					}
				}
				else
				{
					roll += deltaRoll;
				}

				if(abs(deltaRoll/(ms/1000)) < 0.1)
				{
					if(roll.inRange(0, PI/2 + 0.001))
						roll = max(0.0,roll.getAngle() - (ms/1000));
					else if(roll.inRange(-PI/2 - 0.001,0))
						roll = min(0.0, roll.getAngle()-PI*2 + (ms/1000));
				}

				climb += (1.0*controls.climb*(ms/1000) - 1.0*controls.dive*(ms/1000)) * cos(roll);

			}
			else if(controlType == CONTROL_TYPE_ADVANCED)
			{
				float deltaRoll = 2.0*(controls.right - controls.left)*(ms/1000);
				float deltaClimb = 1.5*(controls.climb - controls.dive)*(ms/1000);
				rotation = rotation * Quat4f(Vec3f(0,0,1), deltaRoll) * Quat4f(Vec3f(-1,0,0), deltaClimb);

				Vec3f fwd = rotation * Vec3f(0,0,1);
				Vec3f up = rotation * Vec3f(0,1,0);

				if(fwd.y < 0.99)
				{
					direction = atan2A(fwd.x, fwd.z);
				}
				else
				{
					direction = atan2A(-up.x, -up.z);
				}

			}
			else if(controlType == CONTROL_TYPE_AI)
			{
				roll += 1.5*(controls.right-controls.left)*(ms/1000);
				climb += (controls.climb - controls.dive) * (ms/1000) * cos(roll);
			}

			if(controlType == CONTROL_TYPE_SIMPLE || controlType == CONTROL_TYPE_AI)
			{
				if(climb > PI/2 && climb < PI*3/2)
				{
					direction += PI;
					roll += PI;
					climb = PI - climb;
					lockRollRange = false;
				}
				rotation = Quat4f(0,0,0,1);
				rotation = Quat4f(Vec3f(0,0,1),roll) * rotation;
				rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
				rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;
			}

			//////////////////////move//////////////////////////////////////
			if(ms>0)
			{
				position += rotation * Vec3f(0,0,1) * speed * (ms/1000);
			}
			////////////////////end move////////////////////////////////////

			if(!controls.shoot1)
				extraShootTime=0.0;
			else if(objectInfo.planeData(type)->machineGuns.size() > 0)
			{
				extraShootTime+=ms;
				while(extraShootTime > machineGun.coolDown && machineGun.roundsLeft > 0)
				{
					extraShootTime-=machineGun.coolDown;
					machineGun.roundsLeft--;

					Quat4f rot(slerp(rotation,lastRotation,(float)extraShootTime/ms));

					Vec3f o=rot*(objectInfo.planeData(type)->machineGuns[shotsFired%objectInfo.planeData(type)->machineGuns.size()]);
					Vec3f l=position*(1.0-extraShootTime/ms) + lastPosition*extraShootTime/ms;



					Angle randAng = random<float>() * PI * 2.0;
					float randF = (random<float>(-1.0,1.0) + random<float>(-1.0,1.0))/2.0 * (controlType == CONTROL_TYPE_AI ? PI/512 : PI/64);
					rot = Quat4f(Vec3f(cos(randAng),sin(randAng),0.0),randF) * rot;

					shotsFired++;
					((bulletCloud*)world[bullets].get())->addBullet(o + l,rot*Vec3f(0,0,1),id,time-extraShootTime-machineGun.coolDown);
					cameraShake = max(cameraShake, 0.05);

				//	if(random<float>() < 0.2)
				//		soundManager.playSound("shot1");
				//	else if(random<float>() < 0.25)
				//		soundManager.playSound("shot2");

			//		if(random<float>() < 0.25)
			//		{
			//			auto tracer = new particle::tracer;
			//			particleManager.addEmitter(tracer, position);
			//			tracer->setVelocity(rot*Vec3f(0,0,1) * 2000.0);
			//		}
				}
			}
			if(controls.shoot2)
				shootMissile();

			if(controls.shoot3 && roll.inRange(-PI/6,PI/6) && climb.inRange(-PI/4,PI/12))
			{
				//if(cameraShake < 0.1)
				//	cameraShake = 1.0;

				bombs.firing = true;
			}
			if(bombs.roundsLeft == 0)
			{
				bombs.firing = false;
				bombs.rechargeLeft -= ms;
				if(bombs.rechargeLeft < 0.0)
				{
					bombs.roundsLeft = bombs.roundsMax;
					bombs.rechargeLeft = bombs.rechargeTime;
				}
			}
			else if(bombs.firing)
			{
				bombs.coolDownLeft -= ms;
				while(bombs.coolDownLeft <= 0.0 && bombs.roundsLeft > 0)
				{
					dropBomb();
					cameraShake = max(cameraShake, 0.2);
				}
			}

			Vec2f positionXZ(position.x,position.z);
	//		Vec2f mapCenter = world.bounds().center;
	//		float r = world.bounds().radius;
	//		if(positionXZ.distanceSquared(mapCenter) > r*r)
	//		{
	//			returnToBattle();
	//		}
			smoothCamera();
		}
		altitude=world.altitude(position);
		findTarget();
	}//end if(!dead)
	if(dead) //if we were, or are now dead
	{
		altitude = world.altitude(position);
		speed += 9.8 * (ms/1000) * -sin(climb); //gravity

		if(death == DEATH_HIT_GROUND)
		{

		}
		else if(death == DEATH_EXPLOSION)
		{

		}
		else if(death == DEATH_HIT_WATER)
		{
			position.y -= 10.0 * (ms/1000);
		}
		else if(death == DEATH_TRAILING_SMOKE)
		{
			rotation = slerp(rotation, Quat4f(Vec3f(0,-1,0)), 1.0-pow(0.5, world.time.length()/1000));
			position += rotation * Vec3f(0,0,1) * 200.0 * (ms/1000);
			smoothCamera();
		}
		else
		{
			rotation = Quat4f(0,0,0,1);
			rotation = Quat4f(Vec3f(0,0,1),roll) * rotation;
			rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;

			position += rotation * Vec3f(0,0,1) * speed * (ms/1000);

			Vec3f vel2D = rotation * Vec3f(0,0,1);
			vel2D.y=0;
			vel2D = vel2D.normalize();

			observer.currentFrame.eye = position - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
			observer.currentFrame.center = position + vel2D * 45.0;
			observer.currentFrame.up = Vec3f(0,1,0);
		}

		if(death == DEATH_TRAILING_SMOKE && altitude < 0.0)
		{
			smokeTrail->setActive(false);
			death = world.isLand(position.x,position.z) ? DEATH_HIT_GROUND : DEATH_HIT_WATER;
			if(death == DEATH_HIT_GROUND)
			{
				position.y -= altitude;
				particleManager.addEmitter(new particle::blackSmoke(),id);
				particleManager.addEmitter(new particle::explosion(),id);
				particleManager.addEmitter(new particle::groundExplosionFlash(),id);
			}
			else if(death == DEATH_HIT_WATER)
			{
				Vec3f splashPos = position * lastPosition.y / (lastPosition.y - position.y) - lastPosition * position.y / (lastPosition.y - position.y);
				particleManager.addEmitter(new particle::splash(),splashPos, 10.0);
				particleManager.addEmitter(new particle::splash2(),splashPos, 10.0);
			
				Vec3f vel2D = rotation * Vec3f(0,0,1);
				vel2D.y=0;
				vel2D = vel2D.normalize();
			
				observer.currentFrame.eye = splashPos - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
				observer.currentFrame.center = splashPos + vel2D * 45.0;
				observer.currentFrame.up = Vec3f(0,1,0);
			}
		}
	}

	if(collisionInstance)
	{
		collisionInstance->update(Mat4f(rotation,position), Mat4f(lastRotation,lastPosition));
	}
}
void plane::updateFrame(float interpolation) const
{
	//float oldInterpolation = interpolation - 16.67 / world.time.getUpdateLength(); //what interpolation would have been one frame ago

	Vec3f pos = lerp(lastPosition, position, interpolation);
	Quat4f rot = slerp(lastRotation, rotation, interpolation);


	bool visible = !dead || death != DEATH_EXPLOSION;
	meshInstance->update(Mat4f(rot, pos), visible);
	int n=0;
	for(auto i = bombs.ammoRounds.begin(); i != bombs.ammoRounds.end(); i++, n++)
		i->meshInstance->update(Mat4f(rot, pos + rot * i->offset), bombs.roundsMax - bombs.roundsLeft <= n && visible);
	n=0;
	for(auto i = rockets.ammoRounds.begin(); i != rockets.ammoRounds.end(); i++, n++)
		i->meshInstance->update(Mat4f(rot, pos + rot * i->offset),  rockets.max - rockets.left <= n && visible);

	//auto& engines = objectInfo.planeStats(type).engines;
	//for(int i=0; i < engines.size(); i++)
	//	engineLights[i]->position = pos + rot * engines[i];


	if(firstPerson)
	{
		//Vec3f oldPos = lerp(lastPosition, position, oldInterpolation);
		//Quat4f oldRot = slerp(lastRotation, rotation, oldInterpolation);

		firstPerson->eye	= pos;
		firstPerson->center	= rot * Vec3f(0,0,1) + pos;
		firstPerson->up		= rot * Vec3f(0,1,0);

		//firstPerson->oldEye		= oldPos;
		//firstPerson->oldCenter	= oldRot * Vec3f(0,0,1) + oldPos;
		//firstPerson->oldUp		= oldRot * Vec3f(0,1,0);
	}
	if(thirdPerson)
	{
		thirdPerson->center	= lerp(observer.lastFrame.center, observer.currentFrame.center, interpolation);
		thirdPerson->eye	= lerp(observer.lastFrame.eye, observer.currentFrame.eye, interpolation);
		thirdPerson->up		= lerp(observer.lastFrame.up, observer.currentFrame.up, interpolation);

		//thirdPerson->center		= lerp(observer.lastFrame.center, observer.currentFrame.center, oldInterpolation);
		//thirdPerson->oldCenter	= lerp(observer.lastFrame.eye, observer.currentFrame.eye, oldInterpolation);
		//thirdPerson->oldUp		= lerp(observer.lastFrame.up, observer.currentFrame.up, oldInterpolation);
	}
}
void plane::smoothCamera()
{
	Vec3f shake = random3<float>() * cameraShake * 3.0;
	float cameraDistance = objectInfo.planeData(type)->cameraDistance + 8.0 * max(0.0,pow((speed-300.0)/300.0,3.0));

	cameraRotation = slerp(cameraRotation, rotation, 0.07);
	cameraOffset = lerp(cameraOffset, (cameraRotation * Vec3f(0,sin(15.0*PI/180),-cos(15.0*PI/180))), 0.14);

	observer.currentFrame.center	= position + cameraRotation * Vec3f(0,0,40.0) + shake;
	observer.currentFrame.eye		= position + cameraOffset * cameraDistance;
	observer.currentFrame.up		= cameraRotation * Vec3f(0,1,0);
}
void plane::autoPilotUpdate(float value)
{
	wayPoint w[4];
	double time=world.time();
	float t;
	if(time>(wayPoints.back()).time)
	{
		controled=false;
		Vec3f fwd = wayPoints.back().rotation * Vec3f(0,0,1);

		rotation=		wayPoints.back().rotation;
		position =		wayPoints.back().position;
		direction =		atan2A(fwd.x,fwd.z);
		climb =			asin(fwd.y/fwd.magnitude());
		roll =			0;//to hard and rather useless to find

		observer.currentFrame.center.x = position.x;
		observer.currentFrame.center.z = position.z;

		wayPoints.clear();
		return;
	}
	else if(time<wayPoints.begin()->time)
	{
		return;// should not occur
	}

	for(int i=1;i<wayPoints.size();i++)
	{
		if(wayPoints[i-1].time<time && wayPoints[i].time>time)
		{
			w[0] = i>1 ? wayPoints[i-2] : wayPoints[i-1];
			w[1] = wayPoints[i-1];
			w[2] = wayPoints[i];
			w[3] = i<wayPoints.size()-1 ? wayPoints[i+1] : wayPoints[i];
		}
	}

	t=1.0-(w[2].time-time)/(w[2].time-w[1].time);

	position = catmullRomInterpolate(w[0].position, w[1].position, w[2].position, w[3].position, t);
	rotation = slerp(w[1].rotation,w[2].rotation,t);

	observer.currentFrame.center.x = position.x;
	observer.currentFrame.center.z = position.z;

	//Vec3f fwd = rotation * Vec3f(0,0,1);
	//direction =	atan2A(fwd.x,fwd.z);
	//climb =		asin(fwd.y/fwd.magnitude());
	//turn =		0;

}
void plane::exitAutoPilot()
{
	wayPoint w1;
	wayPoint w2;
	double time=world.time();
	float t;
	if(wayPoints.size()==0)//no waypoints
	{
		roll=0;
		controled=false;
		return;
	}
	if(time>(wayPoints.back()).time)//beyond last waypoint
	{
		controled=false;
		Vec3f fwd = wayPoints.back().rotation * Vec3f(0,0,1);

		rotation=		wayPoints.back().rotation;
		position =		wayPoints.back().position;
		direction =		atan2A(fwd.x,fwd.z);
		climb =			asin(fwd.y/fwd.magnitude());
		roll =			0;//to hard and rather useless to find

		wayPoints.clear();
		return;
	}
	else if(time<wayPoints.begin()->time)//autopilot not yet started
	{
		return;// should not occur
	}

	for(int i=1;i<wayPoints.size();i++)
	{
		if(wayPoints[i-1].time<time && wayPoints[i].time>time)
		{
			w1=wayPoints[i-1];
			w2=wayPoints[i];
		}
	}

	t=(w2.time-time)/(w2.time-w1.time);

	position =	w1.position*t+w2.position*(1.0-t);
	rotation =	slerp(w1.rotation,w2.rotation,t);
	Vec3f fwd = rotation * Vec3f(0,0,1);
	direction =	atan2A(fwd.x,fwd.z);
	climb =		asin(fwd.y/fwd.magnitude());
	roll =		0;

	controled=false;
	wayPoints.clear();
}
void plane::returnToBattle()//needs to be adjusted for initial speed
{
	cameraStates.clear();
	wayPoints.clear();
	controled =true;
	double time=world.time();

	Vec3f fwd	= rotation * Vec3f(0,0,1);
	//Vec3f up	= rotation * Vec3f(0,1,0);
	//Vec3f right	= rotation * Vec3f(1,0,0);
	fwd.y=0; fwd=fwd.normalize();

	Quat4f newRot(Vec3f(0,1,0),atan2A(position.x-world.bounds().center.x,position.z-world.bounds().center.y));
	Vec3f newFwd = newRot * Vec3f(0,0,1);

	observer.currentFrame.eye = Vec3f(position.x - fwd.x*20, position.y + sin(45.0)*20,	 position.z - fwd.z*20);
	observer.currentFrame.center = Vec3f(position.x + fwd.x*35, position.y, position.z + fwd.z*35);
	observer.currentFrame.up = Vec3f(0,1,0);

	float scale = 0.6;
	wayPoints.push_back(wayPoint(time,					position,											rotation								));
	wayPoints.push_back(wayPoint(time+3000.0*scale,		position+newFwd*scale*2500,							newRot									));
	wayPoints.push_back(wayPoint(time+3610.0*scale,		position+newFwd*scale*2875+Vec3f(0,375,0)*scale,	newRot * Quat4f(Vec3f(-1,0,0),PI*0.25)	));
	wayPoints.push_back(wayPoint(time+4220.0*scale,		position+newFwd*scale*2500+Vec3f(0,750,0)*scale,	newRot * Quat4f(Vec3f(-1,0,0),PI*0.5)	));
	wayPoints.push_back(wayPoint(time+4460.0*scale,		position+newFwd*scale*2309+Vec3f(0,588,0)*scale,	newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5300.0*scale,		position+newFwd*scale*1743+Vec3f(0,176,0)*scale,	newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5660.0*scale,		position+newFwd*scale*1500,							newRot * Quat4f(Vec3f(-1,0,0),PI)		));
	wayPoints.push_back(wayPoint(time+7000.0*scale,		position-newFwd*scale*55,							newRot * Quat4f(Vec3f(0,0,1),PI) * Quat4f(Vec3f(-1,0,0),PI) 	));

	////wayPoints.push_back(wayPoint(time+7000.0,	pos,
	////							0,				acceleration,
	////							angle+180,		0));


}

void plane::die(deathType d)
{
	if(dead) //if we are already dead
		return;

	//explode=new explosion(position);
	//particleManager.addEmitter(new particle::blackSmoke(id));

	dead = true;
	if(controled)
	{
		exitAutoPilot();
	}


	if(d == DEATH_NONE)
	{
	//	if(controlType == CONTROL_TYPE_AI)
		{
			death = DEATH_TRAILING_SMOKE;
		
			smokeTrail->setActive(true);
			smokeTrail->setColor(Color(0.05,0.05,0.05));
		
			particleManager.addEmitter(new particle::fireball(), id);

			//particleManager.addEmitter(new particle::debrisSmokeTrail(), position, 1.0);
			//particleManager.addEmitter(new particle::debrisSmokeTrail(), position, 1.0);
			//particleManager.addEmitter(new particle::debrisSmokeTrail(), position, 1.0);
		}
	//	else
	//	{
	//		death = DEATH_EXPLOSION;
	//	}

		particleManager.addEmitter(new particle::explosion(),id);
		particleManager.addEmitter(new particle::explosionSmoke(),id);
		particleManager.addEmitter(new particle::explosionFlash(),id);
		particleManager.addEmitter(new particle::explosionFlash2(),id);
		//particleManager.addEmitter(new particle::explosionSparks(),id);
	}
	else if(d == DEATH_HIT_GROUND)
	{
		position.y -= world.altitude(position);
		//particleManager.addEmitter(new particle::blackSmoke(id));

		death = DEATH_HIT_GROUND;
		particleManager.addEmitter(new particle::blackSmoke(),id);
		particleManager.addEmitter(new particle::explosion(),id);
		particleManager.addEmitter(new particle::groundExplosionFlash(),id);

		cameraShake = 1.0;
	}
	else if(d == DEATH_HIT_WATER)
	{
		death = DEATH_HIT_WATER;
		Vec3f splashPos = lerp(lastPosition, position, lastPosition.y/(lastPosition.y - position.y));
		particleManager.addEmitter(new particle::splash(),splashPos, 10.0);
		particleManager.addEmitter(new particle::splash2(),splashPos, 10.0);

		Vec3f vel2D = rotation * Vec3f(0,0,1);
		vel2D.y=0;
		vel2D = vel2D.normalize();

		observer.currentFrame.eye = splashPos - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
		observer.currentFrame.center = splashPos + vel2D * 45.0;
		observer.currentFrame.up = Vec3f(0,1,0);

		cameraShake = 0.7;
	}
	//smokeTrail->setVisible(false);
}
void plane::findTarget()
{
	target = 0;
	//Angle minAng =PI/3;
	bool targetFound=false;
	float minDistSquared;
	float distSquared;
	Angle ang;
	auto& planes = world(PLANE);
	for(auto i = planes.begin(); i != planes.end();i++)
	{
		distSquared = position.distanceSquared((*i).second->position);
		if(!i->second->dead && team != i->second->team && (!targetFound || distSquared < minDistSquared))
		{
			ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
			minDistSquared = distSquared;
			target = i->second->id;
			targetFound = true;
		}
	}
	auto& AAA = world(ANTI_AIRCRAFT_ARTILLARY);
	for(auto i = AAA.begin(); i != AAA.end();i++)
	{
		distSquared = position.distanceSquared((*i).second->position);
		if(!i->second->dead && team != i->second->team && (!targetFound || distSquared < minDistSquared))
		{
			ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
			minDistSquared = distSquared;
			target = i->second->id;
			targetFound = true;
		}
	}
	auto& ships = world(SHIP);
	for(auto i = ships.begin(); i != ships.end();i++)
	{
		distSquared = position.distanceSquared((*i).second->position);
		if(!i->second->dead && team != i->second->team && (!targetFound || distSquared < minDistSquared))
		{
			ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
			minDistSquared = distSquared;
			target = i->second->id;
			targetFound = true;
		}
	}
	targetLocked = targetFound && (ang < PI/6 && minDistSquared < 2000 * 2000);
}
void plane::shootMissile()
{
	if(rockets.coolDownLeft>0 || rockets.left<=0 || controled)
		return;

	Vec3f fwd	= (rotation * Vec3f(0,0,1)).normalize(),
		  up	= (rotation * Vec3f(0,1,0)).normalize(),
		  right	= (rotation * Vec3f(1,0,0)).normalize();

	int pId=0;

	if(target != 0 && world[target] != nullptr && targetLocked && acosA( (rotation*Vec3f(0,0,1)).dot((world[target]->position-position).normalize()) ) && position.distanceSquared(world[target]->position) < 2000 * 2000)
	{
		pId = target;
	}
	else
	{
		Angle minAng=PI/6;
		auto planes = world(PLANE);
		for(auto i = planes.begin(); i != planes.end();i++)
		{
			Angle ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
			if(!i->second->dead && team != i->second->team && ang < minAng && position.distanceSquared((*i).second->position) < 2000 * 2000)
			{
				minAng = ang;
				pId = i->second->id;
			}
		}
		auto AAA = world(ANTI_AIRCRAFT_ARTILLARY);
		for(auto i = AAA.begin(); i != AAA.end();i++)
		{
			Angle ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
			if(!i->second->dead && team != i->second->team && ang < minAng && position.distanceSquared((*i).second->position) < 2000 * 2000)
			{
				minAng = ang;
				pId = i->second->id;
			}
		}
		auto ships = world(SHIP);
		for(auto i = ships.begin(); i != ships.end();i++)
		{
			Angle ang = acosA( (rotation*Vec3f(0,0,1)).dot(((*i).second->position-position).normalize()) );
			if(!i->second->dead && team != i->second->team && ang < minAng && position.distanceSquared((*i).second->position) < 2000 * 2000)
			{
				minAng = ang;
				pId = i->second->id;
			}
		}
	}
	//if(pId == 0)
	//	return;

	missileType t = rockets.ammoRounds[rockets.max - rockets.left].type;
	Vec3f o = rockets.ammoRounds[rockets.max - rockets.left].offset;

	world.newObject(new missile(t, team, lastPosition+right*o.x+up*o.y+fwd*o.z, rotation, speed, id, pId)); //lastPosition is used since the missile will update this frame and move to under the plane
	rockets.coolDownLeft=rockets.coolDown;
	rockets.left--;
}
void plane::dropBomb()
{
	if(bombs.coolDownLeft>0 || bombs.roundsLeft<=0 || controled)
		return;

	Vec3f fwd	= (rotation * Vec3f(0,0,1)).normalize(),
		  up	= (rotation * Vec3f(0,1,0)).normalize(),
		  right	= (rotation * Vec3f(1,0,0)).normalize();

	bombType t = bombs.ammoRounds[bombs.roundsMax-bombs.roundsLeft].type;
	Vec3f o = bombs.ammoRounds[bombs.roundsMax-bombs.roundsLeft].offset;

	world.newObject(new bomb(t, team, lastPosition+right*o.x+up*o.y+fwd*o.z, rotation, speed, id)); //lastPosition is used since the bomb will update this frame and move to under the plane
	bombs.coolDownLeft += bombs.coolDown;
	bombs.roundsLeft--;
}
void plane::loseHealth(float healthLoss)
{
	//smokeTrail->setActive(true);
	//float c = max(0.5 * floor(health/maxHealth * 4.0) / 4.0, 0.0);
	//smokeTrail->setColor(Color(c, c, c));

	health-=healthLoss;
	if(health<=0.0)
	{
		//smokeTrail->setActive(false);
		health=0;
		cameraShake = 1.0;
		die();
	}
	else
	{
		cameraShake = min(healthLoss / 30.0, 1.0);
	}
}
void plane::initArmaments()
{
	armament::ammo a;
	auto& hardpoints = objectInfo.planeData(type)->hardpoints;
	for(int i = 0; i < hardpoints.size(); i++)
	{
		a.type = hardpoints[i].mType;
		a.offset = hardpoints[i].offset;
		if(!objectInfo[a.type]->mesh.expired())
		{
			a.meshInstance = sceneManager.newChildMeshInstance(objectInfo[a.type]->mesh.lock(), meshInstance, Mat4f(rotation, position + rotation * a.offset));
		}

		if(hardpoints[i].mType & MISSILE)
		{
			rockets.ammoRounds.push_back(a);
		}
		else if(hardpoints[i].mType & BOMB)
		{
			bombs.ammoRounds.push_back(a);
		}
	}

	rockets.max				= rockets.left				= rockets.ammoRounds.size();
	rockets.roundsMax		= rockets.roundsLeft		= 1;
	rockets.coolDown		= rockets.coolDownLeft		= 1000.0f;
	rockets.rechargeTime	= rockets.rechargeLeft		= 7000.0f;
	//rockets.firing									= false;	 not used

	bombs.max				= bombs.left				= 1;
	bombs.roundsMax			= bombs.roundsLeft			= bombs.ammoRounds.size();
	bombs.coolDown			= bombs.coolDownLeft		= 100.0;
	bombs.rechargeTime		= bombs.rechargeLeft		= 8000.0f;
	bombs.firing										= false;

	machineGun.max			= machineGun.left			= 1000;
	machineGun.roundsMax	= machineGun.roundsLeft		= 200;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 450.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 26.0;
	machineGun.firing									= false;
}