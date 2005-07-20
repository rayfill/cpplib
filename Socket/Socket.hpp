#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <util/Notification.hpp>
#include <Socket/IP.hpp>
#include <Socket/SocketException.hpp>

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
	 * @arg sock �����ΏۂƂȂ鐶�̃\�P�b�g�n���h��
	 * @arg timeout �^�C���A�E�g����(�f�t�H���g��defaultTimeout)
	 * @return ��������. true: �ǂݍ��݉\, false: �ǂݍ��ݕs�\
	 */
	bool isReadable(const SocketHandle sock,
					timeval timeout = defaultTimeout) const throw()
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
	 * @arg sock �����ΏۂƂȂ鐶�̃\�P�b�g�n���h��
	 * @arg timeout �^�C���A�E�g����(�f�t�H���g��defaultTimeout)
	 * @return ��������. true: �������݉\, false: �������ݕs�\
	 */
	bool isWritable(const SocketHandle sock,
					timeval timeout = defaultTimeout) const throw()
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
	 * @arg timedout �V�����f�t�H���g�^�C���A�E�g����
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
			socket = static_cast<ScoketHandle>(0);
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
		
		return this->isWrtable(this->socket, timeout);
	}

	/**
	 * �\�P�b�g����̃f�[�^�̎擾
	 * @arg buffer �ǂݍ��񂾃f�[�^�ւ̃|�C���^
	 * @arg readSize �ǂݍ��݉\�ȍő�T�C�Y
	 * @return ���ۂɓǂݍ��܂ꂽ�f�[�^�T�C�Y
	 */
	size_t read(void* buffer, const size_t readSize) 
	{
		return recv(socket, (char*)buffer, readSize, 0);
	}

	/**
	 * �\�P�b�g����̃f�[�^�̎擾(�^�C���A�E�g����)
	 * @arg buffer �ǂݍ��񂾃f�[�^�ւ̃|�C���^
	 * @arg readSize �ǂݍ��݉\�ȍő�T�C�Y
	 * @return ���ۂɓǂݍ��܂ꂽ�f�[�^�T�C�Y�B0���Ԃ����ꍇ�A�����\
	 * �P�b�g���N���[�Y���ꂽ�B
	 * @exception TimeoutException �ҋ@���ԓ��Ƀ\�P�b�g�ɓǂݎ��\
	 * �ȃf�[�^�������Ă��Ȃ������ꍇ
	 */
	size_t readAsync(void* buffer, const size_t readSize)
		throw(TimeoutException)
	{
		if (!this->isReadable(this->socket))
			throw TimeoutException();
    
		return read(buffer, readSize);
	}  

	/**
	 * �\�P�b�g�ւ̃f�[�^��������
	 * @arg buffer �������ރf�[�^�ւ̃|�C���^
	 * @arg writeSize �������ރf�[�^�̃T�C�Y
	 * @return ���ۂɏ������܂ꂽ�f�[�^�̃T�C�Y
	 */
	size_t write(const void* buffer, const size_t writeSize) 
	{
		return send(socket, (char*)buffer, writeSize, 0);
	}

	/**
	 * �\�P�b�g�ւ̃f�[�^��������(�^�C���A�E�g����)
	 * @arg buffer �������ރf�[�^�ւ̃|�C���^
	 * @arg writeSize �������ރf�[�^�̃T�C�Y
	 * @return ���ۂɏ������܂ꂽ�f�[�^�̃T�C�Y�B0���Ԃ����ꍇ�A�����
	 * �\�P�b�g���N���[�Y���ꂽ
	 * @exception TimeoutException �ҋ@���Ԓ��Ƀ\�P�b�g���������݉\
	 * �ɂȂ�Ȃ������ꍇ
	 */
	size_t writeAsync(void* buffer, const size_t writeSize) 
		throw(TimeoutException)
	{
		if (!this->isWritable(this->socket))
			throw TimeoutException();
   
		return write(buffer, writeSize);
	}
};


/**
 * �T�[�o�T�C�h�\�P�b�g�p���[�J�[�X���b�h�N���X
 * @see PosixThread
 * @see WinThread
 * @see Socket
 */
class ServerWorker :
	public RecollectableThread<Thread>,
	public Socket,
	public Observable
{
protected:
	ServerWorker() {}
public:
	ServerWorker(const SocketHandle& handle) throw() :
		RecollectableThread<Thread>(),
		Socket(handle),
		Observable()
	{
	}
	
	virtual ~ServerWorker() throw()
	{
	}
};

/**
 * �T�[�o�\�P�b�g�N���X
 * @see Socket
 * @see ClientSocket
 */
template <typename WorkerThread> 
class ServerSocket :
	protected Socket
{
protected:
	/**
	 * ����\�ȃX���b�h�}�l�[�W��
	 */
	RecollectableThreadGroup threadManager;

	/**
	 * �T�[�o�\�P�b�g�N���X���g�̏I���\�t���O
	 */
	bool isEndable;

protected:

	/**
	 * �V���ȃ��[�J�[�X���b�h�쐬�̂��߂̃t�@�N�g�����\�b�h
	 * @arg handle �T�[�o����󂯎��A�N���C�A���g�ƂȂ������\�P�b
	 * �g�n���h��
	 * @arg info �N���C�A���g�ւ̐ڑ����
	 * @exception std::bad_alloc �t�@�N�g�����N���X�̐����Ɏ��s����
	 * @excpetion ThreadExcpetion ���̑�
	 */ 
	virtual void createNewWorker(SocketHandle handle,
							 TargetInformation /*info*/)
		throw(std::bad_alloc, ThreadException)
	{
  		WorkerThread* childThread = new WorkerThread(handle);
  		threadManager.attach(childThread);
  		childThread->start();
	}

	/**
	 * �I���X���b�h�̉��
	 * @exception ThreadException ��������X���b�h����O�I�����Ă�����
	 * ��
	 */
	virtual void endThreadCollect() throw(ThreadException)
	{
		CriticalSection lock;
		threadManager.join_recollectable();
	}
public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	ServerSocket() throw()
		: Socket(),
  		  threadManager(), 
		  isEndable(false)
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~ServerSocket() throw()
	{
		assert(isEndable);
	}

	/**
	 * �T�[�o�̐ڑ��̂��߂̑O����
	 * @arg ti �T�[�o�\�P�b�g�o�C���h�̂��߂̏��
	 * @return ���펞: true, �ُ펞: false
	 */
	bool prepare(const TargetInformation& ti)
	{
		sockaddr_in info = ti.getInetInfo();
		if(::bind(this->socket,
				  reinterpret_cast<sockaddr*>(&info),
				  sizeof(info)) != 0)
			return false;

		if(::listen(this->socket, 10) != 0)
			return false;

		return true;
	}

	/**
	 * �I���t���O�̎擾
	 */
	bool isFinalize() const throw()
	{
		CriticalSection lock;
		return this->isEndable;
	}
	
	/**
	 * �I���t���O�̃Z�b�g
	 */
	void setFinalize() throw()
	{
		CriticalSection lock;
		this->isEndable = true;
	}

	/**
	 * �T�[�o�\�P�b�g�̎�t�J�n(�u���b�N�����)
	 * �I���ɂ� setFinalize() ���g�p����B
	 * @see ServerSocket::setFinalize()
	 */
	void accept() 
	{
		for(;;)
		{
			if (this->isReadable(this->socket))
			{
				sockaddr_in addrInfo;
				int infoSize;
				SocketHandle client =
					::accept(this->socket,
							 reinterpret_cast<sockaddr*>(&addrInfo),
							 &infoSize);

				this->createNewWorker(client,
									  TargetInformation(addrInfo));
			}
			else
			{
				this->endThreadCollect();
				CriticalSection lock;
				if (this->isEndable == true)
					break;
			}
		}
		this->close();

		threadManager.join_all();
	}
};
#endif /* SOCKET_HPP_ */
