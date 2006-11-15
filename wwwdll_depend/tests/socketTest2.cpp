#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <iostream>
#include <Socket/Socket.hpp>
#include <Socket/ServerSocket.hpp>
#include <windows.h>

/**
 * 
 */
template <typename ServerClass>
class MyServerWorker : public ServerWorker
{
private:
	ServerClass* parent;
	IP info;

protected:
	MyServerWorker() {}

public:
	MyServerWorker(const SocketHandle& handle) :
		ServerWorker(handle), parent() {}

	void setParent(ServerClass* parent_) throw()
	{
		this->parent = parent_;
	}

	void setConnectTarget(const IP& info_)
	{
		this->info = info_;
	}

	virtual unsigned run() throw(ThreadException)
	{
		std::cout << "catch server socket from" 
				  << this->info.getHostname()
				  << ":" << this->info.getPort()
				  << std::endl;

		char buffer[100];
		size_t readCount;
		while((readCount = this->read(buffer, 1)) != 0)
		{
			buffer[1] = 0;
			std::cout << buffer << std::endl;
			if (buffer[0] == 'q')
				break;
			else if(buffer[0] == 'x')
			{
				this->parent->setFinalize();
				break;
			}
		}

		return 0;
	}
};

class MyServerSocket :
	public ServerSocket<MyServerWorker<MyServerSocket> >
{
protected:
	typedef MyServerWorker<MyServerSocket> WorkerClass;

	virtual void createNewWorker(SocketHandle handle,
								 IP info)
		throw(std::bad_alloc, ThreadException)
	{
		WorkerClass* childThread = new WorkerClass(handle);
		childThread->setParent(this);
		childThread->setConnectTarget(info);

  		threadManager.attach(childThread);
  		childThread->start();
	}

};

int main()
{
	SocketModule theSocket;

	MyServerSocket server;
	server.prepare(IP("localhost", 5432));

	server.accept();

	return 0;
}
