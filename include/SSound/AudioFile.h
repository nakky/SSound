/******************************************************************/
/*!
	@file  AudioFile.h
	@brief File descriptor class are defined
	@note
	@todo
	@bug

	@author Naoto Nakamura
	@date   Sep 10, 2009
*/
/******************************************************************/

#ifndef SSOUND_AUDIOFILE_H
#define SSOUND_AUDIOFILE_H

#include "SSound/SoundCommon.h"

#include "S3DMath/Resource.h"

#include <string>

namespace SSound
{
	// required
	class SoundFileDescriptor;

	/****************************************/
	/*!
		@class AudioFile
		@brief Abstract

		@author Naoto Nakamura
		@date Dec. 10, 2008
		*/
	/****************************************/
	class AudioFile : public S3DMath::Resource
	{
	public:
		AudioFile(const std::string &path, const std::string &ext, const bool cache)
			: S3DMath::Resource(),
			  mFilePath(path),
			  mExtension(ext),
			  mDataHead(NULL),
			  mIsCached(cache),
			  mDataSize(0)
		{
		}

	public:
		virtual void init();
		virtual void cleanup();

		virtual void load();
		virtual void unload();

		unsigned int getData(void *buffer, unsigned int pos, unsigned int size);

		unsigned int getDataSize() const { return mDataSize; }
		void cache();
		void uncache();

		bool isCached() const { return mIsCached; }

		SoundFileDescriptor *createFileDescriptor();

	private:
		std::string mFilePath;
		std::string mExtension;

		char *mDataHead;
		bool mIsCached;
		unsigned int mDataSize;
		SoundDataFormat mFormat;
	};

}; // namespace SSound

#endif // SSOUND_AUDIOFILE_H
