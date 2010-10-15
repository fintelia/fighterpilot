
#include <vector>
#include <map>
using namespace std;

#include "enums.h"
#include "gameMath.h"
#include "Terain.h"
#include "settings.h"
Terrain::Terrain()
{
	w=0;
	l=0;
	computedNormals=false;
}
Terrain::Terrain(int w2, int l2) 
{
	int i;
	w = w2;
	l = l2;
	float f=0;
	vector<float> v;
	for(int j=0;j<w;j++)
		v.push_back(f);
	for(i=0;i<l;i++)
		hs.push_back(v);
	/*
	hs = new float*[l];
	for(i = 0; i < l; i++) {
		hs[i] = new float[w];
	}
	*/
	Vec3f h(0,0,0);
	vector<Vec3f> k;
	for(int j=0;j<w;j++)
		k.push_back(h);
	for(i=0;i<l;i++)
		normals.push_back(k);
	//normals = new Vec3f*[l];
	//for(i = 0; i < l; i++) {
	//	normals[i] = new Vec3f[w];
	//}
	computedNormals = false;
}

int Terrain::width() {
	return w;
}
	
int Terrain::length() {
	return l;
}
	
//Sets the height at (x, z) to y
void Terrain::setHeight(int x, int z, float y) {
	if(l==0 || w==0) return;
	hs[z][x] = y;
	computedNormals = false;
}
	
//Returns the height at (x, z)
float Terrain::getHeight(int x, int z) {
	if(l==0 || w==0) return 0;
	if(x>=l)x=l-1;
	if(z>=w)z=w-1;
	if(x<0)x=0;
	if(z<0)z=0;
	return hs[z][x];
}
	
//Computes the normals, if they haven't been computed yet
void Terrain::computeNormals() {
	if (computedNormals) {
		return;
	}
	int i;			
	//Compute the rough version of the normals
	Vec3f** normals2 = new Vec3f*[l];
	for(i = 0; i < l; i++) {
		normals2[i] = new Vec3f[w];
	}
	int z;

	for(z = 0; z < l; z++) {
		for(int x = 0; x < w; x++) {
			Vec3f sum(0.0f, 0.0f, 0.0f);
				
			Vec3f out;
			if (z > 0) {
				out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
			}
			Vec3f in;
			if (z < l - 1) {
				in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
			}
			Vec3f left;
			if (x > 0) {
				left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
			}
			Vec3f right;
			if (x < w - 1) {
				right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
			}
				
			if (x > 0 && z > 0) {
				sum += out.cross(left).normalize();
			}
			if (x > 0 && z < l - 1) {
				sum += left.cross(in).normalize();
			}
			if (x < w - 1 && z < l - 1) {
				sum += in.cross(right).normalize();
			}
			if (x < w - 1 && z > 0) {
				sum += right.cross(out).normalize();
			}
				
			normals2[z][x] = sum;
		}
	}
		
	//Smooth out the normals
	const float FALLOUT_RATIO = 0.5f;
	for(z = 0; z < l; z++) {
		for(int x = 0; x < w; x++) {
			Vec3f sum = normals2[z][x];
				
			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < w - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < l - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}
				
			if (sum.magnitude() == 0) {
				sum = Vec3f(0.0f, 1.0f, 0.0f);
			}
			normals[z][x] = sum;
		}
	}
		
	for(i = 0; i < l; i++) {
		delete[] normals2[i];
	}
	delete[] normals2;
		
	computedNormals = true;
}
	
//Returns the normal at (x, z)
Vec3f Terrain::getNormal(int x, int z) {
	if (!computedNormals) {
		computeNormals();
	}
	if(x>=l)x=l-1;
	if(z>=w)z=w-1;
	if(x<0)x=0;
	if(z<0)z=0;
	return normals[z][x];
}
float Terrain::getInterpolatedHeight(float x, float z)
{
	float sx=floor(x);
	float sz=floor(z);
	Vec3f down(0,-1,0);
	Vec3f vert;

	Vec3f v1(sx+1,getHeight((int)sx+1,(int)sz),sz);
	Vec3f v2(sx,getHeight((int)sx,(int)sz+1),sz+1);
	Vec3f v3;
	//Vec3f v1((tmp1-tmp2));
	Vec3f normal;
	if((x-sx)*(x-sx)+(z-sz)*(z-sz)<=(x-(sx+1))*(x-(sx+1))+(z-(sz+1))*(z-(sz+1)))
	{
		v3.set(sx,getHeight((int)sx,(int)sz),sz);

		float h=((v3*(1.0f-(x-sx))+v1*(x-sx))*(1.0f-(z-sz))+v2*(z-sz)).y;
		if(h<settings.SEA_LEVEL)h=settings.SEA_LEVEL;
		return h;

		//normal=((v3-v2).cross(v3-v1)).normalize();

		//vert[0]=sx;
		//vert[1]=getHeight(sx,sz);
		//vert[2]=sz;

		//return intersect(point,down,vert,normal);
	}
	else
	{
		v3.set(sx+1,getHeight((int)sx+1,(int)sz+1),sz+1);
		float h=((v3*(z-sz)+v1*(1.0f-(z-sz)))*(x-sx)+v2*(1.0f-(x-sx))).y;
		if(h<settings.SEA_LEVEL)h=settings.SEA_LEVEL;
		return h;

		//normal=(v3-v2).cross(v3-v1).normalize();

		//vert[0]=sx+1;
		//vert[1]=getHeight(sx+1,sz+1);
		//vert[2]=sz+1;
		//return intersect(point,down,vert,normal);
	}
}