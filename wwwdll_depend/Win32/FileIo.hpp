#ifndef FILEIO_HPP_
#define FILEIO_HPP_
#include <windows.h>
#include <assert.h>
#include <exception>
#include <stdexcept>

/**
 * �t�@�C�����o�͗�O
 */
class FileIoException : public std::runtime_error
{
	/// �R���X�g���N�^
	FileIoException(): std::runtime_error("file I/O exception") {}

	/**
	 * �R���X�g���N�^
	 * @param reason ��O���R
	 */
	FileIoException(const std::string& reason): std::runtime_error(reason) {}

	/// �f�X�g���N�^
	virtual ~FileIoException() throw() {}
};

/**
 * �t�@�C���I�[�v������O
 */
class FileOpenException : public FileOpenException
{
	/// �R���X�g���N�^
	FileOpenException(): FileIoException("can not open file.") {}
};


/**
 * �t�@�C�����o�͂Ɋւ�����N���X
 */
class FileIo
{
private:
	/// Win32 �t�@�C���n���h��
	HANDLE hFile;
	
	/// ���݂̃t�@�C���ʒu
	size_t currentPoint;

	/// �t�@�C�����������݉\����\���t���O
	bool writable;
	
	/// �t�@�C�����ǂݍ��݉\����\���t���O
	bool readable;
	
public:

	/// �f�t�H���g�R���X�g���N�^
	FileIo()
		: hFile(), currentPoint(),
		  writable(), readable()
	{}

	/**
	 * �f�X�g���N�^
	 * �J���Ă����t�@�C���͎����ŃN���[�Y����܂�
	 */
	virtual ~FileIo()
	{
		close();
	}

	/**
	 * �ǂݍ��݉\����Ԃ�
	 * @return �ǂݍ��݉\����Ԃ��t���O
	 */
	bool isReadable() const throw()
	{
		return readable;
	}

	/**
	 * �������݉\����Ԃ�
	 * @return �������݉\����Ԃ��t���O
	 */
	bool isWritable() const throw()
	{
		return writable;
	}

	/**
	 * �t�@�C���̃I�[�v��
	 * @param openFilename �J���t�@�C����
	 * @param accessMode �J�����[�h �f�t�H���g�œǂݍ��݁B�������݂̏�
	 * ���� GENERIC_WRITE ���w��B�ǂݏ��������̏ꍇ�� GENERIC_READ |
	 * GENERIC_WRITE �Ǝw�肷��
	 * @param fileAttributes �t�@�C���̊�{����
	 * @param createDisposition �������݃I�[�v�����A�t�@�C�����Ȃ�����
	 * �ꍇ�̃A�N�V����
	 * @see CreateFile API
	 */
	virtual void open(const std::string& openFilename,
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

	/**
	 * �t�@�C���̃N���[�Y
	 */
	virtual void close() throw()
	{
		if (hFile != NULL)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
	}

	/**
	 * �t�@�C���̌��݂̒�����Ԃ�
	 * @return �t�@�C���̌��݂̒���
	 */
	size_t getLength() const throw()
	{
		assert(hFile != NULL);

		DWORD fileSizeHigh;
		return GetFileSize(hFile, &fileSizeHigh);
	}
};

#endif /* FILEIO_HPP_ */
