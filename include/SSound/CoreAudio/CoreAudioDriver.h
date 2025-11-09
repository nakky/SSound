/******************************************************************/
/*!
	@file  CoreAudioDriver.h
	@brief CoreAudio driver class is defined
	@note
	@todo
	@bug

	@author Naoto Nakamura
	@date   Sep 10, 2009
*/
/******************************************************************/

#ifndef SSOUND_COREAUDIODRIVER_H
#define SSOUND_COREAUDIODRIVER_H


#include "SSound/SoundCommon.h"

#include "VS/Foundation/Lock.h"
#include "VS/Foundation/QueueThread.h"

#include <AudioToolbox/AudioToolbox.h>

#include "SSound/SoundDriver.h"


namespace SSound{

	class CoreAudioDriver;

		/****************************************/
	/*!
		@class	Sound write request
		@brief

		@author	Naoto Nakamura
		@date	Nov. 9, 2009
		*/
	/****************************************/
	class CoreAudioSoundRequest : public SThread::WorkRequest
	{

	public:
		CoreAudioSoundRequest(CoreAudioDriver *driver, int bufferIndex)
			:SThread::WorkRequest(PRIORITY_NORMAL, false),
			mDriver(driver),
			mBufferIndex(bufferIndex)
		{}

	protected:
		virtual bool work();

	public:


	private:
		CoreAudioDriver *mDriver;
		int mBufferIndex;
	};

	/****************************************/
	/*!
		@class MMEDriver
		@brief 

		@author Naoto Nakamura
		@date Dec. 10, 2008
		*/
	/****************************************/
	class CoreAudioDriver : public SoundDriver
	{
	public:
		CoreAudioDriver(SoundEffector *head, SoundFileDescriptor *fd)
			:SoundDriver(head, fd),
			mIsEnabled(false),
			mIsRepeat(false)
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
		static void callback(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef buffer);

		void setEnabled(bool enabled);

		void resetWithBufferring();

	private:		
		bool mIsEnabled;
		bool mIsRepeat;

		SThread::QueueThread mQueueThread;
		SThread::Mutex mLock;

		AudioQueueRef mAudioQueueID;
		AudioQueueBufferRef mBufferID[STREAMING_NUM_BUFFER];

		CoreAudioSoundRequest *mQueue[STREAMING_NUM_BUFFER];

	};


};	//namespace SSound


#endif // SSOUND_COREAUDIODRIVER_H
