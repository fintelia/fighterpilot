
const int CON_CLIMB=0;
const int CON_DIVE=1;
const int CON_RIGHT=2;
const int CON_LEFT=3;
const int CON_ACCEL=4;
const int CON_BRAKE=5;
const int CON_SHOOT=6;
const int CON_MISSILE=7;

class control
{
private:
	bool valid;
	bool integer;
	int dir;
	int *axis;
	bool *key;
public:
	unsigned char my_char;
	control(int *a,bool positive)
	{
		valid=true;
		integer=true;
		axis=a;
		if(positive)
			dir=1;
		else
			dir=-1;
	}
	control(bool *k,unsigned char c)
	{
		integer=false;
		key=k;
		valid=true;
		my_char=c;
	}
	control()
	{
		integer=false;
		dir=0;
		valid=false;
	}
	int value()
	{
		if(!valid)
			return 0;
		if(integer)
		{
			if(*axis>0)
				return *axis;
			else 
				return 0;
		}
		else
		{
			if(*key)
				return 1000;
			else 
				return 0;
		}
	}
	void newControl(int *a,bool positive)
	{
		valid=true;
		integer=true;
		axis=a;
		if(positive)
			dir=1;
		else
			dir=-1;
	}
	void newControl(bool *k,unsigned char c)
	{
		valid=true;
		integer=false;
		key=k;
		my_char=c;
	}
};



class playerControls
{
private:

public:
	control c[8];
	bool load_controls(char *filename)
	{
		ifstream myfile (filename);
		if (myfile.is_open())
		{
			for(int i=0;i<8;i++)
			{
				myfile.read((char *)(&c[i]),sizeof(c[i]));
			}
			myfile.close();
		}
		else
		{
			return false;
		}
	}
	bool save_controls(char *filename)
	{
		ofstream myfile (filename);
		if (myfile.is_open())
		{
			for(int i=0;i<8;i++)
			{
				myfile.write((char *)(&c[i]),sizeof(c[i]));
			}
			myfile.close();
		}
		else
		{
			return false;
		}
	}

	playerControls(){}
	playerControls(int p)//default controls for player p
	{
		if(p==1)
		{
			c[CON_CLIMB].newControl(&input->keys[53],53);
			c[CON_DIVE].newControl(&input->keys[50],50);
			c[CON_LEFT].newControl(&input->keys[230],230);
			c[CON_RIGHT].newControl(&input->keys[228],228);
			c[CON_ACCEL].newControl(&input->keys[229],229);
			c[CON_BRAKE].newControl(&input->keys[231],231);
			c[CON_SHOOT].newControl(&input->keys[48],48);
			c[CON_MISSILE].newControl(&input->keys[57],57);
			//save_controls("saved controls/P1 default.ctrl"); //uncomment to update defaults
		}
		else if(p==2)
		{
			c[CON_CLIMB].newControl(&input->keys[117],117);
			c[CON_DIVE].newControl(&input->keys[106],106);
			c[CON_LEFT].newControl(&input->keys[100],100);
			c[CON_RIGHT].newControl(&input->keys[97],97);
			c[CON_ACCEL].newControl(&input->keys[119],119);
			c[CON_BRAKE].newControl(&input->keys[115],115);
			c[CON_SHOOT].newControl(&input->keys[32],32);
			c[CON_MISSILE].newControl(&input->keys[98],98);
			//save_controls("saved controls/P2 default.ctrl"); //uncomment to update defaults
		}
	}
	control getCotrol(int con)
	{
		
		if(con>=0 && con<8)
			return c[con];
		else
		{
			control tmp;
			return tmp;
		}
	}
	void setControl(int con,control k)
	{
		if(con>=0 && con<8)
			c[con]=k;
	}
	int playerControls::operator[](int index) {
		return c[index].value();
	}

	//int playerControls::operator[](int index) const {
	//	return getCotrol(index).value();
	//}
};

class Controls
{
private:
	vector<playerControls> pControls;
public:
	Controls(int numPlayers)
	{
		for(int i=0;i<numPlayers;i++)
		{
			playerControls p(i+1);
			pControls.push_back(p);
		}
	}
	playerControls &Controls::operator[](int index) {
		return pControls[index];
	}

	//playerControls Controls::operator[](int index) const {
	//	return pControls[index];
	//}
};

Controls controls(2);