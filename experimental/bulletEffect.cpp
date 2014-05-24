
#include "game.h"

namespace particle
{
	bulletEffect::bulletEffect(): sparkEmitter("bullet", 0)
	{

	}
	void bulletEffect::prepareRender(Vec3f up, Vec3f right)
	{
		shared_ptr<bulletCloud> obj = dynamic_pointer_cast<bulletCloud>(world[parentObject]);

		double time = world.time();
		double lTime = time - 20.0;

		int n;
		Vec3f start, end, dir;
		float a1, a2, len;

		if(obj->bullets.size() > compacity)
		{
			unsigned int tmpCompacity = obj->bullets.size() + 100;
			vertex *tmpVertices = new vertex[tmpCompacity*4];
			memset(tmpVertices,0,tmpCompacity*sizeof(vertex)*4);
		
			memcpy(tmpVertices,vertices,compacity*sizeof(vertex)*4);
			delete[] vertices;
			vertices = tmpVertices;
			compacity = tmpCompacity;
		}


		vNum=0;
		for(auto i = obj->bullets.begin(); i != obj->bullets.end(); i++)
		{
			for(n = 0; n<4; n++)
			{
				vertices[vNum*4 + n].r = 1.0;
				vertices[vNum*4 + n].g = 1.0;
				vertices[vNum*4 + n].b = 0.4;
				vertices[vNum*4 + n].a = 0.5;
			}

			start = i->startPos+i->velocity*(time-i->startTime)*0.001f;
			end = i->startPos+i->velocity*max(lTime-i->startTime,0.0f)*0.001f;

			dir = (end - start).normalize();
			a1 = dir.dot(up);
			a2 = dir.dot(right);
			len = 0.3/sqrt(a1*a1+a2*a2);

			vertices[vNum*4 + 0].position = start + (right*a1 - up*a2)*len;
			vertices[vNum*4 + 1].position = start - (right*a1 - up*a2)*len;
			vertices[vNum*4 + 2].position = end - (right*a1 - up*a2)*len;
			vertices[vNum*4 + 3].position = end + (right*a1 - up*a2)*len;


			vertices[vNum*4 + 0].s = 1.0;	vertices[vNum*4 + 0].t = 0.0;
			vertices[vNum*4 + 1].s = 1.0;	vertices[vNum*4 + 1].t = 1.0;
			vertices[vNum*4 + 2].s = 0.0;	vertices[vNum*4 + 2].t = 1.0;
			vertices[vNum*4 + 3].s = 0.0;	vertices[vNum*4 + 3].t = 0.0;

			vNum++;
		}

		VBO->setVertexData(sizeof(vertex)*vNum*4, vertices);
	}
}
