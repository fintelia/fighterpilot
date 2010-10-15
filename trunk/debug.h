
//#define NDEBUG //asserts

//#define PLANE_ID_BOXES

//#define AI_TARGET_LINES




//#ifdef PLANE_ID_BOXES                    //insert in modes.h dogFight::drawOrthoView
//		glLineWidth(3);
//		for(map<int,planeBase*>::iterator i = planes.begin(); i != planes.end();i++)
//		{
//			if(p.id!=(*i).second->id && 
//				(frustum.sphereInFrustum(Vec3f((*i).second->x,(*i).second->y,(*i).second->z),8) != FrustumG::OUTSIDE))
//			{
//				tar.set((*i).second->x-p.x,(*i).second->y-p.y,(*i).second->z-p.z);
//				tar=tar.normalize();
//				length=1.0/fwd.dot(tar);
//				tar=tar*length;
//				tar+=Vec3f(p.x,p.y,p.z);
//				float tX=dist_Point_to_Segment(tar,frustum.ntl,frustum.nbl)/dist(frustum.ntl,frustum.ntr);
//				float tY=dist_Point_to_Segment(tar,frustum.ntl,frustum.ntr)/dist(frustum.ntl,frustum.nbl);
//				if(i->second->id<=1)
//					glColor3f(0,1,0);
//				else if(i->second->dead)
//					glColor3f(1,0,0);
//				else
//					glColor3f(0,0,0);
//				glBegin(GL_LINE_LOOP);
//					glVertex2f(tX*(float)sw-6,tY*(float)sh*0.5-6);
//					glVertex2f(tX*(float)sw+6,tY*(float)sh*0.5-6);
//					glVertex2f(tX*(float)sw+6,tY*(float)sh*0.5+6);
//					glVertex2f(tX*(float)sw-6,tY*(float)sh*0.5+6);
//				glEnd();
//			}
//		}
//		glLineWidth(1);
//		glColor4f(1,1,1,1);
//#endif
