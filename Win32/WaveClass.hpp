#ifndef WAVECLASS_HPP_
#define WAVECLASS_HPP_

#include <windows.h>
#include <mmsystem.h>
#include <memory.h>
#include <cassert>
#include <stdexcept>
#include <exception>

class BadFileException : public std::runtime_error
{
public:
	BadFileException()
	{}
	
	virtual ~BadFileException() throw()
	{}

	virtual const char* what() const throw()
	{
		return "open file type is bad.";
	}
};

template <typename ContainerType>
class WaveFileIterator 
{
public:
	typedef ContainerType T;
	typedef char ReturnType;

private:
	size_t currentPointer;
	T* container;
public:
	WaveFileIterator(): currentPointer() {}

	WaveFileIterator(size_t currentPointer_, T* container_):
			currentPointer(currentPointer_), container(container_)
	{}

	WaveFileIterator(const WaveFileIterator& itor)
	{
		if (&itor == this)
			return;

		this->currentPointer = itor.currentPointer;
		this->container = itor.container;
	}
		
	~WaveFileIterator()
	{}
		
	bool operator!=(WaveFileIterator& itor)
	{
		return !(this->container == itor.container &&
				 this->currentPointer == itor.currentPointer);
	}

	bool operator==(WaveFileIterator& itor)
	{
		return !this->operator!=(itor);
	}

	WaveFileIterator& operator++()
	{
		++currentPointer;
		return *this;
	}

	WaveFileIterator operator++(int)
	{
		WaveFileIterator before = *this;
		this->currentPointer += 1;
		return before;
	}

	ReturnType operator*()
	{
		container->seek(currentPointer);
		return container->read();
	}
};

class WaveFile 
{
private:
	struct WaveFileHeader 
	{
	private:
		bool isRIFFChunk()
		{ return strncmp(this->pRiffChunkId, "RIFF", 4) == 0; }

		bool isWAVEfmtChunk()
		{ return strncmp(this->pWaveTag, "WAVEfmt ", 8) == 0; }

		bool isDATAChunk()
		{ return strncmp(this->pDataChunkId, "data", 4) == 0; }

		bool isValidDataSize(HANDLE hFile)
		{
			assert(hFile != NULL);

			BY_HANDLE_FILE_INFORMATION fileInfo;
			GetFileInformationByHandle(hFile, &fileInfo);
			const size_t fileSize = fileInfo.nFileSizeLow;

			if ((nRiffChunkSize + 8) == fileSize &&
				nFmtChunkSize == 16 &&
				pDataChunkSize <= (fileSize - 44))
				return true;
			return false;
		}
		bool isValidDataRelation()
		{
			if (nSamplesPerSec * nBlockAlign != nAvgBytesPerSec)
				return false;

			if ((nChannels * nBitsPerSample >> 3) != nBlockAlign)
				return false;

			return true;
		}

	public:
		WAVEFORMATEX getWaveFormat()
		{
			WAVEFORMATEX waveFormat;
			memset(&waveFormat, 0, sizeof(waveFormat));
			waveFormat.wFormatTag = nFormatTag;
			waveFormat.nChannels = nChannels;
			waveFormat.nSamplesPerSec = nSamplesPerSec;
			waveFormat.nAvgBytesPerSec = nAvgBytesPerSec;
			waveFormat.nBlockAlign = nBlockAlign;
			waveFormat.wBitsPerSample = nBitsPerSample;
			waveFormat.cbSize = 0;

			return waveFormat;
		}

		bool isValidate(HANDLE hFile)
		{
			if (!isRIFFChunk() ||
				!isWAVEfmtChunk() ||
				!isDATAChunk())
				return false;

			if (!isValidDataSize(hFile))
				return false;

			if (!isValidDataRelation())
				return false;

			return true;
		}

		size_t getDataOffset()
		{
			return 44;
		}
		
		size_t getLastDataOffset(HANDLE hFile)
		{
			return getDataOffset() + this->pDataChunkSize;
		}

		char pRiffChunkId[4];
		UINT32 nRiffChunkSize;
		char pWaveTag[4];
		char pFmtChunkTag[4];
		UINT32 nFmtChunkSize;
		WORD nFormatTag;
		WORD nChannels;
		UINT32 nSamplesPerSec;
		UINT32 nAvgBytesPerSec;
		WORD nBlockAlign;
		WORD nBitsPerSample;
		char pDataChunkId[4];
		UINT32 pDataChunkSize;
	}; // sizeof(WaveFileHeader) == 44

	HANDLE hFile;

	WaveFileHeader getWaveFileHeader()
	{
		assert(hFile != NULL);

		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0xffffffff)
			throw std::exception();

		WaveFileHeader wfh;
		DWORD readSize;
		ReadFile(hFile, &wfh, 44, &readSize, NULL);

		if (readSize != 44)
			throw BadFileException();

		return wfh;
	}

protected:
	size_t getPCMOffset()
	{
		assert(hFile != NULL);


		WaveFileHeader wfh = getWaveFileHeader();
		
		if (!wfh.isValidate(hFile))
			throw BadFileException();

		return wfh.getDataOffset();
	}

public:
	typedef WaveFileIterator<WaveFile> iterator;

	WaveFile(): hFile(NULL)
	{}

	WaveFile(std::string filename): hFile(NULL)
	{
		open(filename);
	}

	virtual ~WaveFile()
	{
		close();
	}

	bool open(std::string filename)
	{
		assert(hFile == NULL);

		hFile = CreateFile(filename.c_str(),
						   GENERIC_READ,
						   0,
						   NULL,
						   OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL,
						   NULL);
		if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
		{
			hFile = NULL;
			return false;
		}

		return true;
	}

	void close()
	{
		if (hFile != NULL)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
	}

	void seek(size_t point)
	{
		SetFilePointer(hFile, point, NULL, FILE_BEGIN);
	}

	char read()
	{
		assert(hFile != NULL &&
			   hFile != INVALID_HANDLE_VALUE);

		char retValue;
		DWORD readSize;

		ReadFile(hFile, &retValue, 1, &readSize, NULL);

		if (readSize == 0)
			throw std::out_of_range("out of bound to WaveFile::read()");

		return retValue;
	}

	WAVEFORMATEX getWaveFormat()
	{
		return getWaveFileHeader().getWaveFormat();
	}

	iterator begin()
	{
		size_t offset = getPCMOffset();
		return iterator(offset, this);
	}

	iterator end()
	{
		return iterator(
			getWaveFileHeader().getLastDataOffset(hFile), this);
	}
};

template <typename Itor>
class WaveOutDevice 
{
private:
 	enum { numberOfData = 4 };
	unsigned int numberOfProcessing;
	const size_t maxPCMBufferSize;
	bool playDone;
	UINT deviceId;
	HWAVEOUT waveOutHandle;
	WAVEHDR pcmData[numberOfData];
	WAVEFORMATEX waveFormat;
	
	Itor currentPoint;
	Itor endPoint;

	/// @return value: last buffer processed.
	bool overflow()
	{
		++this->numberOfProcessing;
		if (this->numberOfProcessing >= numberOfData)
			this->numberOfProcessing = 0;

		size_t copied =
			this->pcmCopy(this->pcmData[this->numberOfProcessing].lpData);
		this->pcmData[this->numberOfProcessing].dwBufferLength = copied;

		return copied != 0;
	}

	size_t pcmCopy(LPSTR buffer)
	{
		assert(this->pcmData != NULL);

		size_t processOfBytes = 0;

		while(this->currentPoint != this->endPoint &&
			  processOfBytes < this->maxPCMBufferSize)
			buffer[processOfBytes++] = *currentPoint++;

		return processOfBytes;
	}
	
	bool prepare()
	{
		for(unsigned int index = 0; index < numberOfData; ++index)
		{
			::memset(&this->pcmData[index], 0, sizeof(this->pcmData[index]));
			
			this->pcmData[index].lpData =
				new char[this->maxPCMBufferSize]();
			this->pcmData[index].dwBufferLength = this->maxPCMBufferSize;
			
			MMRESULT result = ::waveOutPrepareHeader(
				this->waveOutHandle, 
				&this->pcmData[index],
				sizeof(this->pcmData[index]));
			if (result != MMSYSERR_NOERROR)
				return false;
		}
		return true;
	}

	void unprepare()
	{
		for(unsigned int index = 0; index < numberOfData; ++index)
		{
			::waveOutUnprepareHeader(
				this->waveOutHandle, 
				&this->pcmData[index],
				sizeof(this->pcmData[index]));

			delete[] this->pcmData[index].lpData;
		}
	}

	WAVEFORMATEX getWaveFormat()
	{
		return this->waveFormat;
	}

public:
	WaveOutDevice(UINT DeviceID = WAVE_MAPPER,
				  size_t bufferSize = 176400):
			numberOfProcessing(static_cast<unsigned int>(numberOfData)),
			maxPCMBufferSize(bufferSize),
			playDone(false),
			deviceId(DeviceID), 
			waveOutHandle(NULL),
			waveFormat(),
			currentPoint(),
			endPoint()
	{
	}

	void setPCMData(Itor beginPoint, Itor endPoint_)
	{
		this->currentPoint = beginPoint;
		this->endPoint = endPoint_;
	}

	void setPCMData(Itor beginPoint, size_t length)
	{
		this->currentPoint = beginPoint;
		this->endPoint = beginPoint + length;
	}

	bool open()
	{
		assert(this->waveOutHandle == NULL);

		WAVEFORMATEX waveFormat = this->getWaveFormat();
		MMRESULT result = ::waveOutOpen(&this->waveOutHandle,
										this->deviceId,
										&waveFormat,
										(DWORD)CallbackEntryPoint,
										reinterpret_cast<DWORD>(this),
										WAVE_ALLOWSYNC | CALLBACK_FUNCTION);
		if (result != MMSYSERR_NOERROR)
			return false;

		this->prepare();
		return this->overflow();
	}

	bool play()
	{
		MMRESULT result =
			::waveOutWrite(this->waveOutHandle, 
						   &this->pcmData[this->numberOfProcessing],
						   sizeof(this->pcmData[this->numberOfProcessing]));
		if (result != 0)
			return false;

		this->overflow();
		result =
			::waveOutWrite(this->waveOutHandle, 
						   &this->pcmData[this->numberOfProcessing],
						   sizeof(this->pcmData[this->numberOfProcessing]));
		if (result != 0)
			return false;
		return true;
	}

	bool isPlayEnd()
	{
		return this->playDone;
	}
	
	void close()
	{
		if (this->waveOutHandle != NULL)
		{
			this->unprepare();
			::waveOutClose(this->waveOutHandle);
			this->waveOutHandle = NULL;
		}
	}

	void setWaveFormat(const WAVEFORMATEX& wfe)
	{
		this->waveFormat = wfe;
	}

protected:
	virtual void Callback(UINT uMessage, DWORD dwParam1, DWORD dwParam2)
	{
		switch (uMessage) 
		{
			case WOM_DONE:
			{
				LPWAVEHDR pWaveHeader = reinterpret_cast<LPWAVEHDR>(dwParam1);
				if (pWaveHeader->dwBufferLength == this->maxPCMBufferSize)
				{
					if (this->overflow())
						::waveOutWrite(
							this->waveOutHandle,
							&this->pcmData[this->numberOfProcessing],
							sizeof(this->pcmData[this->numberOfProcessing]));
	   
				}
				else
					this->playDone = true;
				break;
			}
			case WOM_OPEN:
			{
		
				break;
			}
			default:
			{
				break;
			}
		}
	}

	static void CALLBACK CallbackEntryPoint(HWAVE hWave,
											UINT uMsg,
											DWORD dwInstance,
											DWORD dwParam1,
											DWORD dwParam2)
	{
		WaveOutDevice* pThis = reinterpret_cast<WaveOutDevice*>(dwInstance);
		pThis->Callback(uMsg, dwParam1, dwParam2);
	}
};

#endif /* WAVECLASS_HPP_ */
