
#include "SSound/FileIO.h"

namespace SSound{

	/****************************************/
	/*!
		@brief	Open file
		@note

		@return If file is open validly, return TRUE,
				else return FALSE

		@author Naoto Nakamura
		@date Apr. 1, 2009
	*/
	/****************************************/
	bool FileIO::open(int mode)
	{
		if(!IO::open(mode))return FALSE;
		if(mStream.is_open())return FALSE;

		mStream.open(mFilePath.data(), (std::ios::openmode)mode);

		return mStream.is_open();
	}

	/****************************************/
	/*!
		@brief Close file
		@note

		@author Naoto Nakamura
		@date Apr. 1, 2008
	*/
	/****************************************/
	void FileIO::close()
	{
		if(!mStream.is_open())return;
		mStream.close();
		mStream.clear();
	}

};	//namespace SSound
