//
//#include <string>
//#include <vector>
//using namespace std;
//
//#include "gameMath.h"
//#include "Terain.h"
//#include "gridFloat.h"
#include "main.h"

bool gridFloatTerrain::load(string filename)
{
	string line;
	ifstream fin((filename+".hdr").c_str(), ios::in);
	if(!fin.is_open())	
		return false;
	////read header/////////////////////
	getline(fin,line);	w=lexical_cast<int>(line.substr(line.find_last_of(" ")+1,line.size()));
	getline(fin,line);	l=lexical_cast<int>(line.substr(line.find_last_of(" ")+1,line.size()));
	fin.close();
	//read data/////////////////////
	float* data=(float*)malloc(4*w*l);

	ifstream fin2((filename+".flt").c_str(), ios::in | ios::binary);
	if(!fin2.is_open())	
		return false;
	fin2.read((char*)data,4*w*l);
	fin2.close();
	////create vectors/////////////////////
	hs=vector<vector<float>>(l,vector<float>(w,-1));
	normals=vector<vector<Vec3f>>(l,vector<Vec3f>(w,Vec3f(0,0,0)));
	////transfer data into vectors
	float minV=data[0];
	float maxV=data[0];
	for(int i=1;i<w*l;i++)
	{
		if(data[i]<minV)
			minV=data[i];
		if(data[i]>maxV)
			maxV=data[i];
	}
	for(int y=0;y<l;y++)
	{
		for(int x=0;x<w;x++)
		{
			hs[x][y]=(data[x+y*w]-minV)*500/(maxV-minV);
		}
	}
	free(data);
	computedNormals=false;
	return true;
}