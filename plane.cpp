

#include "game.h"

//nPlane::nPlane(int Team, Vec3f sPos, Quat4f sRot, objectType Type, objectController* c):controlledObject(sPos, sRot, Type, c), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0), lockRollRange(true), maxHealth(100)
//{
//	team = Team;
//	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
//}
nPlane::nPlane(int Team, Vec3f sPos, Quat4f sRot, objectType Type):object(sPos, sRot, Type), lastUpdateTime(world.time()), extraShootTime(0.0),shotsFired(0), lockRollRange(true), maxHealth(100),controlType(CONTROL_TYPE_SIMPLE)
{
	team = Team;
	meshInstance = sceneManager.newMeshInstance(objectTypeString(type), position, rotation);
}

void nPlane::init()
{
	spawn();
}
void nPlane::updateSimulation(double time, double ms)
{
	//control->update();
	//controlState controller=control->getControlState();
	lastPosition = position;
	lastRotation = rotation;

	observer.lastFrame = observer.currentFrame;

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

		if(controled)
		{
			if(wayPoints.size()>0)
				autoPilotUpdate(ms);
			else
				controled=false;

			altitude=world.altitude(position);
		}
		else
		{
			float Thrust = 64000.0 + 144000.0f*controls.accelerate - 36000.0*controls.brake;
			float Drag = 0.70743 * speed*speed;
			float Lift = 14.1585 * speed*speed;
			float mass = 29300;

			float acceleration = (Thrust - Drag) / (mass*0.1) + 9.8*sin(climb);
			float jerk = -2.0f * (Drag/(speed*speed)) / (mass*0.1) * speed * acceleration;
			speed = taylor<float>(ms/1000, speed, acceleration, jerk);

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

					if(roll.inRange(0.0,PI,false) && (controls.right-controls.left > 0) || roll.inRange(PI,2.0*PI,false) && (controls.right-controls.left < 0))
					{
						direction -= Lift / (mass*0.2*speed) * sin(roll)/cos(climb) * (ms/1000);
					}
					else
					{
						direction -= Lift / (mass*0.2*speed) * sin(roll)/cos(climb) * (ms/1000) * 0.2;
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
				roll += 1.5*controls.right*(ms/1000) - 1.5*controls.left*(ms/1000);
				//direction -= Lift / (mass*0.2*speed) * sin(roll)/cos(climb) * (controls.climb - controls.dive) * (ms/1000);
				climb += (controls.climb - controls.dive) * (ms/1000) * cos(roll);
			}
			//climb -= (L * cos(roll) / (m*speed) - 9.8*cos(climb)/speed) * (ms/1000) -
			//			(1.0*controller.climb*(ms/1000) - 1.0*controller.dive*(ms/1000)) * cos(roll);


			//if(roll.inRange(-PI/2 - 0.001, PI/2 + 0.001))
			//	roll = roll - abs(1.0*controller.climb*(ms/1000) - 1.0*controller.dive*(ms/1000)) * sin(roll);
			//else
			//	roll = roll + abs(1.0*controller.climb*(ms/1000) - 1.0*controller.dive*(ms/1000)) * sin(roll);


			//direction -= (0.3*controller.climb*(ms/1000) - 0.3*controller.dive*(ms/1000))*sin(roll);
			//turn  = clamp(turn  + 1.5*controller.right*(ms/1000) - 1.5*controller.left*(ms/1000),-1.0,1.0);
			//climb = clamp(climb + (1.0*controller.climb*(ms/1000) - 1.0*controller.dive*(ms/1000))*cos(turn),-PI/3,PI/4);
			//direction -= turn*ms/3000.0;//needs to be adjusted to be continious
			speed += 9.8 * (ms/1000) * -sin(climb);//gravity

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



			//////////////////////move//////////////////////////////////////
			if(ms>0)
			{
				position += rotation * Vec3f(0,0,1) * speed * (ms/1000);
			}
			////////////////////end move////////////////////////////////////

			if(!controls.shoot1)
				extraShootTime=0.0;
			else
			{
				extraShootTime+=ms;
				while(extraShootTime > machineGun.coolDown && machineGun.roundsLeft > 0)
				{
					extraShootTime-=machineGun.coolDown;
					machineGun.roundsLeft--;

					Quat4f rot(slerp(rotation,lastRotation,(float)extraShootTime/ms));

					Vec3f o=rot*(settings.planeStats[type].machineGuns[shotsFired%settings.planeStats[type].machineGuns.size()]);
					Vec3f l=position*(1.0-extraShootTime/ms) + lastPosition*extraShootTime/ms;


					Angle randAng = random<float>() * PI * 2.0;
					float randF = (random<float>(-1.0,1.0) + random<float>(-1.0,1.0))/2.0 * PI/64;
					rot = Quat4f(Vec3f(cos(randAng),sin(randAng),0.0),randF) * rot;

					shotsFired++;
					((bulletCloud*)world[bullets].get())->addBullet(o + l,rot*Vec3f(0,0,1),id,time-extraShootTime-machineGun.coolDown);
	//				world.bullets.push_back(bullet(o + l,rot*Vec3f(0,0,1),id,time-extraShootTime-machineGun.coolDown));
				}
			}
			if(controls.shoot2)	shootMissile();

			if(controls.shoot3 && roll.inRange(-PI/6,PI/6) && climb.inRange(-PI/4,PI/12))
				bombs.firing = true;

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
				}
			}

			planePath.currentPoint(position,rotation);

			smoothCamera();



			Vec2f positionXZ(position.x,position.z);
			Vec2f mapCenter = world.bounds().center;
			float r = world.bounds().radius;
			if(positionXZ.distanceSquared(mapCenter) > r*r)
			{
				returnToBattle();
			}

			altitude=world.altitude(position);
			if(altitude < 0.0)
			{
				death = world.isLand(position.x,position.z) ? DEATH_HIT_GROUND : DEATH_HIT_WATER;
				if(death == DEATH_HIT_GROUND)
				{
					position.y -= altitude;

					//particleManager.addEmitter(new particle::blackSmoke(id));

					death = DEATH_EXPLOSION;
					particleManager.addEmitter(new particle::explosion(),id);
				}
				else if(death == DEATH_HIT_WATER)
				{
					Vec3f splashPos = position * lastPosition.y / (lastPosition.y - position.y) - lastPosition * position.y / (lastPosition.y - position.y);
					particleManager.addEmitter(new particle::splash(),splashPos);


					Vec3f vel2D = rotation * Vec3f(0,0,1);
					vel2D.y=0;
					vel2D = vel2D.normalize();

					observer.currentFrame.eye = splashPos - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
					observer.currentFrame.center = splashPos + vel2D * 45.0;
					observer.currentFrame.up = Vec3f(0,1,0);
				}
				die();
			}
		}

		findTargetVector();




	}

	if(dead)
	{
		altitude = world.altitude(position);
		speed += 9.8 * (ms/1000) * -sin(climb); //gravity

		if(death == DEATH_HIT_GROUND)
		{

		}
		else if(death == DEATH_HIT_WATER)
		{
			//rotation = Quat4f(0,0,0,1);
			//rotation = Quat4f(Vec3f(0,0,1),turn) * rotation;
			//rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			//rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;
			//position += rotation * Vec3f(0,0,1) * (ms/1000);

			position.y -= 10.0 * (ms/1000);
		}
		else if(death == DEATH_EXPLOSION)
		{
			//rotation = Quat4f(0,0,0,1);
			//rotation = Quat4f(Vec3f(0,0,1),roll) * rotation;
			//rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			//rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;

			//position += rotation * Vec3f(0,0,1) * speed * (ms/1000) * 0.1;
		}
		else
		{
			rotation = Quat4f(0,0,0,1);
			rotation = Quat4f(Vec3f(0,0,1),roll) * rotation;
			rotation = Quat4f(Vec3f(1,0,0),-climb) * rotation;
			rotation = Quat4f(Vec3f(0,1,0),direction) * rotation;

			position += rotation * Vec3f(0,0,1) * speed * (ms/1000);

			//int sgn=roll/abs(roll);
			//turn-=0.1*(ms/1000)*sgn;
			//if(turn/abs(turn)!=sgn)
			//	turn=0;

			//climb -= 0.3 * (ms/1000);
			//if(climb < -PI/2)
			//	climb = -PI/2;

			Vec3f vel2D = rotation * Vec3f(0,0,1);
			vel2D.y=0;
			vel2D = vel2D.normalize();

			observer.currentFrame.eye = position - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
			observer.currentFrame.center = position + vel2D * 45.0;
			observer.currentFrame.up = Vec3f(0,1,0);
		}

		if(death == DEATH_TRAILING_SMOKE && altitude < 0.0)
		{
			death = world.isLand(position.x,position.z) ? DEATH_HIT_GROUND : DEATH_HIT_WATER;
			if(death == DEATH_HIT_GROUND)
			{
				position.y -= altitude;
				particleManager.addEmitter(new particle::blackSmoke(),id);
			}
			else if(death == DEATH_HIT_WATER)
			{
				Vec3f splashPos = position * lastPosition.y / (lastPosition.y - position.y) - lastPosition * position.y / (lastPosition.y - position.y);
				particleManager.addEmitter(new particle::splash(),splashPos);


				Vec3f vel2D = rotation * Vec3f(0,0,1);
				vel2D.y=0;
				vel2D = vel2D.normalize();

				observer.currentFrame.eye = splashPos - Vec3f(vel2D.x, -0.60, vel2D.z)*45.0;
				observer.currentFrame.center = splashPos + vel2D * 45.0;
				observer.currentFrame.up = Vec3f(0,1,0);
			}
		}
	}
}
void nPlane::updateFrame(float interpolation) const
{
	Vec3f pos = lerp(lastPosition, position, interpolation);
	Quat4f rot = slerp(lastRotation, rotation, interpolation);


	bool visible = !dead || death != DEATH_EXPLOSION;
	meshInstance->update(pos, rot, visible);
	int n=0;
	for(auto i = bombs.ammoRounds.begin(); i != bombs.ammoRounds.end(); i++, n++)
		i->meshInstance->update(pos + rot * i->offset, rot, bombs.roundsMax - bombs.roundsLeft <= n && visible);
	n=0;
	for(auto i = rockets.ammoRounds.begin(); i != rockets.ammoRounds.end(); i++, n++)
		i->meshInstance->update(pos + rot * i->offset, rot, rockets.max - rockets.left <= n && visible);

	if(firstPerson)
	{
		firstPerson->eye	= pos;
		firstPerson->center	= rot * Vec3f(0,0,1) + pos;
		firstPerson->up		= rot * Vec3f(0,1,0);
	}
	if(thirdPerson)
	{
		thirdPerson->center	= lerp(observer.lastFrame.center, observer.currentFrame.center, interpolation);
		thirdPerson->eye	= lerp(observer.lastFrame.eye, observer.currentFrame.eye, interpolation);
		thirdPerson->up		= lerp(observer.lastFrame.up, observer.currentFrame.up, interpolation);
	}
}
void nPlane::smoothCamera()
{
	if(!cameraStates.empty() && cameraStates.front().time < world.time() - 300)
	{
		auto i = cameraStates.begin();
		while(i+1 != cameraStates.end() && (i+1)->time  < world.time() - 300)
			i++;

		if(i != cameraStates.begin())
			cameraStates.erase(cameraStates.begin(),i);
	}

	cameraState a;
	a.angle = direction;// * Quat4f(Vec3f(-1,0,0),Angle(PI * 0.9));
 	a.time = world.time();
	a.position = position;
	a.velocity = rotation * Vec3f(0,0,speed);

	if(!cameraStates.empty())
	{
		while(abs(cameraStates.back().angle - a.angle) > abs(cameraStates.back().angle - (a.angle+PI*2)))	a.angle += PI*2;
		while(abs(cameraStates.back().angle - a.angle) > abs(cameraStates.back().angle - (a.angle-PI*2)))	a.angle -= PI*2;
	}
	if(cameraStates.empty() || cameraStates.back().time < a.time)
		cameraStates.push_back(a);

	float ang=0.0;
	Vec3f vel;
	Vec3f pos;
	//float dt;
	//float t;
	if(cameraStates.size() > 1)
	{
		for(auto i = cameraStates.begin(); i+1 != cameraStates.end(); i++)
		{
			ang += (i->angle + (i+1)->angle) * ((i+1)->time - i->time) / 2;
			pos += (i->position + (i+1)->position) * ((i+1)->time - i->time) / 2;
			vel += (i->velocity + (i+1)->velocity) * ((i+1)->time - i->time) / 2;
			//dir += (i->rot * Vec3f(0,0,1) + (i+1)->rot * Vec3f(0,0,1)) * ((i+1)->time - i->time) / 2;
			//v += (i->rot * Vec3f(0,0.385,-0.923) + (i+1)->rot * Vec3f(0,0.385,-0.923)) * ((i+1)->time - i->time) / 2;
		}
		ang /= world.time() - cameraStates.front().time;
		pos /= world.time() - cameraStates.front().time;
		vel /= world.time() - cameraStates.front().time;
		//dt = (world.time() - cameraStates.front().time)/2000;
		//dir = dir / (world.time() - cameraStates.front().time);
		//v = v / (world.time() - cameraStates.front().time);
	}
	else
	{
		ang = a.angle;
		pos = a.position;
		vel = a.velocity;
		//dt = 0.0;
		//dir = a.rot * Vec3f(0,0,1);
		//v = a.rot * Vec3f(0,0.385,-0.923);
	}


	observer.currentFrame.eye = /*pos + vel*dt*/position - Vec3f(sin(ang), -0.60, cos(ang)) * 35.0;
	observer.currentFrame.center = /*pos + vel*dt*/position + Vec3f(sin(ang), 0.0, cos(ang)) * 35.0;
	observer.currentFrame.up = Vec3f(0,1,0);

	//center = position + dir * 15.0;
	//camera = position + dir * 15.0 + v * 45;
	//up = rotation * Vec3f(0,1,0);
}
void nPlane::autoPilotUpdate(float value)
{
	wayPoint w1;
	wayPoint w2;
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
			w1=wayPoints[i-1];
			w2=wayPoints[i];
		}
	}

	t=1.0-(w2.time-time)/(w2.time-w1.time);

	position = w1.position*(1.0-t)+w2.position*t;
	rotation = slerp(w1.rotation,w2.rotation,t);
	observer.currentFrame.center.x = position.x;
	observer.currentFrame.center.z = position.z;
	//Vec3f fwd = rotation * Vec3f(0,0,1);
	//direction =	atan2A(fwd.x,fwd.z);
	//climb =		asin(fwd.y/fwd.magnitude());
	//turn =		0;

}
void nPlane::exitAutoPilot()
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
void nPlane::returnToBattle()//needs to be adjusted for initial speed
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

	wayPoints.push_back(wayPoint(time,				position,								rotation								));
	wayPoints.push_back(wayPoint(time+3000.0,		position+newFwd*2500,					newRot									));
	wayPoints.push_back(wayPoint(time+3610.0,		position+newFwd*2875+Vec3f(0,375,0),	newRot * Quat4f(Vec3f(-1,0,0),PI*0.25)	));
	wayPoints.push_back(wayPoint(time+4220.0,		position+newFwd*2500+Vec3f(0,750,0),	newRot * Quat4f(Vec3f(-1,0,0),PI*0.5)	));
	wayPoints.push_back(wayPoint(time+4460.0,		position+newFwd*2309+Vec3f(0,588,0),	newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5300.0,		position+newFwd*1743+Vec3f(0,176,0),	newRot * Quat4f(Vec3f(-1,0,0),3.7699)	));
	wayPoints.push_back(wayPoint(time+5660.0,		position+newFwd*1500,					newRot * Quat4f(Vec3f(-1,0,0),PI)		));
	wayPoints.push_back(wayPoint(time+7000.0,		position-newFwd*55,						newRot * Quat4f(Vec3f(0,0,1),PI) * Quat4f(Vec3f(-1,0,0),PI) 	));

	////wayPoints.push_back(wayPoint(time+7000.0,	pos,
	////							0,				acceleration,
	////							angle+180,		0));


}

void nPlane::die()
{
	//if(!dead)	explode=new explosion(position);
	if(!dead)
	{
		//particleManager.addEmitter(new particle::blackSmoke(id));
	}
	dead =true;
	if(controled)
	{
		exitAutoPilot();
	//	controled=true;
	}

	if(!respawning)
		respawnTime=world.time()+5000;
	respawning=true;

	if(death == DEATH_NONE)
	{
		death = DEATH_EXPLOSION;
		particleManager.addEmitter(new particle::explosion(),id);
		particleManager.addEmitter(new particle::explosionSmoke(),id);
		particleManager.addEmitter(new particle::explosionFlash(),id);
	}
}
void nPlane::findTargetVector()
{
	//Vec3f enemy;
	//for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
	//{
	//	enemy=(*i).second->pos;
	//	if(acos( velocity.dot( (enemy-pos).normalize() )) < 0.1 && (*i).first!=id && !(*i).second->dead && pos.distance(enemy)<4000)
	//	{
	//		targeter=(enemy-pos).normalize();
	//		return;
	//	}
	//}
	//targeter=velocity;
	targeter = rotation * Vec3f(0,0,1);
}

void nPlane::shootMissile()
{
	if(rockets.coolDownLeft>0 || rockets.left<=0 || controled)
		return;

	Vec3f fwd	= (rotation * Vec3f(0,0,1)).normalize(),
		  up	= (rotation * Vec3f(0,1,0)).normalize(),
		  right	= (rotation * Vec3f(1,0,0)).normalize();

	int pId=0;
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
	//if(pId == 0)
	//	return;

	missileType t = rockets.ammoRounds[rockets.max - rockets.left].type;
	Vec3f o = rockets.ammoRounds[rockets.max - rockets.left].offset;

	world.newObject(new missile(t, team, position+right*o.x+up*o.y+fwd*o.z, rotation, speed, id, pId));
	rockets.coolDownLeft=rockets.coolDown;
	rockets.left--;
}
void nPlane::dropBomb()
{
	if(bombs.coolDownLeft>0 || bombs.roundsLeft<=0 || controled)
		return;

	Vec3f fwd	= (rotation * Vec3f(0,0,1)).normalize(),
		  up	= (rotation * Vec3f(0,1,0)).normalize(),
		  right	= (rotation * Vec3f(1,0,0)).normalize();

	bombType t = bombs.ammoRounds[bombs.roundsMax-bombs.roundsLeft].type;
	Vec3f o = bombs.ammoRounds[bombs.roundsMax-bombs.roundsLeft].offset;

	world.newObject(new bomb(t, team, position+right*o.x+up*o.y+fwd*o.z, rotation, speed, id));
	bombs.coolDownLeft += bombs.coolDown;
	bombs.roundsLeft--;
}
void nPlane::loseHealth(float healthLoss)
{
	health-=healthLoss;
	if(health<0)
	{
		health=0;
		die();
	}
}
void nPlane::initArmaments()
{
	armament::ammo a;
	for(int i = 0; i < settings.planeStats[type].hardpoints.size(); i++)
	{
		a.type = settings.planeStats[type].hardpoints[i].mType;
		a.offset = settings.planeStats[type].hardpoints[i].offset;
		a.meshInstance = sceneManager.newMeshInstance(objectTypeString(a.type), position + rotation * a.offset, rotation);
		if(settings.planeStats[type].hardpoints[i].mType & MISSILE)
		{
			rockets.ammoRounds.push_back(a);
		}
		else if(settings.planeStats[type].hardpoints[i].mType & BOMB)
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
	bombs.roundsMax			= bombs.roundsLeft			= rockets.ammoRounds.size();
	bombs.coolDown			= bombs.coolDownLeft		= 100.0;
	bombs.rechargeTime		= bombs.rechargeLeft		= 8000.0f;
	bombs.firing										= false;

	machineGun.max			= machineGun.left			= 1000;
	machineGun.roundsMax	= machineGun.roundsLeft		= 200;
	machineGun.rechargeTime	= machineGun.rechargeLeft	= 450.0;
	machineGun.coolDown		= machineGun.coolDownLeft	= 26.0;
	machineGun.firing									= false;
}

void nPlane::spawn()
{
	initArmaments();
	position = startPos;
	rotation = startRot;

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
	speed=300.0;

	dead = false;
	controled=false;
	maneuver=0;
	death = DEATH_NONE;
	health=maxHealth;
	//updateAll(controlState());

	planePath.currentPoint(position,rotation);

	respawning=false;
	shotsFired = 0;

	smoothCamera(); //set up the camera

//	particleManager.addEmitter(new particle::planeContrail(id, Vec3f(7,0,-5)));
//	particleManager.addEmitter(new particle::planeContrail(id, Vec3f(-7,0,-5)));
}
