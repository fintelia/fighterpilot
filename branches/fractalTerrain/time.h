
double GetTime();

//const extern double UPDATE_LENGTH;

class GameTime
{
private:
	double timeSpeed;				//time speed in    ms(gametime) / ms(realtime)
	int paused;						//equal to 0 if time is paused, greater than 0 if time is not paused

	long long lReal;				//ticks at last frame
	double lGame;					//time in ms at last frame

	long long cReal;				//ticks at current frame
	double cGame;					//ticks in ms at current frame

	bool changingSpeed;				//whether we are changing the time speed
	double changeTimeLeft;			//how much real time we have left to do it (in ms)
	double finalTimeSpeed;			//what the final time speed will be

	long long ticksPerSecond;		//how many clock ticks occur each second
	double trueGameTime() const;	//get what the game time would be if a new frame were started now

	bool updateStage;				//whether we are in the fixed time step update stage, rather than the variable time step frame stage
	double cUpdateTime;				//what the current update stage time is
	double lUpdateTime;				//what the last update stage time was
	double updateLength;			//how much game time there will be between the current current update stage time and the next update stage time

public:
	GameTime();
	void reset();
	void nextUpdate();
	void nextFrame();

	void changeSpeed(double speed, double changeTime=0.0); //changeTime = amount of time to change between current speed and new speed in real ms

	void pause();
	void unpause();
	void setPaused(bool b);
	bool isPaused() const;
	double length() const;
	double lastTime() const;
	double time() const;
	double operator() () const;
	bool needsUpdate() const;
	double interpolate() const;
	double getSpeed() const;
	double getUpdateLength() const;
	void setUpdateLength(double length);
};
