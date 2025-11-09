
#include "SSound/CoreAudio/CoreAudioDriver.h"
#include "SSound/SoundEffector.h"

namespace SSound{

	bool CoreAudioSoundRequest::work(){
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
	void CoreAudioDriver::callback(void *inUserData, AudioQueueRef inAQ, AudioQueueBufferRef buffer)
	{
		int bufferIdx = -1;
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			if((AudioQueueBufferRef)buffer == ((CoreAudioDriver*)inUserData)->mBufferID[i]){
				bufferIdx = i;
				break;
			}
		}
		
		((CoreAudioDriver*)inUserData)->mQueue[bufferIdx]->resetState();
		((CoreAudioDriver*)inUserData)->mQueueThread.addRequest(((CoreAudioDriver*)inUserData)->mQueue[bufferIdx]);
	}
	

	/****************************************/
	/*!
		@brief  Open
		@note

		@author Naoto Nakamura
		@date   Oct 9, 2009
	*/
	/****************************************/
	bool CoreAudioDriver::open()
	{
		SoundDriver::open();

		mQueueThread.init();
		mQueueThread.start();
		mQueueThread.suspend();

		AudioStreamBasicDescription outputFormat;
		
		outputFormat.mSampleRate = mDataFormat.mSampleRate;
		outputFormat.mChannelsPerFrame = mDataFormat.mChannelsPerSample;
		outputFormat.mFormatID = kAudioFormatLinearPCM;
		outputFormat.mBytesPerPacket = mDataFormat.mBytesPerSample * mDataFormat.mChannelsPerSample;
		outputFormat.mFramesPerPacket = 1;
		outputFormat.mBytesPerFrame = mDataFormat.mBytesPerSample * mDataFormat.mChannelsPerSample;
		outputFormat.mBitsPerChannel = mDataFormat.mBytesPerSample * 8;
		outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
		
		OSStatus err;
		
		mLock.lock();
		err = AudioQueueNewOutput(&outputFormat, CoreAudioDriver::callback, this, nil, nil, 0, &mAudioQueueID);
		mLock.unlock();

	
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			err = AudioQueueAllocateBuffer(mAudioQueueID, mSoundBuffers[i]->getBufferSize(), &mBufferID[i]);
			
			mQueue[i] = new CoreAudioSoundRequest(this, i);
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
	void CoreAudioDriver::close()
	{
		mLock.lock();
		if(mAudioQueueID != NULL){
			AudioQueueDispose(mAudioQueueID, true);
		}
		mLock.unlock();

		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			AudioQueueFreeBuffer(mAudioQueueID, mBufferID[i]);
		}
		
		mQueueThread.cleanup();
		
		for(int i = 0 ; i < STREAMING_NUM_BUFFER ; i++){
			SAFE_DELETE(mQueue[i]);
		}

		SoundDriver::close();
	}

	
	void CoreAudioDriver::play()
	{
		mLock.lock();
		mQueueThread.resume();
		AudioQueueStart(mAudioQueueID, nil);
		mLock.unlock();
	}

	void CoreAudioDriver::pause()
	{
		mLock.lock();
		mQueueThread.suspend();

		AudioQueuePause(mAudioQueueID);
		//AudioQueueStop(sr->mQueueID, true);

		mLock.unlock();
	}

	void CoreAudioDriver::stop()
	{
		mLock.lock();

		mQueueThread.suspend();

		AudioQueuePause(mAudioQueueID);
		//AudioQueueStop(sr->mQueueID, true);

		mLock.unlock();
		resetWithBufferring();
	}


	bool CoreAudioDriver::isEnabled()
	{
		mLock.lock();
		bool ret = this->mIsEnabled;
		mLock.unlock();

		return ret;
	}

	void CoreAudioDriver::setEnabled(bool enabled)
	{
		mLock.lock();
		this->mIsEnabled = enabled;
		mLock.unlock();
	}

	bool CoreAudioDriver::setVolume(const float vol)
	{
		float rvol = vol;
		if(vol < 0.0f)rvol = 0.0f;
		else if(vol > 1.0f)rvol = 1.0f;

		mLock.lock();
		((VolumePanEffector*)mEffectorHead)->setVolume(rvol);
		mLock.unlock();

		return TRUE;
	}

	bool CoreAudioDriver::setPan(const float pan)
	{
		float rpan = pan;
		if(pan < -1.0f)rpan = -1.0f;
		else if(pan > 1.0f)rpan = 1.0f;

		mLock.lock();
		((VolumePanEffector*)mEffectorHead)->setPan(rpan);
		mLock.unlock();
		return TRUE;
	}

	float CoreAudioDriver::getVolume()
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector*)mEffectorHead)->getVolume();
		mLock.unlock();
		return ret;
	}

	float CoreAudioDriver::getPan()
	{
		float ret = 0.0f;
		mLock.lock();
		ret = ((VolumePanEffector*)mEffectorHead)->getPan();
		mLock.unlock();
		return ret;
	}


	void CoreAudioDriver::threadBuffering(unsigned int bIndex)
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
			size_t preSize = (mLoopEndPos - mFileDescriptor->getCurrentSample()) * mDataFormat.mChannelsPerSample * mDataFormat.mBytesPerSample;
			startPos = 0;
			size = preSize;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy((char*)mBufferID[bIndex]->mAudioData + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);

			mFileDescriptor->seek(mLoopStartPos);
			startPos = preSize;
			size =  mSoundBuffers[bIndex]->getBufferSize() - preSize;
			readDataSize += mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy((char*)mBufferID[bIndex]->mAudioData + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);

		}else{
			startPos = 0;
			size = 0;
			readDataSize = mSoundBuffers[bIndex]->bufferData(startPos, size);
			memcpy((char*)mBufferID[bIndex]->mAudioData + startPos, mSoundBuffers[bIndex]->getBufferHead(), size);
		}
		mLock.unlock();

		if (readDataSize == 0){
			stop();
		}else if(readDataSize < mSoundBuffers[bIndex]->getBufferSize()){
			mSoundBuffers[bIndex]->setEndPointFlag(true);
		}

		mBufferID[bIndex]->mAudioDataByteSize = (int)readDataSize;
		OSStatus err;
		err = AudioQueueEnqueueBuffer(mAudioQueueID, mBufferID[bIndex], 0, NULL);
		err = AudioQueueStart(mAudioQueueID, nil);
	}

	
	/****************************************/
	/*!
		@brief	Reset with bufferring leading data
		@note

		@author	Naoto Nakamura
		@date	Dec. 10, 2008
		*/
	/****************************************/
	void CoreAudioDriver::resetWithBufferring()
	{
		mLock.lock();

		AudioQueueStop(mAudioQueueID, true);

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
