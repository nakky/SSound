

#include "SSound/SoundBuffer.h"

#include "SSound/SoundEffector.h"


namespace SSound{


	//////////////////////////////////////////////////////////////////////
	//							SoundBuffer								//
	//////////////////////////////////////////////////////////////////////

	/****************************************/
	/*!
		@brief	Allocate buffer
		@note

		@param	bufferSize Buffer size on memory
		@return	If processing is done validly,
				return true, else return false

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	bool SoundBuffer::allocateBuffer(const unsigned int bufferSize)
	{
		mBufferSize = bufferSize;

		mInputBuffer = new char[bufferSize];

		return true;
	}
	/****************************************/
	/*!
		@brief	Release buffer
		@note

		@return	If processing is done validly,
				return true, else return false

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	bool SoundBuffer::releaseBuffer()
	{
		if(mBufferSize == 0)return false;

		SAFE_DELETE_ARRAY(mInputBuffer);

		mBufferSize = 0;
		return true;
	}

	/****************************************/
	/*!
		@brief	Buffering method
		@note

		@return	Read data size

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	size_t SoundBuffer::bufferData(size_t startBufferPos, size_t &size)
	{
		if(size == 0)size = mBufferSize;
		size_t readSize = mFileDescriptor->load(mInputBuffer + startBufferPos, size);
		mCurrentBufferHead = mInputBuffer;

		SoundEffector *cureff = mEffectorHead;
		SoundEffector *nexteff = mEffectorHead->getNext();
		while(cureff != NULL){
			nexteff = cureff->getNext();

			cureff->process(mCurrentBufferHead + startBufferPos, size);
			mCurrentBufferHead = cureff->mBuffer;

			cureff = nexteff;
		}

		return readSize;
	}


};	//namespace SSound
