#ifndef NOISE_HPP_
#define NOISE_HPP_

#include <windows.h>
#include <tchar.h>

#include <stdexcept>
#include <limits>
#include <Cryptography/PureRandom.hpp>


class Noise : public PureRandom
{
private:
	
public:
	Noise() : PureRandom()
	{}
	~Noise()
	{}

	virtual unsigned char getRandom() throw(std::exception)
	{
		LARGE_INTEGER stop;
		HANDLE tempFile = INVALID_HANDLE_VALUE;

		try
		{
			TCHAR path[MAX_PATH+1];
			if (GetTempPath(MAX_PATH, path) > MAX_PATH+1)
				throw std::logic_error("Temporary directory path is "
									   "longer than MAX_PATH.");

			TCHAR filename[MAX_PATH+1];
			if (GetTempFileName(path, _T("TMP"), 0, filename) == 0)
				throw std::logic_error("Temporary file can not created.");

			// capture air noise from disk head seek.
			tempFile =
				CreateFile(filename,
						   GENERIC_READ,
						   0,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);
			if (tempFile == INVALID_HANDLE_VALUE)
			{
				DeleteFile(filename);
				throw std::runtime_error("can not open temporary file.");
			}

			CloseHandle(tempFile);
			DeleteFile(filename);
		}
		catch (std::exception& /*e*/)
		{
			if (tempFile != INVALID_HANDLE_VALUE)
				CloseHandle(tempFile);

			throw;
		}
			
		QueryPerformanceCounter(&stop);

		return static_cast<unsigned char>(
			stop.QuadPart &
			std::numeric_limits<unsigned char>::max());
	}
};

#endif /* NOISE_HPP_ */
