


#include "SThread/QueueThread.h"

#include "SSound/SoundBuffer.h"
#include "SSound/SoundFactory.h"
#include "SSound/SoundSystem.h"
#include "SSound/SoundEffector.h"

#include "SSound/AudioFile.h"


namespace SSound{
	
	//////////////////////////////////////////////////////////////////////
	//							MemoryFileDescriptor					//
	//////////////////////////////////////////////////////////////////////
	size_t MemoryFileDescriptor::load(void *data, const size_t size)
	{
		unsigned int dataPos = mCurrentSample * mDataFormat.mChannelsPerSample * mDataFormat.mBytesPerSample;
		int realSize = mAudioFile->getData(data, dataPos, (int)size);
		mCurrentSample += realSize / mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample;		
		return realSize;
	}

	bool MemoryFileDescriptor::seek(unsigned int pos)
	{
		if(pos > mAudioFile->getDataSize())return FALSE;
		mCurrentSample = pos;
		return TRUE;
	}

	
};	//namespace SSound
