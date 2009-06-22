
#ifdef USING_XINPUT
Input *input=new xinput_input;
#else
Input *input=new standard_input;
#endif



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
			if(*axis*dir>0)
				return *axis*dir;
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
		my_char=0;
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
	void invalidate()
	{
		valid=false;
		my_char=-1;
	}
	bool operator== (const control c) const
	{
		if(integer!=c.integer)//if both are integers or keys
		{
			if(integer)
			{
				if(axis==c.axis && dir==c.dir)
					return true;
			}
			else
			{
				if(key==c.key)
					return true;
			}
		}
		return false;
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
			c[CON_CLIMB].newControl(&input->keys[VK_NUMPAD5],VK_NUMPAD5);
			c[CON_DIVE].newControl(&input->keys[VK_NUMPAD2],VK_NUMPAD2);
			c[CON_LEFT].newControl(&input->keys[VK_LEFT],VK_LEFT);
			c[CON_RIGHT].newControl(&input->keys[VK_RIGHT],VK_RIGHT);
			c[CON_ACCEL].newControl(&input->keys[VK_UP],VK_UP);
			c[CON_BRAKE].newControl(&input->keys[VK_DOWN],VK_DOWN);
			c[CON_SHOOT].newControl(&input->keys[VK_NUMPAD0],VK_NUMPAD0);
			c[CON_MISSILE].newControl(&input->keys[VK_NUMPAD9],VK_NUMPAD9);
			#ifdef USING_XINPUT
				xinput_input* x=(xinput_input*)input;
				if(x->g_Controllers[0].bConnected)
				{
					c[CON_CLIMB].newControl		(&input->axes[3][0]	,false);
					c[CON_DIVE].newControl		(&input->axes[3][0]	,true);
					c[CON_LEFT].newControl		(&input->axes[2][0]	,false);
					c[CON_RIGHT].newControl		(&input->axes[2][0]	,true);
					c[CON_ACCEL].newControl		(&input->axes[5][0]	,true);
					c[CON_BRAKE].newControl		(&input->axes[5][0]	,false);
					c[CON_SHOOT].newControl		(&input->axes[1][0]	,true);
					c[CON_MISSILE].newControl	(&input->axes[0][0]	,true);
				}
			#endif		
			//save_controls("saved controls/P1 default.ctrl"); //uncomment to update defaults
		}
		else if(p==2)
		{
			c[CON_CLIMB].newControl(&input->keys[0x55],0x55);
			c[CON_DIVE].newControl(&input->keys[0x4A],0x4A);
			c[CON_LEFT].newControl(&input->keys[0x41],0x41);
			c[CON_RIGHT].newControl(&input->keys[0x44],0x44);
			c[CON_ACCEL].newControl(&input->keys[0x57],0x57);
			c[CON_BRAKE].newControl(&input->keys[0x53],0x53);
			c[CON_SHOOT].newControl(&input->keys[0x20],0x20);
			c[CON_MISSILE].newControl(&input->keys[0x42],0x42);
			#ifdef USING_XINPUT
				xinput_input* x=(xinput_input*)input;
				if(x->g_Controllers[1].bConnected)
				{
					c[CON_CLIMB].newControl		(&input->axes[3][1]	,false);
					c[CON_DIVE].newControl		(&input->axes[3][1]	,true);
					c[CON_LEFT].newControl		(&input->axes[2][1]	,false);
					c[CON_RIGHT].newControl		(&input->axes[2][1]	,true);
					c[CON_ACCEL].newControl		(&input->axes[5][1]	,true);
					c[CON_BRAKE].newControl		(&input->axes[5][1]	,false);
					c[CON_SHOOT].newControl		(&input->axes[1][1]	,true);
					c[CON_MISSILE].newControl	(&input->axes[0][1]	,true);
				}
			#endif
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