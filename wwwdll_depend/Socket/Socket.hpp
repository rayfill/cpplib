#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <util/Notification.hpp>
#include <Socket/IP.hpp>
#include <Socket/SocketException.hpp>
#include <Thread/ThreadException.hpp>

/**
 * ���܂��܂�Socket�̊��N���X
 */
class Socket
{
protected:
	SocketHandle socket; ///< ���̃\�P�b�g�n���h��
	timeval defaultTimeout; ///< send() �� recv() ���u���b�N�ŌĂяo
							///�����ꍇ�̃^�C���A�E�g����
	bool isClosed; /// �\�P�b�g�������Ă��邩�ǂ���

	/**
	 * �\�P�b�g�̓ǂݍ��݉\����
	 * @param sock �����ΏۂƂȂ鐶�̃\�P�b�g�n���h��
	 * @param timeout �^�C���A�E�g����(�f�t�H���g��defaultTimeout)
	 * @return ��������. true: �ǂݍ��݉\, false: �ǂݍ��ݕs�\
	 */
	bool isReadable(const SocketHandle sock,
					timeval& timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);

		timeval val = timeout;
		if (::select((SelectRange)sock + 1, &fd, 0, 0, &val) > 0)
			return true;

		return false;
	}

	/**
	 * �\�P�b�g�̏������݉\����
	 * @param sock �����ΏۂƂȂ鐶�̃\�P�b�g�n���h��
	 * @param timeout �^�C���A�E�g����(�f�t�H���g��defaultTimeout)
	 * @return ��������. true: �������݉\, false: �������ݕs�\
	 */
	bool isWritable(const SocketHandle sock,
					timeval& timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
    
		timeval val = timeout;
		if (::select((SelectRange)sock + 1, 0, &fd, 0, &val) > 0)
			return true;
		return false;
	}

	/**
	 * �R���X�g���N�^
	 * @param timedout �V�����f�t�H���g�^�C���A�E�g����
	 */
	Socket(const timeval& timedout):
		socket(), defaultTimeout(timedout), isClosed(true)
	{
		open();
	}

	/**
	 * �\�P�b�g�n���h���̍쐬
	 */
	void open() throw(SocketException)
	{
		assert(socket == static_cast<SocketHandle>(0));
		assert(isClosed == true);
	  
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (socket == static_cast<SocketHandle>(0))
			throw SocketException("can not open socket");
	}

public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	Socket() throw()
		: socket(), defaultTimeout(), isClosed(true)
	{
		defaultTimeout.tv_sec = 30;
		defaultTimeout.tv_usec = 0;
		open();
	}

	/**
	 * �\�P�b�g�n���h������̃R�s�[�R���X�g���N�^
	 */
	Socket(const SocketHandle& inheritHandle)
		: socket(inheritHandle), defaultTimeout(), isClosed(false)
	{
		defaultTimeout.tv_sec = 30;
		defaultTimeout.tv_usec = 0;
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~Socket() throw()
	{
		close();
	}

	void setTimeout(const long sec, const long usec)
	{
		defaultTimeout.tv_sec = sec;
		defaultTimeout.tv_usec = usec;
	}

	timeval getTimeout() const
	{
		return defaultTimeout;
	}

	bool isConnected() const
	{
		return !isClosed;
	}

	bool isReadable() const throw()
	{
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1;

		return this->isReadable(this->socket, timeout);
	}

	bool isWritable() const throw()
	{
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1;
		
		return this->isWritable(this->socket, timeout);
	}

	/**
	 * �\�P�b�g����̃f�[�^�̎擾
	 * @param buffer �ǂݍ��񂾃f�[�^�ւ̃|�C���^
	 * @param readSize �ǂݍ��݉\�ȍő�T�C�Y
	 * @return ���ۂɓǂݍ��܂ꂽ�f�[�^�T�C�Y
	 * @todo winsock�̂ڂ����I�Irecv�̖߂�l��size_t����Ȃ��Ȃ�āE�E�E
	 */
	size_t read(void* buffer, const size_t readSize) 
	{
		const int readed = recv(socket,
								static_cast<char*>(buffer),
								static_cast<int>(readSize), 0);
		if (readed <= 0)
		{
			close();
			open();
			throw ConnectionClosedException();
		}
		return readed;
	}

	/**
	 * �\�P�b�g����̃f�[�^�̎擾(�^�C���A�E�g����)
	 * @param buffer �ǂݍ��񂾃f�[�^�ւ̃|�C���^
	 * @param readSize �ǂݍ��݉\�ȍő�T�C�Y
	 * @return ���ۂɓǂݍ��܂ꂽ�f�[�^�T�C�Y�B0���Ԃ����ꍇ�A�����\
	 * �P�b�g���N���[�Y���ꂽ�B
	 * @exception TimeoutException �ҋ@���ԓ��Ƀ\�P�b�g�ɓǂݎ��\
	 * �ȃf�[�^�������Ă��Ȃ������ꍇ
	 */
	size_t readAsync(void* buffer, const size_t readSize)
		throw(TimeoutException, ConnectionClosedException)
	{
		if (!this->isReadable(this->socket, this->defaultTimeout))
			throw TimeoutException();
    
		return read(buffer, readSize);
	}  

	/**
	 * �\�P�b�g�ւ̃f�[�^��������
	 * @param buffer �������ރf�[�^�ւ̃|�C���^
	 * @param writeSize �������ރf�[�^�̃T�C�Y
	 * @return ���ۂɏ������܂ꂽ�f�[�^�̃T�C�Y
	 * @throw ConnectionClosedException 
	 */
	size_t write(const void* buffer, const size_t writeSize) 
	{
		const int writed = send(socket,
								static_cast<const char*>(buffer),
								static_cast<int>(writeSize), 0);
		if (writed < 0)
		{
			close();
			open();
			throw ConnectionClosedException();
		}

		return writed;
	}

	/**
	 * �\�P�b�g�ւ̃f�[�^��������(�^�C���A�E�g����)
	 * @param buffer �������ރf�[�^�ւ̃|�C���^
	 * @param writeSize �������ރf�[�^�̃T�C�Y
	 * @return ���ۂɏ������܂ꂽ�f�[�^�̃T�C�Y�B0���Ԃ����ꍇ�A�����
	 * �\�P�b�g���N���[�Y���ꂽ
	 * @exception TimeoutException �ҋ@���Ԓ��Ƀ\�P�b�g���������݉\
	 * �ɂȂ�Ȃ������ꍇ
	 */
	size_t writeAsync(const void* buffer, const size_t writeSize) 
		throw(TimeoutException, ConnectionClosedException)
	{
		if (!this->isWritable(this->socket, this->defaultTimeout))
			throw TimeoutException();
   
		return write(buffer, writeSize);
	}

	/**
	 * �\�P�b�g�n���h���̊J��
	 */
	void close() throw() 
	{
		if (socket != 0) 
		{
			SocketModule::SocketClose(socket);
			socket = static_cast<SocketHandle>(0);
			isClosed = true;
		}
	}
};

#endif /* SOCKET_HPP_ */
