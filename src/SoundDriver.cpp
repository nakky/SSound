

#include "SSound/SoundDriver.h"
#include "SSound/SoundEffector.h"


#if defined PLAYERAPI_MME
#include "SSound/MME/MMEDriver.h"
#elif defined PLAYERAPI_COREAUDIO
#include "SSound/CoreAudio/CoreAudioDriver.h"
#elif defined PLAYERAPI_ALSA
#include "SSound/ALSA/ALSADriver.h"
#elif defined PLAYERAPI_OPENSLES
#include "SSound/OpenSLES/OpenSLESDriver.h"
#endif


namespace SSound{

	bool SoundDriver::open()
	{
		Object::init();

		unsigned int tmpMemorySize[STREAMING_NUM_BUFFER];

		unsigned int memsum = STREAMING_BUFFER_SIZE;
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			if(i != STREAMING_NUM_BUFFER - 1){
				tmpMemorySize[i] =  STREAMING_BUFFER_SIZE / STREAMING_NUM_BUFFER;
				tmpMemorySize[i] = (tmpMemorySize[i] / 2) * 2;
				memsum -= tmpMemorySize[i];
			}else{
				tmpMemorySize[i] = memsum;
			}
		}

		for (int i = 0; i < STREAMING_NUM_BUFFER; i++)
		{
			mSoundBuffers[i] = new SoundBuffer(mFileDescriptor, mEffectorHead);
			mSoundBuffers[i]->init();
			mSoundBuffers[i]->allocateBuffer(tmpMemorySize[i]);
		}

		return TRUE;
	}

	void SoundDriver::close()
	{
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			if(mSoundBuffers[i] != NULL){
				mSoundBuffers[i]->cleanup();
				SAFE_DELETE(mSoundBuffers[i]);
				mSoundBuffers[i] = NULL;
			}
		}

		Object::cleanup();
	}

	bool SoundDriver::setRepeatTime(const unsigned int startMilliSec, const unsigned int endMilliSec)
		{
		SoundDataFormat format = mFileDescriptor->getDataFormat();
		unsigned int startPoint = (unsigned int)((float)startMilliSec / 1000.0f * (float)format.mSampleRate);
		unsigned int endPoint = (unsigned int)((float)endMilliSec / 1000.0f * (float)format.mSampleRate);

		unsigned int terminalPoint = format.mNumSample;

		if(endPoint > terminalPoint) return FALSE;

		if(startMilliSec == 0){
			mLoopStartPos = 0;
		}else{
			mLoopStartPos = startPoint;
		}

		if(endMilliSec == 0){
			mLoopEndPos = terminalPoint;
		}else{
			mLoopEndPos = endPoint;
		}
		return TRUE;
	}


	SoundDriver *SoundDriver::createDriver(SoundEffector *head, SoundFileDescriptor *fd)
	{

#if defined PLAYERAPI_MME
		return new MMEDriver(head, fd);
#elif defined PLAYERAPI_COREAUDIO
		return new CoreAudioDriver(head, fd);
#elif defined PLAYERAPI_ALSA
		return new ALSADriver(head, fd);
#elif  defined PLAYERAPI_OPENSLES
		return new OpenSLESDriver(head, fd);
#endif

	}

};	//namespace SSound