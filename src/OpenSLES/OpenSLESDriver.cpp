
#include "SSound/OpenSLES/OpenSLESDriver.h"
#include "SSound/SoundEffector.h"
#include "SSound/SoundSystem.h"

namespace SSound{

	bool OpenSLESSoundRequest::work(){
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
	void OpenSLESDriver::callback(SLAndroidSimpleBufferQueueItf bq, void *context)
	{
		OpenSLESDriver *driver = (OpenSLESDriver*)context;

		driver->mLock.lock();
		int bufferIdx = driver->mBufferQueue.front();
		driver->mBufferQueue.pop();

		driver->mQueue[bufferIdx]->resetState();

		driver->mQueueThread.addRequest(((OpenSLESDriver*)context)->mQueue[bufferIdx]);
		driver->mBufferQueue.push(bufferIdx);
		driver->mLock.unlock();

		//LOGD("buffer:%d", bufferIdx);
	}
	

	/****************************************/
	/*!
		@brief  Open
		@note

		@author Naoto Nakamura
		@date   Oct 9, 2009
	*/
	/****************************************/
	bool OpenSLESDriver::open()
	{
		SoundDriver::open();

		mQueueThread.init();
		mQueueThread.start();
		mQueueThread.suspend();


		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, STREAMING_NUM_BUFFER};
		
		SLuint32 speaker = 0;
		if(mDataFormat.mChannelsPerSample == 2) speaker = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
		else speaker = SL_SPEAKER_FRONT_CENTER;
		
		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, mDataFormat.mChannelsPerSample, mDataFormat.mSampleRate * 1000,
			mDataFormat.mBytesPerSample * 8, mDataFormat.mBytesPerSample * 8,
			speaker, SL_BYTEORDER_LITTLEENDIAN};


		SLDataSource audioSrc = {&loc_bufq, &format_pcm};

		SLEngineItf engineInterface = SoundSystem::getEngineInterface();
		SLresult result;
		const SLInterfaceID idOutput[1] = {SL_IID_ENVIRONMENTALREVERB};
		const SLboolean reqOutput[1] = {SL_BOOLEAN_FALSE};
		result = (*engineInterface)->CreateOutputMix(engineInterface, &mOutputMixObject, 1, idOutput, reqOutput);

		result = (*mOutputMixObject)->Realize(mOutputMixObject, SL_BOOLEAN_FALSE);

		//Params
		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObject};
		SLDataSink audioSnk = {&loc_outmix, NULL};

		const SLInterfaceID ids[4] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
		const SLboolean req[4] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


		//Create interfaces
		result = (*engineInterface)->CreateAudioPlayer(engineInterface, &mPlayerObject, &audioSrc, &audioSnk, 4, ids, req);

		result = (*mPlayerObject)->Realize(mPlayerObject, SL_BOOLEAN_FALSE);

		//player interface
		result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_PLAY, &mPlayerInterface);


		//callback
		result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_BUFFERQUEUE, &mPlayerBufferQueue);
		result = (*mPlayerBufferQueue)->RegisterCallback(mPlayerBufferQueue, OpenSLESDriver::callback, this);

		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			//buffer queue
			mQueue[i] = new OpenSLESSoundRequest(this, i);
			mQueue[i]->init();
		}

		//effect
		result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_EFFECTSEND, &mPlayerEffectSendInterface);

		//volume
		result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_VOLUME, &mPlayerVolumeInterface);

		//playbackrate
		result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_PLAYBACKRATE, &mPlaybackRateInterface);

		SLuint32 capa;

		result = (*mPlaybackRateInterface)->GetRateRange(mPlaybackRateInterface, 0, &mMinPlaybackRate, &mMaxPlaybackRate, &mPlaybackRateStepSize, &capa);

		result = (*mPlaybackRateInterface)->SetPropertyConstraints(mPlaybackRateInterface, SL_RATEPROP_NOPITCHCORAUDIO);



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
	void OpenSLESDriver::close()
	{
		if (mPlayerObject != NULL) {
			(*mPlayerObject)->Destroy(mPlayerObject);
			mPlayerObject = NULL;
			mPlayerInterface = NULL;
			mPlayerEffectSendInterface = NULL;
			mPlayerVolumeInterface = NULL;
			mPlaybackRateInterface = NULL;
			mPlayerBufferQueue = NULL;
		}
		
		
		if (mOutputMixObject != NULL) {
			(*mOutputMixObject)->Destroy(mOutputMixObject);
			mOutputMixObject = NULL;
			mOutputMixEnvironmentalReverb = NULL;
		}

		mQueueThread.cleanup();

		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			SAFE_DELETE(mQueue[i]);
		}

		SoundDriver::close();
	}

	
	void OpenSLESDriver::play()
	{
		mLock.lock();
		mQueueThread.resume();
		
		SLresult result = (*mPlayerInterface)->SetPlayState(mPlayerInterface, SL_PLAYSTATE_PLAYING);
		
		mLock.unlock();
	}

	void OpenSLESDriver::pause()
	{
		mLock.lock();
		mQueueThread.suspend();

		SLresult result = (*mPlayerInterface)->SetPlayState(mPlayerInterface, SL_PLAYSTATE_PAUSED);

		mLock.unlock();
	}

	void OpenSLESDriver::stop()
	{
		mLock.lock();

		mQueueThread.suspend();

		SLresult result = (*mPlayerInterface)->SetPlayState(mPlayerInterface, SL_PLAYSTATE_STOPPED);

		mLock.unlock();
		resetWithBufferring();
	}


	bool OpenSLESDriver::isEnabled()
	{
		mLock.lock();
		bool ret = this->mIsEnabled;
		mLock.unlock();

		return ret;
	}

	void OpenSLESDriver::setEnabled(bool enabled)
	{
		mLock.lock();
		this->mIsEnabled = enabled;
		mLock.unlock();
	}

	bool OpenSLESDriver::setVolume(const float vol)
	{
		float rvol = vol;
		if(vol < 0.0f)rvol = 0.0f;
		else if(vol > 1.0f)rvol = 1.0f;

		mLock.lock();
		((VolumePanEffector*)mEffectorHead)->setVolume(rvol);
		mLock.unlock();

		return TRUE;
	}

	bool OpenSLESDriver::setPan(const float pan)
	{
		float rpan = pan;
		if(pan < -1.0f)rpan = -1.0f;
		else if(pan > 1.0f)rpan = 1.0f;

		mLock.lock();
		((VolumePanEffector*)mEffectorHead)->setPan(rpan);
		mLock.unlock();
		return TRUE;
	}

	float OpenSLESDriver::getVolume()
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector*)mEffectorHead)->getVolume();
		mLock.unlock();
		return ret;
	}

	float OpenSLESDriver::getPan()
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector*)mEffectorHead)->getPan();
		mLock.unlock();
		return ret;
	}

	void OpenSLESDriver::threadBuffering(unsigned int bIndex)
	{
		mLock.lock();

		size_t readDataSize = 0;
		unsigned int startPos = 0;
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

			mFileDescriptor->seek(mLoopStartPos);
			startPos = preSize;
			size =  mSoundBuffers[bIndex]->getBufferSize() - preSize;
			readDataSize += mSoundBuffers[bIndex]->bufferData(startPos, size);

		}else{
			startPos = 0;
			size = 0;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
		}
		mLock.unlock();

		if (readDataSize == 0){
			stop();
		}else if(readDataSize < mSoundBuffers[bIndex]->getBufferSize()){
			mSoundBuffers[bIndex]->setEndPointFlag(true);
		}

		mLock.lock();
		SLresult result;
		result = (*mPlayerBufferQueue)->Enqueue(mPlayerBufferQueue, mSoundBuffers[bIndex]->getBufferHead(), readDataSize);
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
	void OpenSLESDriver::resetWithBufferring()
	{
		mLock.lock();

		SLresult result = (*mPlayerInterface)->SetPlayState(mPlayerInterface, SL_PLAYSTATE_STOPPED);

		mFileDescriptor->reset();

		mQueueThread.clearAllRequest();
		std::queue<int> empty;
		std::swap( mBufferQueue, empty );

		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			mQueue[i]->resetState();
			mQueueThread.addRequest(mQueue[i]);
			mBufferQueue.push(i);

			if(mSoundBuffers[i]->isEndPoint())break;
		}
		mLock.unlock();
	}

};	//namespace SSound


