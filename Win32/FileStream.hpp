#ifndef FILESTERAM_HPP_
#define FILESTERAM_HPP_

#include <Win32/FileIo.hpp>

class FileStream : public FileIo
{
private:
	std::vector<unsigned char>* buffer;
	size_t length;

public:
	FileStream()
		: buffer(), length()
	{
	}

	virtual ~FileStream() throw()
	{
	}

	class FileStreamIterator
	{

	};

	typedef FileStreamIterator iterator;
	virtual void close() throw()
	{
		if (buffer != NULL)
			delete buffer;
		buffer = NULL;
		
	}

	virtual void open(std::string openFilename,
			  DWORD accessMode = GENERIC_READ,
			  DWORD fileAttributes = 0,
			  DWORD creationDisposition = OPEN_EXISTING)
		throw(FileOpenException, std::bad_alloc)
	{
		bool result = FileIo::open(openFilename,
								   accessMode,
								   fileAttributes,
								   creationDisposition);

		length = getLength();
		
	}
};

#endif /* FILESTERAM_HPP_ */
