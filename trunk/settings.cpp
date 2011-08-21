
#include "main.h"
void ObjectStats::load(string filename)
{
	string line;
	ifstream fin(filename, ios::in);

	planeType	cP	= F18;//default to f18
	missileType	cMissileType=MISSILE1;

	//int			cH	= -1,//hardpoint #
	bool		mirror = false;

	char_separator<char> sep(" ,/", "");
		
	tokenizer<char_separator<char> >::iterator t;

	if(fin.is_open())
	{
		while(!fin.eof())
		{
			getline(fin,line);
			if(line.find_first_of("=")==line.npos || line.find_first_not_of(" \t")==line.npos) continue;

			line=line.substr(line.find_first_not_of(" \t"),line.npos);
			unsigned int f=line.find_first_of("=");
			string var = line.substr(0,f);

			unsigned int comment=line.find_first_of("'#");
			string value = line.substr(f+1,comment-f-1);


			try
			{
				//planeModels.push_back(load_model(lexical_cast<string>(value).c_str()));
				if(var.compare("PLANE") == 0)
				{
					if(value == "F12") 		cP=F12;
					else if(value == "F16")	cP=F16;
					else if(value == "F18")	cP=F18;
					else if(value == "F22")	cP=F22;
					else if(value == "UAV")	cP=UAV;
					else if(value == "B2")	cP=B2;
					else cP=(planeType)lexical_cast<int>(value);
				}
				else if(var.compare("DEFAULT") == 0)
				{
					if(value == "F12")		defaultPlane=F12;
					else if(value == "F16")	defaultPlane=F16;
					else if(value == "F18")	defaultPlane=F18;
					else if(value == "F22")	defaultPlane=F22;
					else if(value == "UAV")	defaultPlane=UAV;
					else if(value == "B2")	defaultPlane=B2;
					else defaultPlane=(planeType)lexical_cast<int>(value); // Try to avoid
					mirror=false;
				}
				//else if(var == "MISSILE")
				//{
				//	missileStats.push_back(missileStat());
				//	missileStats.back().dispList=dataManager.loadModel(value);//objModel::objDisplayList((char*)lexical_cast<string>(value).c_str());
				//	cMissileType++;
				//}
				else if(var == "CURRENT_WEAPON")
				{
					if(value == "MISSILE1")			cMissileType=MISSILE1;
					else if(value == "MISSILE2")	cMissileType=MISSILE2;
					else if(value == "MISSILE3")	cMissileType=MISSILE3;
					else if(value == "MISSILE4")	cMissileType=MISSILE4;
					else if(value == "BOMB1")		cMissileType=BOMB1;
					else if(value == "BOMB2")		cMissileType=BOMB2;
				}
				else if(var.compare("W_OFFSET") == 0 && cMissileType != -1)
				{
					Vec3f v;
					int d = 0;

					tokenizer<char_separator<char> > Tokenizer(value, sep);
					for (t = Tokenizer.begin(); t != Tokenizer.end(); t++, d++)
					{
						v[d]=lexical_cast<float>(*t);
					}

					planeStats[cP].hardpoints.push_back(planeStat::hardpoint(cMissileType,v));
					if(mirror)	
						planeStats[cP].hardpoints.push_back(planeStat::hardpoint(cMissileType, Vec3f(-v.x,v.y,v.z)));
				}
				else if(var.compare("G_OFFSET") == 0)
				{
					Vec3f v;
					int d = 0;

					tokenizer<char_separator<char> > Tokenizer(value, sep);
					for (t = Tokenizer.begin(); t != Tokenizer.end(); t++, d++)
					{
						v[d]=lexical_cast<float>(*t);
					}

					planeStats[cP].machineGuns.push_back(v);
					if(mirror)	
						planeStats[cP].machineGuns.push_back(-v);
				}
				else if(var.compare("MIRROR") == 0)
				{
					mirror = (value == "TRUE");
				}
			}
			catch(bad_lexical_cast &)
			{
				debugBreak();//boost type conversion failed
				closingMessage("Error reading 'modelData.txt'. Fighter-Pilot will now close.","Error");
				return;
			}
		}
		fin.close();
	}
	else 
	{
		debugBreak();//could not open model data file
		closingMessage("'modelData.txt' not found. Fighter-Pilot will now close.","Missing File");
	}
}
//void ObjectStats::loadModelData(char* filename)
//{
//	string line;
//	ifstream fin(filename, ios::in);
//
//	planeType	cP=F18;//default to f18
//	int			cH=-1,//hardpoint #
//				cM=-1;//current missile #
//	bool		hMirror=false;
//
//	string		s;	
//	char_separator<char> sep(" ,/", "");
//		
//	tokenizer<char_separator<char> >::iterator t;
//
//	if(fin.is_open())
//	{
//		while(!fin.eof())
//		{
//			getline(fin,line);
//			if(line.find_first_of("=")==line.npos || line.find_first_not_of(" \t")==line.npos) continue;
//
//			line=line.substr(line.find_first_not_of(" \t"),line.npos);
//			unsigned int f=line.find_first_of("=");
//			string var = line.substr(0,f);
//
//			unsigned int comment=line.find_first_of("'#");
//			string value = line.substr(f+1,comment-f-1);
//
//
//			try
//			{
//				//planeModels.push_back(load_model(lexical_cast<string>(value).c_str()));
//				if(var.compare("PLANE") == 0)
//				{
//					if(value.compare("F12")==0) 				
//						cP=F12;
//					else if(value.compare("F16")==0)		
//						cP=F16;
//					else if(value.compare("F18")==0)		
//						cP=F18;
//					else if(value.compare("F22")==0)			
//						cP=F22;
//					else if(value.compare("UAV")==0)			
//						cP=UAV;
//					else if(value.compare("B2")==0)	
//						cP=B2;
//					else
//						cP=(planeType)lexical_cast<int>(value); // Try to avoid
//				}
//				else if(var.compare("DEFAULT") == 0)
//				{
//					if(value.compare("F12")==0) 				
//						defaultPlane=F12;
//					else if(value.compare("F16")==0)		
//						defaultPlane=F16;
//					else if(value.compare("F18")==0)		
//						defaultPlane=F18;
//					else if(value.compare("F22")==0)			
//						defaultPlane=F22;
//					else if(value.compare("UAV")==0)			
//						defaultPlane=UAV;
//					else if(value.compare("B2")==0)	
//						defaultPlane=B2;
//					else
//						defaultPlane=(planeType)lexical_cast<int>(value); // Try to avoid
//					hMirror=false;
//					cH=-1;
//				}
//				//else if(var.compare("MODEL") == 0)
//				//{
//				//	//int mId=objModel::objDisplayList((char*)lexical_cast<string>(value).c_str());//load_model((char*)lexical_cast<string>(value).c_str());
//				//	int mId=dataManager.loadModel(value);
//				//	planeModels.insert(pair<planeType,int>(cP,mId));
//				//	planeStats[cP].dispList=mId;
//				//	hMirror=false;
//				//	cH=-1;
//				//}
//				//else if(var.compare("MISSILE") == 0)
//				//{
//				//	cM++;
//				//	missileStats.push_back(missileStat());
//				//	missileStats[cM].dispList=dataManager.loadModel(value);//objModel::objDisplayList((char*)lexical_cast<string>(value).c_str());
//				//	int i=4;
//				//}
//				//else if(var.compare("M_MISSILE") == 0)
//				//{
//				//	cH++;
//				//	if(hMirror) cH++;
//				//	planeStats[cP].hardpoints.push_back(planeStat::hardpoint());
//				//	planeStats[cP].hardpoints[cH].missileNum=lexical_cast<int>(value);
//				//	hMirror=false;
//				//}
//				else if(var.compare("M_OFFSET") == 0 && cH != -1)
//				{
//					int d = 0;
//					tokenizer<char_separator<char> > Tokenizer(value, sep);
//					for (t = Tokenizer.begin(); t != Tokenizer.end(); t++, d++)
//					{
//						s=*t;
//						planeStats[cP].hardpoints[cH].offset[d]=lexical_cast<float>(*t);
//					}
//					if(hMirror)
//					{
//						planeStats[cP].hardpoints[cH+1].offset=planeStats[cP].hardpoints[cH].offset;
//						planeStats[cP].hardpoints[cH+1].offset.x *= -1.0;
//					}
//				}
//				else if(var.compare("MIRROR") == 0 && cH != -1)
//				{
//					hMirror= (value == "TRUE");
//					if(hMirror)
//					{
//						planeStats[cP].hardpoints.push_back(planeStat::hardpoint(planeStats[cP].hardpoints[cH]));
//						planeStats[cP].hardpoints[cH+1].offset.x *= -1.0;
//					}
//				}
//				//else if(var.compare("MIRROR") == 0 && cm != -1)
//				//{
//				//	missileStats[cm].mirror=value.compare("TRUE")==0;
//				//}
//				//else if (var.compare("NAME") == 0)
//				//{
//				//	while(value.find("_")!=value.npos)
//				//		value[value.find("_")]=' ';
//				//	getInstance()->units[ct].name		=value;
//				//}
//			}
//			catch(bad_lexical_cast &)
//			{
//				debugBreak();//boost type conversion failed
//				closingMessage("Error reading 'modelData.txt'. Fighter-Pilot will now close.","Error");
//				return;
//			}
//		}
//		fin.close();
//	}
//	else 
//	{
//		debugBreak();//could not open model data file
//		closingMessage("'modelData.txt' not found. Fighter-Pilot will now close.","Missing File");
//	}
//}