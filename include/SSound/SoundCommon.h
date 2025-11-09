/******************************************************************/
/*!
    @file  SoundCommon.h
    @brief Common header for SSound
    @note
    @todo
    @bug

    @author Naoto Nakamura
    @date   Sep 10, 2009
*/
/******************************************************************/

#ifndef SSOUND_SOUNDCOMMON_H
#define SSOUND_SOUNDCOMMON_H

#include "SSound/Common.h"

#if defined OS_WINDOWS

#define PLAYERAPI_MME
// #define PLAYERAPI_WASAPI

#elif defined OS_MACOSX || defined OS_IPHONE

#define PLAYERAPI_COREAUDIO

#elif defined OS_LINUX

#define PLAYERAPI_ALSA

#elif defined OS_ANDROID

#define PLAYERAPI_OPENSLES

#else

#endif

namespace SSound
{

#if defined PLAYERAPI_OPENSLES
    static const int STREAMING_NUM_BUFFER = 1;
    static const unsigned int STREAMING_BUFFER_SIZE = 2048;
#else
    static const int STREAMING_NUM_BUFFER = 3;
    static const unsigned int STREAMING_BUFFER_SIZE = 44100;
#endif

    /****************************************/
    /*!
        @brief struct SoundDataFormat

        @author Naoto Nakamura
        @date Dec. 10, 2008
        */
    /****************************************/
    struct SoundDataFormat
    {
        unsigned int mSampleRate;        //!< Sample Rate
        unsigned int mChannelsPerSample; //!< Number of channels in a frame
        unsigned int mBytesPerSample;    //!< Sound data size per frame
        unsigned int mNumSample;         //!< No. frames (samples)
        unsigned int mDataSize;          //!< Sound data size
    };

    extern void init();
    extern void cleanup();

}; // namespace SSound

#endif // SSOUND_SOUNDCOMMON_H
