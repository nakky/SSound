

#include "SThread/QueueThread.h"

#include "SSound/SoundObject.h"
#include "SSound/SoundFactory.h"
#include "SSound/SoundSystem.h"
#include "SSound/SoundEffector.h"
#include "SSound/SoundDriver.h"
#include "SSound/AudioFile.h"

namespace SSound{


	//////////////////////////////////////////////////////////////////////
	//							SoundObject								//
	//////////////////////////////////////////////////////////////////////
	/****************************************/
	/*!
		@brief  Constructor
		@note

		@author Naoto Nakamura
		@date   Oct 9, 2009
	*/
	/****************************************/
	SoundObject::SoundObject(const AudioFile *file)
		:Object(),
		mAudioFile(file),
		mFileDescriptor(NULL),
		mState(SOUNDOBJECT_STOP),
		mEffectorHead(NULL)
	{
	}

	/****************************************/
	/*!
		@brief	Initialize
		@note

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	void SoundObject::init()
	{
		mFileDescriptor = mAudioFile->createFileDescriptor();
		mDataFormat = mFileDescriptor->getDataFormat();

		mEffectorHead = new VolumePanEffector(mDataFormat);
		mEffectorHead->init();

		mDriver = SoundDriver::createDriver(mEffectorHead, mFileDescriptor);
		mDriver->open();

		setRepeatTime();

		return;
	}

	/****************************************/
	/*!
		@brief	cleanup
		@note

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	void SoundObject::cleanup()
	{
		pause();

		mDriver->close();
		SAFE_DELETE(mDriver);

		SoundEffector *cureff = mEffectorHead;
		SoundEffector *nexteff = NULL;
		while(cureff != NULL){
			nexteff = cureff->getNext();
			cureff->cleanup();
			SAFE_DELETE(cureff);
			cureff = nexteff;

		}
		
		if(mFileDescriptor != NULL){
			mFileDescriptor->close();
			SAFE_DELETE( mFileDescriptor );
			mFileDescriptor = NULL;
		}
	}


	/****************************************/
	/*!
		@brief	Play
		@note

		@author Naoto Nakamura
		@date   Sep 16, 2009
	*/
	/****************************************/
	bool SoundObject::play()
	{
		mState = SOUNDOBJECT_PLAY;
		mDriver->play();
		return TRUE;
	}

	bool SoundObject::pause()
	{
		mState = SOUNDOBJECT_PAUSE;
		mDriver->pause();
		return TRUE;
	}

	bool SoundObject::stop()
	{
		mState = SOUNDOBJECT_PAUSE;
		mDriver->stop();
		return TRUE;
	}

	bool SoundObject::setVolume(const float vol)
	{
		mDriver->setVolume(vol);
		return TRUE;
	}

	bool SoundObject::setPan(const float pan)
	{
		mDriver->setPan(pan);
		return TRUE;
	}

	float SoundObject::getVolume() const
	{
		return mDriver->getVolume();
	}

	float SoundObject::getPan() const
	{
		return mDriver->getPan();
	}

};	//namespace SSound
