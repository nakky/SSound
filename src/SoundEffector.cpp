

#include "SSound/SoundEffector.h"

namespace SSound{
	

	//////////////////////////////////////////////////////////////////////
	//							SoundEffector							//
	//////////////////////////////////////////////////////////////////////
	void SoundEffector::init()
	{
		mBuffer = new char [STREAMING_BUFFER_SIZE];
	}

	void SoundEffector::cleanup()
	{
		SAFE_DELETE_ARRAY(mBuffer);
	}
	
	

	//////////////////////////////////////////////////////////////////////
	//							VolumePanEffector						//
	//////////////////////////////////////////////////////////////////////
	void VolumePanEffector::init()
	{
		SoundEffector::init();
		setVolumePan(1.0f, 0.0f);
	}
	
	void VolumePanEffector::updateFilter()
	{
		if(mFormat.mChannelsPerSample == 2){
			mFilter[0] = mVolume * (1.0f - mPan) / 2.0f;
			mFilter[1] = mVolume * (1.0f + mPan) / 2.0f;
		}else{
			mFilter[0] = mVolume;
			mFilter[1] = mVolume;
		}
	}

	void VolumePanEffector::process(char *src, size_t size)
	{
		short *ssrc = (short*)src;
		short *sdst = (short*)mBuffer;

		size_t length = size / 2;
		for(size_t i = 0 ; i < length ; i += 2){
			*(sdst + i) = (short)((float)*(ssrc + i) * mFilter[0]);
			*(sdst + (i + 1)) = (short)((float)*(ssrc + (i + 1)) * mFilter[1]);
		}
	}

};	//namespace SSound
