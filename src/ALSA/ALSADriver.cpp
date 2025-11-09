
#include "SSound/ALSA/ALSADriver.h"
#include "SSound/SoundEffector.h"

#include "SThread/Timer.h"

// #ifdef PLAYERAPI_ALSA

namespace SSound
{
	static int xrun_recovery(snd_pcm_t *handle, int err)
	{
		if (err == -EPIPE)
		{ /* under-run */
			err = snd_pcm_prepare(handle);
			if (err < 0)
			{
				printf("Can't recover from underrun, prepare failed: %s\n", snd_strerror(err));
			}
			else
			{
				printf("Recovered from underrun\n");
			}
			return 0;
		}
		else if (err == -ESTRPIPE)
		{ /* suspend */
			err = snd_pcm_resume(handle);
			if (err < 0)
			{
				printf("Can't recover from suspend, resume failed: %s\n", snd_strerror(err));
			}
			else
			{
				printf("Resumed successfully\n");
			}
			return 0;
		}
		else
		{
			printf("Unknown error code: %d\n", err);
		}

		return err;
	}

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

		snd_pcm_open(&mPCMHandle, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK); //"plughw:0,0"

		snd_pcm_hw_params_t *hwparams;
		snd_pcm_hw_params_malloc(&hwparams);
		snd_pcm_hw_params_any(mPCMHandle, hwparams);

		if (snd_pcm_hw_params_set_access(mPCMHandle, hwparams, SND_PCM_ACCESS_MMAP_INTERLEAVED) == 0)
		{
			mIsMmapAvailable = true;
		}
		else
		{
			snd_pcm_hw_params_set_access(mPCMHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
		}

		snd_pcm_format_t pcmformat;

		if (mDataFormat.mBytesPerSample == 2)
		{
			pcmformat = SND_PCM_FORMAT_S16_LE;
		}
		else
		{
			pcmformat = SND_PCM_FORMAT_S8;
		}

		snd_pcm_hw_params_set_channels(mPCMHandle, hwparams, mDataFormat.mChannelsPerSample);
		snd_pcm_hw_params_set_format(mPCMHandle, hwparams, pcmformat);
		snd_pcm_hw_params_set_rate_near(mPCMHandle, hwparams, &mDataFormat.mSampleRate, 0);

		snd_pcm_hw_params_set_periods(mPCMHandle, hwparams, 1, 0);
		snd_pcm_hw_params_set_buffer_size(mPCMHandle, hwparams, STREAMING_NUM_BUFFER);
		snd_pcm_hw_params(mPCMHandle, hwparams);

		snd_pcm_hw_params_free(hwparams);

#if 0
		//sw
		snd_pcm_sw_params_t *swparams;
		snd_pcm_sw_params_alloca(&swparams);
		snd_pcm_sw_params_current(mPCMHandle, swparams);
		//snd_pcm_sw_params_set_start_threshold(mPCMHandle, swparams, ex_data->frag_len);
		//snd_pcm_sw_params_set_avail_min(mPCMHandle, swparams, ex_data->frag_len);
		snd_pcm_sw_params(mPCMHandle, swparams);

		snd_pcm_sw_params_free(swparams);
#endif // 0

		int ufdCount = snd_pcm_poll_descriptors_count(mPCMHandle);
		mUfds = new struct pollfd[ufdCount];
		snd_pcm_poll_descriptors(mPCMHandle, mUfds, ufdCount);

		if (!mIsMmapAvailable)
			snd_pcm_nonblock(mPCMHandle, 0);

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
		mQueueThread.cleanup();

		mLock.lock();

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
		mLock.lock();
		mQueueThread.suspend();

		snd_pcm_drop(mPCMHandle);
		snd_pcm_prepare(mPCMHandle);

		mLock.unlock();
	}

	void ALSADriver::stop()
	{
		mLock.lock();

		mQueueThread.suspend();

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

		if (mIsMmapAvailable)
		{

			snd_pcm_state_t state = snd_pcm_state(mPCMHandle);
			if (state != mPreviousState)
			{
				printf("state changed to: %s\n", snd_pcm_state_name(state));
				mPreviousState = state;
			}

			if (state == SND_PCM_STATE_SETUP)
			{
				int rc = snd_pcm_prepare(mPCMHandle);
			}

			if (state == SND_PCM_STATE_PREPARED)
			{
				int rc = snd_pcm_start(mPCMHandle);
				printf("snd_pcm_start returned: %d\n", rc);
			}

			snd_pcm_avail_update(mPCMHandle);
			snd_pcm_uframes_t numFrames = readDataSize / mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample;

			const snd_pcm_channel_area_t *areas;
			snd_pcm_uframes_t offset;

			int ret = snd_pcm_mmap_begin(mPCMHandle, &areas, &offset, &numFrames);
			if (ret < 0)
			{
				if ((ret = xrun_recovery(mPCMHandle, ret)) < 0)
				{
					printf("MMAP begin avail error: %s\n", snd_strerror(ret));
				}
			}
			else
			{
				char *mmap = (char *)areas[0].addr + areas[0].first / 8 + offset * areas[0].step / 8;

				memcpy(mmap, mBuffer[bIndex], mSoundBuffers[bIndex]->getBufferSize());
			}

			snd_pcm_sframes_t commitframes = snd_pcm_mmap_commit(mPCMHandle, offset, numFrames);
			if (commitframes < 0 || (snd_pcm_uframes_t)commitframes != numFrames)
			{
				if ((ret = xrun_recovery(mPCMHandle, commitframes >= 0 ? -EPIPE : commitframes)) < 0)
				{
					printf("MMAP commit error: %s\n", snd_strerror(ret));
				}
			}
		}
		else
		{

			int numFrames = readDataSize / mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample;
			int frames = 0;
			int framesum = 0;

			mLock.unlock();
			snd_pcm_sframes_t delay;
			snd_pcm_delay(mPCMHandle, &delay);

			while (delay > 0 && delay > numFrames * 3)
			{
				snd_pcm_delay(mPCMHandle, &delay);
				if (mQueue[bIndex]->getState() == Foundation::WorkRequest::WORK_ABORTED)
					return;
				Foundation::Timer::sleep(2);
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
		}

		mLock.unlock();

		// static int index = 0;
		// printf("update:%d\n", index++);

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
