/******************************************************************/
/*!
	@file  SoundDriver.h
	@brief Base sound driver class is defined
	@note
	@todo
	@bug

	@author Naoto Nakamura
	@date   Sep 10, 2009
*/
/******************************************************************/

#ifndef SSOUND_SOUNDDRIVER_H
#define SSOUND_SOUNDDRIVER_H

#include "SSound/SoundCommon.h"

#include "SSound/SoundBuffer.h"

#include <string.h>

namespace SSound
{

	/****************************************/
	/*!
		@class SoundDriver
		@brief Abstract

		@author Naoto Nakamura
		@date Dec. 10, 2008
		*/
	/****************************************/
	class SoundDriver
	{
	public:
		SoundDriver(SoundEffector *head, SoundFileDescriptor *fd)
			: mDataFormat(fd->getDataFormat()),
			  mFileDescriptor(fd),
			  mEffectorHead(head)
		{
		}

		virtual bool open();
		virtual void close();

		virtual void play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;

		virtual bool isEnabled() = 0;

		virtual bool isRepeat() = 0;
		virtual void setRepeat(bool repeat) = 0;

		virtual float getVolume() = 0;
		virtual bool setVolume(const float vol) = 0;

		virtual float getPan() = 0;
		virtual bool setPan(const float pan) = 0;

		virtual bool setRepeatTime(const unsigned int startMilliSec = 0, const unsigned int endMilliSec = 0);

		static SoundDriver *createDriver(SoundEffector *head, SoundFileDescriptor *fd);

	protected:
		SoundDataFormat mDataFormat;
		SoundFileDescriptor *mFileDescriptor;

		SoundEffector *mEffectorHead;

		unsigned int mLoopStartPos;
		unsigned int mLoopEndPos;

		SoundBuffer *mSoundBuffers[STREAMING_NUM_BUFFER];
	};

}; // namespace SSound

#endif // SSOUND_SOUNDDRIVER_H
