#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

#include <Thread/ThreadException.hpp>
#include <Thread/CollectableThreadGroup.hpp>

/**
 * �T�[�o�T�C�h�\�P�b�g�p���[�J�[�X���b�h�N���X
 * @see PosixThread
 * @see WinThread
 * @see Socket
 */
class ServerWorker :
	public CollectableThread<Thread>,
	public Socket,
	public Observable
{
protected:
	ServerWorker() {}
public:
	ServerWorker(const SocketHandle& handle) throw() :
		CollectableThread<Thread>(),
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
	CollectableThreadGroup threadManager;

	/**
	 * �T�[�o�\�P�b�g�N���X���g�̏I���\�t���O
	 */
	bool isEndable;

protected:

	/**
	 * �V���ȃ��[�J�[�X���b�h�쐬�̂��߂̃t�@�N�g�����\�b�h
	 * @param handle �T�[�o����󂯎��A�N���C�A���g�ƂȂ������\�P�b
	 * �g�n���h��
	 * @param info �N���C�A���g�ւ̐ڑ����
	 * @exception std::bad_alloc �t�@�N�g�����N���X�̐����Ɏ��s����
	 * @excpetion ThreadExcpetion ���̑�
	 */ 
	virtual void createNewWorker(SocketHandle handle,
								 IP /*info*/)
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
	 * @param ti �T�[�o�\�P�b�g�o�C���h�̂��߂̏��
	 * @return ���펞: true, �ُ펞: false
	 */
	bool prepare(const IP& ti)
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
			if (this->isReadable(this->socket, this->defaultTimeout))
			{
				sockaddr_in addrInfo;
				int infoSize;
				SocketHandle client =
					::accept(this->socket,
							 reinterpret_cast<sockaddr*>(&addrInfo),
							 &infoSize);

				this->createNewWorker(client,
									  IP(addrInfo));
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

#endif /* SERVERSOCKET_HPP_  */
