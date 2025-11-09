/******************************************************************/
/*!
	@file  SoundSystem.h
	@brief Manager class is defined
	@note
	@todo
	@bug

	@author Naoto Nakamura
	@date   Sep 17, 2009
*/
/******************************************************************/

#ifndef SSOUND_SOUNDSYSTEM_H
#define SSOUND_SOUNDSYSTEM_H

#include "SSound/SoundCommon.h"

#include "SSound/SoundObject.h"

#include "SSound/IO.h"
#include "SThread/QueueThread.h"

#include "SSound/SoundCommon.h"

#ifdef PLAYERAPI_OPENSLES
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES.h>
#endif // PLAYERAPI_OPENSLES

namespace SSound
{

	//////////////////////////////////////////////////
	//				forward declarations			//
	//////////////////////////////////////////////////
	// implemented
	class SoundSystem;

	// required
	class Sound;

	/****************************************/
	/*!
		@class	SoundSystem
		@brief
		@author	Naoto Nakamura
		@date	Sep 18, 2009
		*/
	/****************************************/
	class SoundSystem
	{
		friend class Sound;
		friend class BufferingSound;

	private:
		SoundSystem();

	public:
		static void init();
		static void cleanup();

	private:
#if defined PLAYERAPI_OPENSLES
		static SLObjectItf mEngine;
		static SLEngineItf mEngineInterface;
#endif

	public:
		static AudioFile *createAudioFile(
			const std::string &filePath,
			const std::string &ext,
			const bool cache = FALSE);

#if defined PLAYERAPI_OPENSLES
		static SLEngineItf getEngineInterface() { return mEngineInterface; }
#endif
	};

}; // namespace SSound

#endif // SSOUND_SOUNDSYSTEM_H
