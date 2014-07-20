
class SoundManager
{
public:
	class sound
	{
	private:
		unsigned int soundID;
		void* dsSecondaryBuffer; //IDirectSoundBuffer8*
		string filename;
		friend class SoundManager;
	public:
		bool loop;
	};
	class soundInstance
	{
	private:
		unsigned int soundID;
	public:
		Vec3f position;
		Vec3f velocity;
	};

private:
	map<string, shared_ptr<sound>> sounds;
	map<string, weak_ptr<soundInstance>> soundInstances;

	void* dsDevice; //IDirectSound8*
	void* dsPrimaryBuffer; //IDirectSoundBuffer*

	SoundManager(): dsDevice(nullptr){}
public:
	static SoundManager& getInstance()
	{
		static SoundManager* pInstance = new SoundManager();
		return *pInstance;
	}

	bool initialize();
	void loadSound(string name, string filename, bool loop);
	shared_ptr<soundInstance> newSoundInstance();
	void playSound(string name);
	void updateFrame();
	~SoundManager();
};

typedef shared_ptr<SoundManager::soundInstance> soundInstancePtr;

extern SoundManager& soundManager;