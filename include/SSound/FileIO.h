/******************************************************************/
/*!
	@file	FileIO.h
	@brief	Defined file IO objects
	@note
	@todo
	@bug	

	@author	Naoto Nakamura
	@date	Apl. 1, 2009
*/
/******************************************************************/


#ifndef SSOUND_FILEIO_H
#define SSOUND_FILEIO_H

#include "SSound/SoundCommon.h"

#include <stdio.h>

#include <fstream>

#include "SSound/IO.h"


namespace SSound{
	//////////////////////////////////////////////////////
	//				forward declarations				//
	//////////////////////////////////////////////////////
	//implemented
	class FileIO;
	class FileReader;
	class FileWriter;

	//////////////////////////////////////////////////////
	//				const declarations					//
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	//				class declarations					//
	//////////////////////////////////////////////////////
	/****************************************/
	/*!
		@class	FileProcessingObject
		@brief	Abstract object for file processing
		@note	
		@author	Naoto Nakamura
		@date	Apl. 1, 2009
	*/
	/****************************************/
	class FileIO : public IO
	{
	public:
		FileIO()
		:IO(),
		mFilePath(std::string(""))
		{
		}
		
		FileIO(const std::string &filePath)
			:IO(), mFilePath(filePath){}

	public:

		void setFilePath(const std::string &path){mFilePath = path;}
		virtual bool isOpened() {return mStream.is_open();}

		virtual bool open(int mode);
		virtual void close();

		FORCE_INLINE virtual size_t read(void *buffer, const size_t size);
		FORCE_INLINE virtual bool read(char &c);
		FORCE_INLINE virtual bool read(unsigned char &c);
		FORCE_INLINE virtual bool read(int &i);
		FORCE_INLINE virtual bool read(unsigned int &i);
		FORCE_INLINE virtual bool read(float &f);

		FORCE_INLINE virtual bool write(const std::string &buffer);
		FORCE_INLINE virtual bool write(const char *buffer);
		FORCE_INLINE virtual bool write(const char c);
		FORCE_INLINE virtual bool write(const unsigned char c);
		FORCE_INLINE virtual bool write(const int i);
		FORCE_INLINE virtual bool write(const unsigned int i);
		FORCE_INLINE virtual bool write(const float f);
		
		FORCE_INLINE virtual bool seekGet(const size_t offset, const size_t origin);
		FORCE_INLINE virtual bool seekPut(const size_t offset, const size_t origin);

		FORCE_INLINE virtual size_t tellGet();
		FORCE_INLINE virtual size_t tellPut();

		FORCE_INLINE virtual bool peek(unsigned char &c);

		FORCE_INLINE virtual bool end();
		
	protected:
		std::string mFilePath;		//<! Forcused file path
		std::fstream mStream;		//<! Input file stream
	};

	/****************************************/
	/*!
		@class	FileReader
		@brief	
		@note	
		@author	Naoto Nakamura
		@date	Apl. 1, 2009
	*/
	/****************************************/
	class FileInput : public Input
	{
	public:
		FileInput() : mIO(), mIsTextMode(TRUE){}
		FileInput(const std::string &filePath, bool isTextMode)
			:mIO(filePath), mIsTextMode(isTextMode){}

	public:
		virtual bool open(){
			int mode = 0;
			mode |= IO::READ;
			if(!mIsTextMode) mode |= IO::BINARY;
			return mIO.open(mode);
		};
		virtual void close(){mIO.close();}

		virtual bool isOpened() {return mIO.isOpened();}

		void setFilePath(const std::string &path){mIO.setFilePath(path);}

		FORCE_INLINE size_t read(void *buffer, const size_t size){return mIO.read(buffer, size);}
		FORCE_INLINE bool read(char &c){return mIO.read(c);}
		FORCE_INLINE bool read(unsigned char &c){return mIO.read(c);}
		FORCE_INLINE bool read(int &i){return mIO.read(i);}
		FORCE_INLINE bool read(unsigned int &i){return mIO.read(i);}
		FORCE_INLINE bool read(float &f){return mIO.read(f);}

		FORCE_INLINE virtual bool seek(const size_t offset, const size_t origin){return mIO.seekGet(offset, origin);}
		FORCE_INLINE virtual size_t tell(){return mIO.tellGet();}
		
		FORCE_INLINE virtual bool peek(unsigned char &c){return mIO.peek(c);}
		
		FORCE_INLINE virtual bool end(){return mIO.end();}
		
	protected:
		FileIO mIO;
		bool mIsTextMode;
	};


	/****************************************/
	/*!
		@class	FileWriter
		@brief	
		@note	
		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	class FileOutput : public Output
	{
	public:
		FileOutput() : mIO(), mIsTextMode(TRUE){}
		FileOutput(const std::string &filePath, const bool isTextMode)
			:mIO(filePath), mIsTextMode(isTextMode){}

	public:
		virtual bool open(){
			int mode = 0;
			mode |= IO::WRITE;
			if(!mIsTextMode) mode |= IO::BINARY;
			return mIO.open(mode);
		};
		virtual void close(){mIO.close();}

		virtual bool isOpened() {return mIO.isOpened();}

		void setFilePath(const std::string &path){mIO.setFilePath(path);}

		FORCE_INLINE bool write(const std::string &buffer){return mIO.write(buffer);}
		FORCE_INLINE bool write(const char *buffer){return mIO.write(buffer);}
		FORCE_INLINE bool write(const char c){return mIO.write(c);}
		FORCE_INLINE bool write(const unsigned char c){return mIO.write(c);}
		FORCE_INLINE bool write(const int i){return mIO.write(i);}
		FORCE_INLINE bool write(const unsigned int i){return mIO.write(i);}
		FORCE_INLINE bool write(const float f){return mIO.write(f);}

		FORCE_INLINE virtual bool seek(const int offset, const int origin){return mIO.seekGet(offset, origin);}
		FORCE_INLINE virtual size_t tell(){return mIO.tellPut();}
		
	protected:
		FileIO mIO;
		bool mIsTextMode;
	};


};	//namespace SSound

#include "FileIO-inl.h"

#endif //SSOUND_FILEIO_H
