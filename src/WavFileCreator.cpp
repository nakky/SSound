

#include "SSound/WavFileCreator.h"

namespace SSound {

	//////////////////////////////////////////////////////////////////
	//						WavFileDescriptor						//
	//////////////////////////////////////////////////////////////////
	/*************************************************/
	/*!
	@brief		Load sound data from file

	@param		data Data buffer
	@param		size Data size you want to read
	@return	Read data size

	@author	Naoto Nakamura
	@date		Dec. 11, 2009
	*/
	/*************************************************/
	size_t WavFileDescriptor::load(void *data, const size_t size)
	{
		size_t retSize = mInput->read((char*)data, size);
		mCurrentSample += (unsigned int)(retSize / mDataFormat.mChannelsPerSample / mDataFormat.mBytesPerSample);

		return retSize;
	}

	/*************************************************/
	/*!
	@brief		Seek reading position in sound file

	@param		pos Reading position
	@return	If processing is done validly,
	return true, else return false

	@author	Naoto Nakamura
	@date		Dec. 11, 2009
	*/
	/*************************************************/
	bool WavFileDescriptor::seek(unsigned int pos)
	{
		long dpos = (long)pos * (long)mDataFormat.mChannelsPerSample * (long)mDataFormat.mBytesPerSample;
		mInput->seek(dpos, std::ios::beg);
		mCurrentSample = pos;
		return TRUE;
	}

	/*************************************************/
	/*!
	@brief		Close file descriptor

	@return	If processing is done validly,
	return true, else return false

	@author	Naoto Nakamura
	@date		Dec. 11, 2009
	*/
	/*************************************************/
	bool WavFileDescriptor::close()
	{
		mInput->close();
		SAFE_DELETE(mInput);
		mCurrentSample = 0;
		return TRUE;
	}


	//////////////////////////////////////////////////////////////////
	//						WavFileCreator							//
	//////////////////////////////////////////////////////////////////
	/*************************************************/
	/*!
	@brief		Get file descriptor from file

	@param		filePath File path
	@param		fd Achieved file descriptor
	@return	If processing is done validly,
	return FR_SUCCESS, else return FR_FAIL_TO_READ

	@author	Naoto Nakamura
	@date		Dec. 11, 2009
	*/
	/*************************************************/
	SoundFileDescriptor *WavFileCreator::createFileDescriptor(Input *input)
	{
		WavFileDescriptor *wfd = new WavFileDescriptor(input);

		wfd->mInput->open();

		if (!wfd->mInput->isOpened()) { wfd->close(); delete wfd; return NULL; }

		int bufInt = 0;
		char chunkName[5] = { 0, 0, 0, 0, '\0' };
		char bufString[5] = { 0, 0, 0, 0, '\0' };

		wfd->mInput->read(bufString, sizeof(char) * 4); //"RIFF"

		wfd->mInput->read((char*)&bufInt, sizeof(char) * 4); //file size

		wfd->mInput->read(bufString, sizeof(char) * 4); //"WAVE"

		int byteOfChunk = 0;
		SoundDataFormat dataFormat;

		while(true){
			
			wfd->mInput->read(chunkName, sizeof(char) * 4); 
			wfd->mInput->read((char*)&byteOfChunk, sizeof(char) * 4); //16:Linear PCM
			
			//std::cout << "chunkName:" << chunkName << std::endl;
			
			if(strcmp(chunkName, "fmt ") == 0 || strcmp(chunkName, "FMT ") == 0){	
				//std::cout << "fmt:" << byteOfChunk << std::endl;

				short formatID = 0;
				wfd->mInput->read((char*)&formatID, sizeof(char) * 2); //format ID, PCM:1(01 00)
		
																		//Only lenear PCM is supported
				if (formatID != 1) { wfd->close(); delete wfd; return NULL; }
		
				short numChannel = 0;
				wfd->mInput->read((char*)&numChannel, sizeof(char) * 2); //Num of chnnel monaural:1(01 00) stereo:2(02 00)
		
				int samplingRate = 0;
				wfd->mInput->read((char*)&samplingRate, sizeof(char) * 4); //sampling rate. ex.44100(44 AC 00 00)
		
				int bytePerSec = 0;
				wfd->mInput->read((char*)&bytePerSec, sizeof(char) * 4); //byte per sec
		
				short blockSize = 0;
				wfd->mInput->read((char*)&blockSize, sizeof(char) * 2); //byte/sample?~channel
		
				short bitsPerSample = 0;
				wfd->mInput->read((char*)&bitsPerSample, sizeof(char) * 2); //bits per sample. ex.16bit(10 00)	

				dataFormat.mSampleRate = samplingRate;
				dataFormat.mChannelsPerSample = numChannel;
				dataFormat.mBytesPerSample = (bitsPerSample / 8);
			}else if(strcmp(chunkName, "data") == 0 || strcmp(chunkName, "DATA") == 0){
				//std::cout << "data:" << byteOfChunk << std::endl;

				dataFormat.mDataSize = byteOfChunk;
				dataFormat.mNumSample = dataFormat.mDataSize / dataFormat.mBytesPerSample / dataFormat.mChannelsPerSample;
				break;
			}else if(strcmp(chunkName, "list") == 0 || strcmp(chunkName, "LIST") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "fact") == 0 || strcmp(chunkName, "FACT") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "cue ") == 0 || strcmp(chunkName, "CUE ") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "plst") == 0 || strcmp(chunkName, "PLST") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "labl") == 0 || strcmp(chunkName, "LABL") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "note") == 0 || strcmp(chunkName, "NOTE") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "ltxt") == 0 || strcmp(chunkName, "LTXT") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "smpl") == 0 || strcmp(chunkName, "SMPL") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}else if(strcmp(chunkName, "inst") == 0 || strcmp(chunkName, "INST") == 0){
				wfd->mInput->seek(byteOfChunk, std::ios_base::cur);
			}
		}
		
		setFileFormat(*wfd, dataFormat);

		return wfd;
	}

}; //namespace SSound

