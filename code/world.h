class World
{
public:
	Terrain *terrain;
	vector<bullet> bullets;
	vector<missile> missiles;
	vector<spark> fire;
	vectot<spark> smoke;
	map<int,planeBase*> planes;

	void update()
	{
		vector<int> toDelete;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			if(!(*i).second->Update(value))
			{
				toDelete.push_back((*i).first);
			}
		}
		for(vector<int>::iterator i=toDelete.begin();i!=toDelete.end();i++)
		{
			int l=planes.erase(*i);
			l=l;
		}
	//------------------------------------------------------------------------//
		Vec3f b1;
		Vec3f b2;
		Vec3f p;
		int l;
		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
		{
			p[0]=(*i).second->x;p[1]=(*i).second->y;p[2]=(*i).second->z;
			for(l=0;l<(signed int)bullets.size();l++)
			{
				b1[0]=bullets[l].x;b1[1]=bullets[l].y;b1[2]=bullets[l].z;
				if(dist(p,b1)<32 && bullets[l].owner != (*i).first)
				{
					(*i).second->die();
					bullets.erase(bullets.begin()+l);
					l--;
				}
			}
			for(l=0;l<(signed int)missiles.size();l++)
			{
				b1[0]=missiles[l].x;b1[1]=missiles[l].y;b1[2]=missiles[l].z;
				if(dist(p,b1)<32 && missiles[l].owner != (*i).first)
				{
					(*i).second->die();
					missiles.erase(missiles.begin()+l);
					l--;
				}
			}
		}
		for(int i=0;i<(signed int)bullets.size();i++)
		{
			if(!bullets[i].update((float)value/33.0))
				bullets.erase(bullets.begin()+i);
		}
		for(int i=0;i<(signed int)missiles.size();i++)
		{
			if(!missiles[i].Update((float)value/33.0))
				missiles.erase(missiles.begin()+i);
		}
		for(int i=0;i<(signed int)fire.size();i++)
		{
			if(!fire[i].update((float)value/33.0))
				fire.erase(fire.begin()+i);
		}
		for(int i=0;i<(signed int)smoke.size();i++)
		{
			if(!smoke[i].update((float)value/33.0))
				smoke.erase(smoke.begin()+i);
		}
	}
	void clear()
	{
		terrain=NULL;
		planes.clear();
		bullets.clear();
		missiles.clear();
		fire.clear();
		smoke.clear();
	}
	World(){}
};