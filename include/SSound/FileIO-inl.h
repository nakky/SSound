/******************************************************************/
/*!
	@file	FileIO-inl.h
	@brief	Defined FORCE_INLINE functions of file IO objects
	@note
	@todo
	@bug	

	@author	Naoto Nakamura
	@date	Apl. 1, 2009
*/
/******************************************************************/


#ifndef SSOUND_FILEIO_INL_H
#define SSOUND_FILEIO_INL_H

namespace SSound{
	//////////////////////////////////////////////////////
	//					FileIO						//
	//////////////////////////////////////////////////////
	/****************************************/
	/*!
		@brief	Read data into buffer with size
		@note	

		@param	buffer Buffer for reading
		@param	size Read data size
		@return	If the file pointer is at the end
				of file return FALSE, else return TRUE
			
		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE size_t FileIO::read(void *buffer, const size_t size)
	{
		if(mStream.eof())return false;
		
		mStream.read((char*)buffer, size);

		return mStream.gcount();
	}

	/****************************************/
	/*!
		@brief	Read a charactor
		@note	
		
		@param	c Charactor buffer for reading
		@return	If the file pointer is at the end
				of file return FALSE, else return TRUE
			
		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::read(char &c)
	{
		if(mStream.eof())return false;

		mStream >> c;
		
		return true;
	}

	/****************************************/
	/*!
		@brief	Read a charactor
		@note	
		
		@param	c Charactor buffer for reading
		@return	If the file pointer is at the end
				of file return FALSE, else return TRUE
			
		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::read(unsigned char &c)
	{
		if(mStream.eof())return false;

		mStream >> c;
		
		return true;
	}

	/****************************************/
	/*!
		@brief	Read a integer
		@note	
			
		@param	i Integer buffer for reading
		@return	If the file pointer is at the end
				of file return FALSE, else return TRUE

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::read(int &i)
	{
		if(mStream.eof())return false;

		mStream >> i;
		
		return true;
	}

	/****************************************/
	/*!
		@brief	Read a integer
		@note	
			
		@param	i Integer buffer for reading
		@return	If the file pointer is at the end
				of file return FALSE, else return TRUE

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::read(unsigned int &i)
	{
		if(mStream.eof())return false;

		mStream >> i;
		
		return true;
	}

	/****************************************/
	/*!
		@brief	Read a float value
		@note	

		@param	f Float buffer for reading
		@return	If the file pointer is at the end
				of file return FALSE, else return TRUE

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::read(float &f)
	{
		if(mStream.eof())return false;

		mStream >> f;
		
		return true;
	}

	//////////////////////////////////////////////////////
	//					FileIO						//
	//////////////////////////////////////////////////////
	/****************************************/
	/*!
		@brief	Write data
		@note	
			
		@param	buffer Buffer which has writed data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const std::string &buffer)
	{
		mStream << buffer.c_str();
		return true;
	}

	/****************************************/
	/*!
		@brief	Write data
		@note	
			
		@param	buffer Pointer to buffer which has writed data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const char *buffer)
	{
		mStream << buffer;
		return true;
	}

	/****************************************/
	/*!
		@brief	Write charctor data
		@note	
			
		@param	c Writed charactor data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const char c)
	{
		mStream << c;
		return true;
	}

	/****************************************/
	/*!
		@brief	Write charctor data
		@note	
			
		@param	c Writed charactor data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const unsigned char c)
	{
		mStream << c;
		return true;
	}

	/****************************************/
	/*!
		@brief	Write integer data
		@note	
			
		@param	i Writed integer data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const int i)
	{
		mStream << i;
		return true;
	}

	/****************************************/
	/*!
		@brief	Write integer data
		@note	
			
		@param	i Writed integer data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const unsigned int i)
	{
		mStream << i;
		return true;
	}

	/****************************************/
	/*!
		@brief	Write floating value data
		@note	
			
		@param	f Writed float data

		@author	Naoto Nakamura
		@date	Apr. 1, 2009
	*/
	/****************************************/
	FORCE_INLINE bool FileIO::write(const float f)
	{
		mStream << f;
		return true;
	}
	
	/****************************************/
	/*!
		@brief	Seek
		@note
		
		@param	f Writed float data
		
		@author	Naoto Nakamura
		@date	Apr. 1, 2009
		*/
	/****************************************/
	FORCE_INLINE bool FileIO::seekGet(const size_t offset, const size_t origin)
	{
		mStream.seekg(offset, (std::ios::seekdir)origin);
		return true;
	}
	
	FORCE_INLINE bool FileIO::seekPut(const size_t offset, const size_t origin)
	{
		mStream.seekp(offset, (std::ios::seekdir)origin);
		return true;
	}
	
	FORCE_INLINE size_t FileIO::tellGet()
	{
		return (int)mStream.tellg();
	}
	
	FORCE_INLINE size_t FileIO::tellPut()
	{
		return (int)mStream.tellg();
	}

	
	FORCE_INLINE bool FileIO::peek(unsigned char &c)
	{
		c = mStream.peek();
		return true;
	}

	FORCE_INLINE bool FileIO::end()
	{
		return mStream.eof();
	}

};	//namespace SSound

#endif //SSOUND_FILEIO_INL_H
