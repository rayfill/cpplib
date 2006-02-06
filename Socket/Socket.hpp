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

	/**
	 * �\�P�b�g�̓ǂݍ��݉\����
	 * @param sock �����ΏۂƂȂ鐶�̃\�P�b�g�n���h��
	 * @param timeout �^�C���A�E�g����(�f�t�H���g��defaultTimeout)
	 * @return ��������. true: �ǂݍ��݉\, false: �ǂݍ��ݕs�\
	 */
	bool isReadable(const SocketHandle sock,
					timeval timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
    
		if (::select((SelectRange)sock + 1, &fd, 0, 0, &timeout))
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
					timeval timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
    
		if (::select((SelectRange)sock + 1, 0, &fd, 0, &timeout))
			return true;
		return false;
	}

	/**
	 * �R���X�g���N�^
	 * @param timedout �V�����f�t�H���g�^�C���A�E�g����
	 */
	Socket(const timeval& timedout):
		socket(), defaultTimeout(timedout) 
	{
		open();
	}

	/**
	 * �\�P�b�g�n���h���̍쐬
	 */
	void open() throw(SocketException)
	{
		assert(socket == static_cast<SocketHandle>(0));
	  
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (socket == static_cast<SocketHandle>(0))
			throw SocketException();
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
		}
	}

public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	Socket() throw()
		: socket(), defaultTimeout()
	{
		defaultTimeout.tv_sec = 1;
		defaultTimeout.tv_usec = 0;
		open();
	}

	/**
	 * �\�P�b�g�n���h������̃R�s�[�R���X�g���N�^
	 */
	Socket(const SocketHandle& inheritHandle)
		: socket(inheritHandle), defaultTimeout()
	{
		defaultTimeout.tv_sec = 1;
		defaultTimeout.tv_usec = 0;
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~Socket() throw()
	{
		close();
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
	 */
	size_t read(void* buffer, const size_t readSize) 
	{
		return recv(socket, (char*)buffer, readSize, 0);
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
		throw(TimeoutException)
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
	 */
	size_t write(const void* buffer, const size_t writeSize) 
	{
		return send(socket, (char*)buffer, writeSize, 0);
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
	size_t writeAsync(void* buffer, const size_t writeSize) 
		throw(TimeoutException)
	{
		if (!this->isWritable(this->socket, this->defaultTimeout))
			throw TimeoutException();
   
		return write(buffer, writeSize);
	}
};

#endif /* SOCKET_HPP_ */
