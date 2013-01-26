
class SoundManager
{
public:
	class sound
	{
		unsigned int soundID;
		void* dsSecondaryBuffer; //IDirectSoundBuffer8*
	};
	class soundInstance
	{
		unsigned int soundID;
		Vec3f position;
		Vec3f velocity;
	};

private:
	map<unsigned int, sound> sounds;
	map<unsigned int, weak_ptr<soundInstance>> soundInstances;

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
	void addSound();
	shared_ptr<soundInstance> newSoundInstance();
	void updateFrame();
	~SoundManager();
};

typedef shared_ptr<SoundManager::soundInstance> soundInstancePtr;

extern SoundManager& soundManager;