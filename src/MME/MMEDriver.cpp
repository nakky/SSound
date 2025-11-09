
#include "SSound/MME/MMEDriver.h"
#include "SSound/SoundEffector.h"

#ifdef PLAYERAPI_MME

namespace SSound{

	bool MMESoundQueue::work(){
		mDriver->threadBuffering(mBufferIndex);
		return TRUE;
	}

	/****************************************/
	/*!
		@brief	Static buffer callback
		@note

		@param	inUserData instance of SoundData
		@param	inAQ Queue reference
		@param	buffer Target audio queue buffer

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	void CALLBACK MMEDriver::callback(
		HWAVEOUT hwo,
		UINT uMsg,
		DWORD_PTR inUserData,
		DWORD_PTR buffer,
		DWORD_PTR dwParam2)
	{
		switch(uMsg){
			case WOM_DONE:
				{
					int bufferIdx = -1;
					for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
						if((LPWAVEHDR)buffer == ((MMEDriver*)inUserData)->mWaveHDR[i]){
							bufferIdx = i;
							break;
						}
					}

					((MMEDriver*)inUserData)->mQueue[bufferIdx]->resetState();
					((MMEDriver*)inUserData)->mQueueThread.addRequest(((MMEDriver*)inUserData)->mQueue[bufferIdx]);	
				}
				break;
			case WOM_OPEN:
				{
					((MMEDriver*)inUserData)->setEnabled(true);
				}
				break;
			case WOM_CLOSE:
				{
					((MMEDriver*)inUserData)->setEnabled(false);
				}
				break;
		}
	}


	/****************************************/
	/*!
		@brief  Open
		@note

		@author Naoto Nakamura
		@date   Oct 9, 2009
	*/
	/****************************************/
	bool MMEDriver::open()
	{
		SoundDriver::open();

		mQueueThread.init();
		mQueueThread.start();
		mQueueThread.suspend();

		WAVEFORMATEX waveformat;
		waveformat.wFormatTag = WAVE_FORMAT_PCM;

		waveformat.nChannels = mDataFormat.mChannelsPerSample;
		waveformat.nSamplesPerSec = mDataFormat.mSampleRate;
		waveformat.wBitsPerSample = 16;
		waveformat.nBlockAlign = waveformat.nChannels * waveformat.wBitsPerSample / 8;
		waveformat.nAvgBytesPerSec =  waveformat.nSamplesPerSec * waveformat.nBlockAlign;
		waveformat.cbSize = 0;

		mLock.lock();
		MMRESULT res = 0;
		res = waveOutOpen
			(
			&mWaveOut,
			WAVE_MAPPER,
			&waveformat,
			(DWORD_PTR)&MMEDriver::callback,
			(DWORD_PTR)this,
			CALLBACK_FUNCTION
			);
		mLock.unlock();

	
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			mWaveHDR[i] = new WAVEHDR();

			ZeroMemory(mWaveHDR[i], sizeof(WAVEHDR));

			mWaveHDR[i]->lpData = (LPSTR) new BYTE[mSoundBuffers[i]->getBufferSize()];

			mWaveHDR[i]->dwBufferLength = mSoundBuffers[i]->getBufferSize();
			mWaveHDR[i]->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
			mWaveHDR[i]->dwLoops = 1;

			waveOutPrepareHeader(mWaveOut, mWaveHDR[i],  sizeof(WAVEHDR));
			mQueue[i] = new MMESoundQueue(this, i);
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
	void MMEDriver::close()
	{
		mQueueThread.cleanup();

		mLock.lock();
		if(mWaveOut != NULL){
			waveOutPause(mWaveOut);

			for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
				waveOutUnprepareHeader(mWaveOut, mWaveHDR[i], sizeof(WAVEHDR));
			}

			waveOutClose(mWaveOut);
			mWaveOut = NULL;
		}
		mLock.unlock();

		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			SAFE_DELETE_ARRAY(mWaveHDR[i]->lpData);
			SAFE_DELETE(mWaveHDR[i]);
		}
		
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			SAFE_DELETE(mQueue[i]);
		}

		SoundDriver::close();
	}

	
	void MMEDriver::play()
	{
		mLock.lock();
		mQueueThread.resume();
		mLock.unlock();
	}

	void MMEDriver::pause()
	{
		mLock.lock();
		mQueueThread.suspend();

		waveOutReset(mWaveOut);
		waveOutPause(mWaveOut);

		mLock.unlock();
	}

	void MMEDriver::stop()
	{
		mLock.lock();

		mQueueThread.suspend();

		waveOutReset(mWaveOut);
		waveOutPause(mWaveOut);

		mLock.unlock();
		resetWithBufferring();
	}


	bool MMEDriver::isEnabled()
	{
		mLock.lock();
		bool ret = this->mIsEnabled;
		mLock.unlock();

		return ret;
	}

	void MMEDriver::setEnabled(bool enabled)
	{
		mLock.lock();
		this->mIsEnabled = enabled;
		mLock.unlock();
	}

		bool MMEDriver::setVolume(const float vol)
	{
		float rvol = vol;
		if(vol < 0.0f)rvol = 0.0f;
		else if(vol > 1.0f)rvol = 1.0f;

		mLock.lock();
		((VolumePanEffector*)mEffectorHead)->setVolume(rvol);
		mLock.unlock();

		return TRUE;
	}

	bool MMEDriver::setPan(const float pan)
	{
		float rpan = pan;
		if(pan < -1.0f)rpan = -1.0f;
		else if(pan > 1.0f)rpan = 1.0f;

		mLock.lock();
		((VolumePanEffector*)mEffectorHead)->setPan(rpan);
		mLock.unlock();
		return TRUE;
	}

	float MMEDriver::getVolume() 
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector*)mEffectorHead)->getVolume();
		mLock.unlock();
		return ret;
	}

	float MMEDriver::getPan() 
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector*)mEffectorHead)->getPan();
		mLock.unlock();
		return ret;
	}


	void MMEDriver::threadBuffering(unsigned int bIndex)
	{
	
		mLock.lock();

		size_t readDataSize = 0;
		size_t startPos = 0;
		size_t size = 0;

		unsigned int nextSample =
			mFileDescriptor->getCurrentSample()
			+ mSoundBuffers[bIndex]->getBufferSize()
			/ mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample;

		if(mIsRepeat &&  nextSample > mLoopEndPos){
			unsigned int preSize = (mLoopEndPos - mFileDescriptor->getCurrentSample()) * mDataFormat.mChannelsPerSample * mDataFormat.mBytesPerSample;
			startPos = 0;
			size = preSize;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy(mWaveHDR[bIndex]->lpData + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);

			mFileDescriptor->seek(mLoopStartPos);
			startPos = preSize;
			size =  mSoundBuffers[bIndex]->getBufferSize() - preSize;
			readDataSize += mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy(mWaveHDR[bIndex]->lpData + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);

		}else{
			startPos = 0;
			size = 0;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy(mWaveHDR[bIndex]->lpData + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);
		}
		mLock.unlock();

		if (readDataSize == 0){
			stop();
		}else if(readDataSize < mSoundBuffers[bIndex]->getBufferSize()){
			mSoundBuffers[bIndex]->setEndPointFlag(true);
		}

		MMRESULT res = 0;
		res = waveOutWrite(mWaveOut, mWaveHDR[bIndex], sizeof(WAVEHDR));
		res = waveOutRestart(mWaveOut);
	
	}

	
	/****************************************/
	/*!
		@brief	Reset with bufferring leading data
		@note

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	void MMEDriver::resetWithBufferring()
	{
		mLock.lock();

		waveOutReset(mWaveOut);
		waveOutPause(mWaveOut);

		mFileDescriptor->reset();

		mQueueThread.clearAllRequest();
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			mQueue[i]->resetState();
			mQueueThread.addRequest(mQueue[i]);
			if(mSoundBuffers[i]->isEndPoint())break;
		}
		mLock.unlock();
	}

};	//namespace SSound


#endif //PLAYERAPI_MME