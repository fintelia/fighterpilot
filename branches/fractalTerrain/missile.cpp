
#include "game.h"

missileBase::missileBase(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float Speed, int Owner):object(Type), life(15.0), difAng(0), lastAng(0), speed(Speed), acceleration(1180.0/3.0), owner(Owner)
{
	lastPosition = position = sPos;
	lastRotation = rotation = sRot;
	meshInstance = objectInfo[type]->newMeshInstance(Mat4f(rotation, position));
	collisionInstance = objectInfo[type]->newCollisionInstance(Mat4f(rotation, position));
}

void missileBase::updateFrame(float interpolation) const
{
	if(meshInstance)
	{
		meshInstance->update(Mat4f(slerp(lastRotation,rotation, interpolation), lerp(lastPosition,position,interpolation)), !awaitingDelete);
	}
}
missile::missile(missileType Type, teamNum Team,Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target):missileBase(Type, Team, sPos, sRot, speed, Owner), target(Target), launchTime(world.time()), engineStarted(false), contrailStarted(false), smallContrailStarted(false) 
{

}
void missile::updateSimulation(double time, double ms)
{
	if(awaitingDelete)
		return;

	if(!contrailStarted && time - launchTime >= 200.0)
	{
		contrailStarted = true;
		particleManager.addEmitter(new particle::contrail(),id);
	}
	if(!smallContrailStarted && time - launchTime >= 300.0)
	{
		smallContrailStarted = true;
		particleManager.addEmitter(new particle::contrailSmall(),id,Vec3f(0,0,-5.0));
	}
	lastPosition = position;
	lastRotation = rotation;

	/////////////////follow target////////////////////
	auto enemy = world[target];
	Vec3f destVec=rotation*Vec3f(0,0,1);
	if(enemy != NULL && !enemy->dead && engineStarted)
	{
		destVec = (enemy->position - position).normalize();

		if(enemy->type & PLANE)
		{
			Vec3f r = enemy->position - position;
			Vec3f v = enemy->rotation * Vec3f(0,0,dynamic_pointer_cast<plane>(enemy)->speed);
		
			float a = v.dot(v) - 1180.0*1180.0;
			float b = 2.0 * v.dot(r);
			float c = r.dot(r);
			float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);

			if(b*b - 4.0*a*c >= 0.0 && t >= 0.0)
			{
				destVec = (enemy->position + v * t - position).normalize();
			}
		}



		Vec3f fwd = rotation * Vec3f(0,0,1);

		Angle angle = acosA(destVec.dot(fwd));
	
		Quat4f targetRot(destVec);
	
		if( angle > PI*3/4)
		{
			target = 0;
		}
		else if(angle > 0.01)// we don't want to (or need to) divide by zero
		{
			float turnRate = PI * 1.5 * ms/1000 * (speed/1180.0);
			rotation = slerp(rotation,targetRot,(float)turnRate/angle);
		}


		//Vec3f newLineOfSight = (rotation.conjugate() * destVec).normalize();
		//Vec3f deltaLOS = newLineOfSight - lineOfSight;
		//float angle = newLineOfSight.dot(lineOfSight) / 1000.0 * world.time.length() * 1.0;
		//Vec3f axis = lineOfSight.cross(newLineOfSight);
		//rotation = rotation * Quat4f(axis, angle);

		//lineOfSight = newLineOfSight;
	}
	else if(enemy != NULL && enemy->dead)
	{
		particleManager.addEmitter(new particle::explosionSmoke(),position,2.0);
		life = 0.0;
	}

	//////////////////Movement//////////////
	if(!engineStarted && time - launchTime >= 150.0)
		engineStarted = true;
	
	if(engineStarted)
	{
		speed = min(speed + acceleration*(ms/1000), 1180.0);
		position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);
	}
	else
	{
		double lTime = (time-launchTime - ms)/1000.0;
		double cTime = (time-launchTime)/1000.0;

		position.y += 84.9 * (lTime*lTime - cTime*cTime);
		position += (rotation*Vec3f(0,0,1)) * speed * (ms/1000);
	}

	life-=ms/1000;
	if(life < 0.0 || world.altitude(position) < 0.0)
	{
		if(enemy != NULL && !enemy->dead && !(enemy->type & PLANE) && position.distance(enemy->position+enemy->rotation*enemy->meshInstance->getBoundingSphere().center) < 30.0)
		{
			enemy->loseHealth(105);
			if(enemy->dead)
			{
				if(players.numPlayers() >= 1 && owner==players[0]->objectNum()) players[0]->addKill();
				if(players.numPlayers() >= 2 && owner==players[1]->objectNum()) players[1]->addKill();
			}

		}
		if(world.altitude(position) < 0.0 && world.isLand(position.x, position.z))
		{
			particleManager.addEmitter(new particle::explosion(),position,5.0);
			particleManager.addEmitter(new particle::groundExplosionFlash(),position,5.0);
		}
		awaitingDelete = true;
		meshInstance.reset();
	}

	if(collisionInstance)
	{
		collisionInstance->update(Mat4f(rotation,position), Mat4f(lastRotation,lastPosition));
	}
}
SAMmissile::SAMmissile(missileType Type, teamNum Team, Vec3f sPos, Quat4f sRot, float speed, int Owner, int Target): missileBase(Type, Team, sPos, sRot, speed, Owner), target(Target)
{
	minAngle = 2.0*PI;
}
void SAMmissile::init()
{
	particleManager.addEmitter(new particle::contrail(),id);
	particleManager.addEmitter(new particle::contrailSmall(),id,Vec3f(0,0,-5.0));
}
void SAMmissile::updateSimulation(double time, double ms)
{
	lastPosition = position;
	lastRotation = rotation;
	/////////////////follow target////////////////////
	auto enemy = world[target];
	Vec3f destVec=rotation*Vec3f(0,0,1);
	if(enemy != NULL && !enemy->dead)
	{
		destVec = (enemy->position - position).normalize();

		if(enemy->type & PLANE)
		{
			Vec3f r = enemy->position - position;
			Vec3f v = enemy->rotation * Vec3f(0,0,dynamic_pointer_cast<plane>(enemy)->speed);
		
			float a = v.dot(v) - 1180.0*1180.0;
			float b = 2.0 * v.dot(r);
			float c = r.dot(r);
			float t = (-b - sqrt(b*b - 4.0*a*c)) / (2.0 * a);

			if(b*b - 4.0*a*c >= 0.0 && t >= 0.0)
			{
				destVec = (enemy->position + v * t - position).normalize();
			}
		}

		Vec3f fwd = rotation * Vec3f(0,0,1);
		
		float angle = acos(destVec.dot(fwd));
		Quat4f targetRot(destVec);
		
		minAngle = min(angle,minAngle);
		
		if(angle > PI*3/4 || angle > minAngle + PI/12)
		{
			target = 0;
		}
		else if(angle > 0.01)// we don't want to (or need to) divide by zero
		{
			rotation = slerp(rotation,targetRot,(float)((PI * 0.9 * ms/1000)/angle));
		}
	}
	else if(enemy != NULL && enemy->dead)
	{
		particleManager.addEmitter(new particle::explosionSmoke(),position,2.0);
		life = 0.0;
	}
	//////////////////Movement//////////////
	speed = min(speed + acceleration*(ms/1000), 500.0);
	position += (rotation*Vec3f(0,0,1)) * speed *(ms/1000);

	life-=ms/1000;

	if(collisionInstance)
	{
		collisionInstance->update(Mat4f(rotation,position), Mat4f(lastRotation,lastPosition));
	}
//	if(life < 0.0 || world.altitude(position) < 0.0)
//	{
//		awaitingDelete = true;
//		meshInstance->setDeleteFlag(true);
//		meshInstance = nullptr;
//	}
}
