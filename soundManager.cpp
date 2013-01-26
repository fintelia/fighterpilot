
#include "engine.h"
#include <windows.h>
#include <dsound.h>


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
void SoundManager::addSound()
{

}
shared_ptr<SoundManager::soundInstance> SoundManager::newSoundInstance()
{
	return shared_ptr<soundInstance>();
}
void SoundManager::updateFrame()
{

}
SoundManager::~SoundManager()
{
	if(dsDevice)
	{
		IDirectSound_Release((IDirectSound8*)dsDevice); 
	}
}