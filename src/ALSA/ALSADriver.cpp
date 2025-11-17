
#include "SSound/ALSA/ALSADriver.h"
#include "SSound/SoundEffector.h"

#include "SThread/Timer.h"

#include <iostream>

// #ifdef PLAYERAPI_ALSA

namespace SSound
{
	bool ALSASoundQueue::work()
	{
		mDriver->threadBuffering(mBufferIndex);
		return TRUE;
	}

	/****************************************/
	/*!
		@brief  Open
		@note

		@author Naoto Nakamura
		@date   Oct 9, 2009
	*/
	/****************************************/
	bool ALSADriver::open()
	{
		SoundDriver::open();

		mQueueThread.init();
		// mQueueThread.setPriority(Foundation::PRIORITY_BELOW_NORMAL);
		mQueueThread.start();
		mQueueThread.suspend();

		const char *dev = "default";

		snd_pcm_open(&mPCMHandle, dev, SND_PCM_STREAM_PLAYBACK, 0); //"plughw:0,0"

		snd_pcm_format_t pcmformat;

		if (mDataFormat.mBytesPerSample == 2)
		{
			pcmformat = SND_PCM_FORMAT_S16_LE;
		}
		else
		{
			pcmformat = SND_PCM_FORMAT_S8;
		}

		uint16_t channels = mDataFormat.mChannelsPerSample;
		uint32_t samplesPerSec	= mDataFormat.mSampleRate;
		int soft_resample = 1;
		unsigned int latency = 50000;

		snd_pcm_set_params(mPCMHandle, pcmformat, SND_PCM_ACCESS_RW_INTERLEAVED, channels, samplesPerSec, soft_resample, latency);

		for (int i = 0; i < STREAMING_NUM_BUFFER; i++)
		{
			mBuffer[i] = new char[mSoundBuffers[i]->getBufferSize()];

			mQueue[i] = new ALSASoundQueue(this, i);
			mQueue[i]->init();
		}

		resetWithBufferring();

		return true;
	}

	/****************************************/
	/*!
		@brief  Close
		@note

		@author Naoto Nakamura
		@date   Oct 9, 2009
	*/
	/****************************************/
	void ALSADriver::close()
	{
		mLock.lock();

		mQueueThread.cleanup();

		snd_pcm_drain(mPCMHandle);
		snd_pcm_close(mPCMHandle);
		mPCMHandle = NULL;

		mLock.unlock();

		SAFE_DELETE_ARRAY(mUfds);

		for (int i = 0; i < STREAMING_NUM_BUFFER; i++)
		{
			SAFE_DELETE_ARRAY(mBuffer[i]);
			SAFE_DELETE(mQueue[i]);
		}

		SoundDriver::close();
	}

	void ALSADriver::play()
	{
		mLock.lock();
		mQueueThread.resume();
		mLock.unlock();
	}

	void ALSADriver::pause()
	{
		mQueueThread.suspend();

		mLock.lock();

		snd_pcm_drop(mPCMHandle);
		snd_pcm_prepare(mPCMHandle);

		mLock.unlock();
	}

	void ALSADriver::stop()
	{
		mQueueThread.suspend();

		mLock.lock();

		snd_pcm_drop(mPCMHandle);
		snd_pcm_prepare(mPCMHandle);

		mLock.unlock();
		resetWithBufferring();
	}

	bool ALSADriver::isEnabled()
	{
		mLock.lock();
		bool ret = this->mIsEnabled;
		mLock.unlock();

		return ret;
	}

	void ALSADriver::setEnabled(bool enabled)
	{
		mLock.lock();
		this->mIsEnabled = enabled;
		mLock.unlock();
	}

	bool ALSADriver::setVolume(const float vol)
	{
		float rvol = vol;
		if (vol < 0.0f)
			rvol = 0.0f;
		else if (vol > 1.0f)
			rvol = 1.0f;

		mLock.lock();
		((VolumePanEffector *)mEffectorHead)->setVolume(rvol);
		mLock.unlock();

		return TRUE;
	}

	bool ALSADriver::setPan(const float pan)
	{
		float rpan = pan;
		if (pan < -1.0f)
			rpan = -1.0f;
		else if (pan > 1.0f)
			rpan = 1.0f;

		mLock.lock();
		((VolumePanEffector *)mEffectorHead)->setPan(rpan);
		mLock.unlock();
		return TRUE;
	}

	float ALSADriver::getVolume()
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector *)mEffectorHead)->getVolume();
		mLock.unlock();
		return ret;
	}

	float ALSADriver::getPan()
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector *)mEffectorHead)->getPan();
		mLock.unlock();
		return ret;
	}

	void ALSADriver::threadBuffering(unsigned int bIndex)
	{
		mLock.lock();

		size_t readDataSize = 0;
		size_t startPos = 0;
		size_t size = 0;

		unsigned int nextSample =
			mFileDescriptor->getCurrentSample() + mSoundBuffers[bIndex]->getBufferSize() / mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample;

		//std::cout << "next:" << ( nextSample) << std::endl;

		if (mIsRepeat && nextSample > mLoopEndPos)
		{
			unsigned int preSize = (mLoopEndPos - mFileDescriptor->getCurrentSample()) * mDataFormat.mChannelsPerSample * mDataFormat.mBytesPerSample;
			startPos = 0;
			size = preSize;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy(mBuffer[bIndex] + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);

			mFileDescriptor->seek(mLoopStartPos);
			startPos = preSize;
			size = mSoundBuffers[bIndex]->getBufferSize() - preSize;
			readDataSize += mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy(mBuffer[bIndex] + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);
		}
		else
		{
			startPos = 0;
			size = 0;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy(mBuffer[bIndex] + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);
		}
		mLock.unlock();

		mLock.lock();
		if (readDataSize == 0)
		{
			mLock.unlock();

			stop();

			mLock.lock();
		}
		else if (readDataSize < mSoundBuffers[bIndex]->getBufferSize())
		{
			mSoundBuffers[bIndex]->setEndPointFlag(true);
		}

		
		int numFrames = readDataSize / mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample;
		int frames = 0;
		int framesum = 0;

		//std::cout << "numFrames:" << ( numFrames) << std::endl;

		mLock.unlock();

		if(numFrames == 0) return;

		snd_pcm_sframes_t delay;
		snd_pcm_delay(mPCMHandle, &delay);

		while (delay > 0 && delay > numFrames * 3)
		{
			snd_pcm_delay(mPCMHandle, &delay);
			if (mQueue[bIndex]->getState() == SThread::WorkRequest::WORK_ABORTED)
				return;
			SThread::Timer::sleep(2);
		}

		mLock.lock();

		while (framesum < numFrames)
		{
			frames = snd_pcm_writei(mPCMHandle, mBuffer[bIndex], numFrames - framesum);

			if (frames < 0)
			{
				frames = snd_pcm_recover(mPCMHandle, frames, 0);
			}

			if (frames > 0)
			{
				framesum += frames;
			}
			else
			{
				snd_pcm_prepare(mPCMHandle);
				snd_pcm_start(mPCMHandle);
			}
		}
		

		mLock.unlock();

		mLock.lock();

		mQueue[bIndex]->resetState();
		mQueueThread.addRequest(mQueue[bIndex]);

		mLock.unlock();
	}

	/****************************************/
	/*!
		@brief	Reset with bufferring leading data
		@note

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	void ALSADriver::resetWithBufferring()
	{
		mLock.lock();

		snd_pcm_drop(mPCMHandle);
		snd_pcm_prepare(mPCMHandle);

		mFileDescriptor->reset();

		mLock.unlock();

		mLock.lock();
		mQueueThread.clearAllRequest();
		for (int i = 0; i < STREAMING_NUM_BUFFER; i++)
		{
			mQueue[i]->resetState();
			mQueueThread.addRequest(mQueue[i]);
			if (mSoundBuffers[i]->isEndPoint())
				break;
		}
		mLock.unlock();
	}

}; // namespace SSound

// #endif //PLAYERAPI_ALSA
