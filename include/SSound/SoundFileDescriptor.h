/******************************************************************/
/*!
	@file  SoundFileDescirptorr.h
	@brief File descriptor class are defined
	@note
	@todo
	@bug

	@author Naoto Nakamura
	@date   Sep 10, 2009
*/
/******************************************************************/

#ifndef SSOUND_SOUNDFILEDESCRIPTOR_H
#define SSOUND_SOUNDFILEDESCRIPTOR_H

#include "SSound/SoundCommon.h"

#include "VS/Foundation/ResourceDB.h"

#include <string.h>

namespace SSound
{

	// required
	class AudioFile;

	/****************************************/
	/*!
		@class SoundFileDescriptor
		@brief Abstract

		@author Naoto Nakamura
		@date Dec. 10, 2008
		*/
	/****************************************/
	class SoundFileDescriptor
	{
		friend class AudioFile;
		friend class SoundCreator;

	public:
		SoundFileDescriptor()
			: mIsOpened(true)
		{
		}

		virtual ~SoundFileDescriptor() {}

		virtual size_t load(void *data, const size_t size) = 0;
		virtual bool seek(unsigned int pos) = 0;
		virtual void reset() { seek(0); }

		virtual bool close()
		{
			mIsOpened = false;
			return true;
		}
		virtual bool isOpened() const { return mIsOpened; }

		virtual unsigned int getCurrentSample() const = 0;

		SoundDataFormat getDataFormat() const { return mDataFormat; }

		int getChannelsPerSample() const { return mDataFormat.mChannelsPerSample; }

	private:
		bool mIsOpened; //<! Flag for if the file is opened

	protected:
		SoundDataFormat mDataFormat; //<! Target data format
	};

	/****************************************/
	/*!
		@class MemoryFileDescriptor
		@brief Sound file descriptor of virtual file on memory

		@author Naoto Nakamura
		@date Dec. 10, 2008
		*/
	/****************************************/
	class MemoryFileDescriptor : public SoundFileDescriptor
	{
	private:
		MemoryFileDescriptor();

	public:
		MemoryFileDescriptor(AudioFile *file)
			: SoundFileDescriptor(),
			  mAudioFile(file),
			  mCurrentSample(0)
		{
		}

	public:
		virtual size_t load(void *data, const size_t size);
		virtual bool seek(unsigned int pos);
		virtual unsigned int getCurrentSample() const { return mCurrentSample; }

	private:
		AudioFile *mAudioFile;
		unsigned int mCurrentSample;
	};

}; // namespace SSound

#endif // SSOUND_SOUNDFILEDESCRIPTOR_H
