#ifndef FILEIO_HPP_
#define FILEIO_HPP_
#include <windows.h>
#include <assert.h>
#include <exception>
#include <stdexcept>

/**
 * ファイル入出力例外
 */
class FileIoException : public std::runtime_error
{
	/// コンストラクタ
	FileIoException(): std::runtime_error("file I/O exception") {}

	/**
	 * コンストラクタ
	 * @param reason 例外理由
	 */
	FileIoException(const std::string& reason): std::runtime_error(reason) {}

	/// デストラクタ
	virtual ~FileIoException() throw() {}
};

/**
 * ファイルオープン時例外
 */
class FileOpenException : public FileOpenException
{
	/// コンストラクタ
	FileOpenException(): FileIoException("can not open file.") {}
};


/**
 * ファイル入出力に関する基底クラス
 */
class FileIo
{
private:
	/// Win32 ファイルハンドル
	HANDLE hFile;
	
	/// 現在のファイル位置
	size_t currentPoint;

	/// ファイルが書き込み可能かを表すフラグ
	bool writable;
	
	/// ファイルが読み込み可能かを表すフラグ
	bool readable;
	
public:

	/// デフォルトコンストラクタ
	FileIo()
		: hFile(), currentPoint(),
		  writable(), readable()
	{}

	/**
	 * デストラクタ
	 * 開いていたファイルは自動でクローズされます
	 */
	virtual ~FileIo()
	{
		close();
	}

	/**
	 * 読み込み可能かを返す
	 * @return 読み込み可能かを返すフラグ
	 */
	bool isReadable() const throw()
	{
		return readable;
	}

	/**
	 * 書き込み可能かを返す
	 * @return 書き込み可能かを返すフラグ
	 */
	bool isWritable() const throw()
	{
		return writable;
	}

	/**
	 * ファイルのオープン
	 * @param openFilename 開くファイル名
	 * @param accessMode 開くモード デフォルトで読み込み。書き込みの場
	 * 合は GENERIC_WRITE を指定。読み書き両方の場合は GENERIC_READ |
	 * GENERIC_WRITE と指定する
	 * @param fileAttributes ファイルの基本属性
	 * @param createDisposition 書き込みオープン時、ファイルがなかった
	 * 場合のアクション
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
	 * ファイルのクローズ
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
	 * ファイルの現在の長さを返す
	 * @return ファイルの現在の長さ
	 */
	size_t getLength() const throw()
	{
		assert(hFile != NULL);

		DWORD fileSizeHigh;
		return GetFileSize(hFile, &fileSizeHigh);
	}
};

#endif /* FILEIO_HPP_ */
