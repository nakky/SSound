/******************************************************************/
/*!
 @file	WavReader.h
 @brief	Wave file reader class definition
 @note
 @todo
 @bug

 @author	Naoto Nakamura
 @date   Dec. 11, 2009
 */
/******************************************************************/
#ifndef SSOUND_WAVFILECREATOR_H
#define SSOUND_WAVFILECREATOR_H

#include "SSound/SoundCommon.h"
#include "SSound/SoundFactory.h"

#include <fstream>

namespace SSound
{
	//////////////////////////////////////////////////
	//				forward declarations			//
	//////////////////////////////////////////////////
	// defined class
	class WavFileDescriptor;
	class WavCreator;

	// required class
	//////////////////////////////////////////////////
	//				class declarations				//
	//////////////////////////////////////////////////

	/****************************************/
	/*!
		@class	 WavFileDescriptor
		@brief	 Wave file descriptor class
		@author	Naoto Nakamura
		@date	Dec. 11, 2009
		*/
	/****************************************/
	class WavFileDescriptor : public SoundFileDescriptor
	{
		friend class WavFileCreator;

	public:
		WavFileDescriptor(Input *input)
			: SoundFileDescriptor(),
			  mCurrentSample(0),
			  mInput(input)
		{
		}

		virtual unsigned int getCurrentSample() const { return mCurrentSample; }

		virtual size_t load(void *data, const size_t size);
		virtual bool seek(unsigned int pos);
		virtual bool close();

	private:
		Input *mInput;
		unsigned int mCurrentSample;
	};

	/****************************************/
	/*!
		@class	 WavFileCreator
		@brief	 Wave file creator class
		@author	Naoto Nakamura
		@date	Dec. 11, 2009
		*/
	/****************************************/
	class WavFileCreator : public SoundCreator
	{
	public:
		// constructor
		explicit WavFileCreator() : SoundCreator() {}

		// read
		virtual SoundFileDescriptor *createFileDescriptor(Input *input);
	};

}; // namespace SSound

#endif // SSOUND_WAVFILECREATOR_H
