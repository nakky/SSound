/******************************************************************/
/*!
	@file  ALSADriver.h
	@brief ALSA driver class is defined
	@note
	@todo
	@bug

	@author Naoto Nakamura
	@date   Sep 10, 2009
*/
/******************************************************************/

#ifndef SSOUND_ALSADRIVER_H
#define SSOUND_ALSADRIVER_H


#include "SSound/SoundCommon.h"

#include "SThread/Lock.h"
#include "SThread/QueueThread.h"

#include "SSound/SoundDriver.h"

#include <alsa/asoundlib.h>

//#ifdef PLAYERAPI_ALSA

namespace SSound{

	class ALSADriver;

		/****************************************/
	/*!
		@class	Sound write request
		@brief

		@author	Naoto Nakamura
		@date	Nov. 9, 2009
		*/
	/****************************************/
	class ALSASoundQueue : public SThread::WorkRequest
	{

	public:
		ALSASoundQueue(ALSADriver *driver, int bufferIndex)
			:SThread::WorkRequest(PRIORITY_NORMAL, false),
			mDriver(driver),
			mBufferIndex(bufferIndex)
		{}

	protected:
		virtual bool work();

	public:

	private:
		ALSADriver *mDriver;
		int mBufferIndex;
	};

	/****************************************/
	/*!
		@class ALSADriver
		@brief

		@author Naoto Nakamura
		@date Dec. 10, 2008
		*/
	/****************************************/
	class ALSADriver : public SoundDriver
	{
	public:
		ALSADriver(SoundEffector *head, SoundFileDescriptor *fd)
			:SoundDriver(head, fd),
			mIsEnabled(false),
			mIsRepeat(false),
			mIsMmapAvailable(false),
			mPreviousState(SND_PCM_STATE_LAST),
			mIsAborted(false)
		{}

		virtual bool open();
		virtual void close();

		virtual void play();
		virtual void pause();
		virtual void stop();

		virtual bool isEnabled();

		virtual bool isRepeat(){ return mIsRepeat; }
		virtual void setRepeat(bool repeat) { mIsRepeat = repeat; }


		virtual float getVolume();
		virtual bool setVolume(const float vol);

		virtual float getPan();
		virtual bool setPan(const float pan);


		void threadBuffering(unsigned int bIndex);

	private:

		void resetWithBufferring();
		void setEnabled(bool enabled);

	private:

		bool mIsEnabled;
		bool mIsRepeat;
		bool mIsAborted;

		SThread::QueueThread mQueueThread;
		SThread::Mutex mLock;

		snd_pcm_t* mPCMHandle;
		char *mBuffer[STREAMING_NUM_BUFFER];

		ALSASoundQueue *mQueue[STREAMING_NUM_BUFFER];

		int mNumFrames;

		bool mIsMmapAvailable;
		struct pollfd *mUfds;

		snd_pcm_state_t mPreviousState;

	};


};	//namespace SSound

//#endif //PLAYERAPI_ALSA

#endif // SSOUND_ALSADRIVER_H
