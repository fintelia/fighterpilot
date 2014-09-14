
#include "engine.h"

#ifdef WINDOWS
	#include <windows.h>
	#include <dsound.h>
#endif

#if defined(WINDOWS)
struct SoundManager::SoundInterface{
	void* dsDevice; //IDirectSound8*
	void* dsPrimaryBuffer; //IDirectSoundBuffer*	
};
SoundManager(): interface(new SoundInterface)
{
	interface->dsDevice = nullptr;
	interface->dsPrimaryBuffer = nullptr;
}

bool SoundManager::initialize()
{
	if(FAILED(DirectSoundCreate8(NULL, (IDirectSound8**)&dsDevice, NULL)))
		return false;

	if(FAILED(static_cast<IDirectSound8*>(dsDevice)->SetCooperativeLevel(graphics->getWindowHWND(), DSSCL_PRIORITY)))
		return false;

	// Setup the primary buffer description.
	//DSBUFFERDESC bufferDesc;
	//bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	//bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	//bufferDesc.dwBufferBytes = 0;
	//bufferDesc.dwReserved = 0;
	//bufferDesc.lpwfxFormat = NULL;
	//bufferDesc.guid3DAlgorithm = GUID_NULL;
	//
	//if(FAILED(static_cast<IDirectSound8*>(dsDevice)->CreateSoundBuffer(&bufferDesc, (IDirectSoundBuffer*)&dsPrimaryBuffer, NULL)))
	//	return false;



	//Success!!!

	return true;
}
void SoundManager::playSound(string name)
{
	auto s = sounds.find(name);
	if(s != sounds.end())
	{
		PlaySoundA(nullptr, nullptr, 0);
		if(s->second->loop)
			PlaySoundA(s->second->filename.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);
		else
			PlaySoundA(s->second->filename.c_str(), nullptr, SND_FILENAME | SND_ASYNC);
	}
}
SoundManager::~SoundManager()
{
	if(dsDevice)
	{
		IDirectSound_Release((IDirectSound8*)dsDevice); 
	}
}

#elif defined(LINUX)
struct SoundManager::SoundInterface{};
SoundManager::SoundManager(): interface(new SoundInterface)
{

}
bool SoundManager::initialize()
{
	//code not written...
	return false;
}
void SoundManager::playSound(string name)
{
	//code not written...
}
SoundManager::~SoundManager()
{
	//code not written...
}
#endif

void SoundManager::loadSound(string name, string filename, bool loop)
{
	auto nSound = std::make_shared<sound>();
	nSound->filename = filename;
	nSound->loop = loop;
	nSound->dsSecondaryBuffer = nullptr;
	sounds[name] = nSound;
}
shared_ptr<SoundManager::soundInstance> SoundManager::newSoundInstance()
{
	return shared_ptr<soundInstance>();
}

void SoundManager::updateFrame()
{

}
