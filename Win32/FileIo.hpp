#ifndef FILEIO_HPP_
#define FILEIO_HPP_
#include <windows.h>
#include <assert.h>
#include <exception>
#include <stdexcept>

class FileIoException : public std::runtime_error
{
	FileIoException(): std::runtime_error("file I/O exception") {}
	FileIoException(std::string reason): std::runtime_error(reason) {}
};

class FileOpenException : public FileOpenException
{
	FileOpenException(): FileIoException("can not open file.") {}
};

class FileIo
{
private:
	HANDLE hFile;
	size_t currentPoint;
	bool writable;
	bool readable;
	
public:
	FileIo()
		: hFile(), currentPoint(),
		  writable(), readable()
	{}

	virtual ~FileIo()
	{
		close();
	}

	bool isReadable() const throw()
	{
		return readable;
	}

	bool isWritable() const throw()
	{
		return writable;
	}

	virtual void open(std::string openFilename,
					  DWORD accessMode = GENERIC_READ,
					  DWORD fileAttributes = 0,
					  DWORD creationDisposition = OPEN_EXISTING)
		throw(FileOpenException)
	{
		assert (hFile == NULL);

		hFile = CreateFile(openFilename.c_str(),
						   accessMode,
						   FILE_SHARE_READ,
						   0,
						   creationDisposition,
						   fileAttrubutes,
						   NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hFile = NULL;
			throw FileOpenException();
		}

		readable = accessMode & GENERIC_READ ? true : false;
		writable = accessMode & GERERIC_WRITE ? true : false;
	}

	virtual void close() throw()
	{
		if (hFile != NULL)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
	}

	size_t getLength() const throw()
	{
		assert(hFile != NULL);

		DWORD fileSizeHigh;
		return GetFileSize(hFile, &fileSizeHigh);
	}
};

#endif /* FILEIO_HPP_ */
