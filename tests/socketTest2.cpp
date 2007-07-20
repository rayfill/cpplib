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
		try
		{
			this->shutdown(SocketImpl::write);

			try
			{
				std::cout << "catch server socket from: " 
						  << this->info.getHostname()
						  << ":" << this->info.getPort()
						  << std::endl;
			}
			catch (std::exception& )
			{
				std::cout << "catch server socket from: " 
						  << IP::getIpString(this->info.getIp())
						  << ":" << this->info.getPort()
						  << std::endl;
			}
		
			char buffer[100];
			size_t readCount;
			while((readCount = this->read(buffer, 1)) != 0)
			{
				if (buffer[0] == 'q')
					break;
				else if(buffer[0] == 'x')
				{
					this->parent->setFinalize();
					break;
				}

				buffer[1] = 0;
				std::cout << buffer;
				std::cout.flush();
			}
		}
		catch (std::exception& e)
		{
			std::cout << "raise exception: " << e.what() << std::endl;
			std::cout << "connection closed." << std::endl;
			this->shutdown(SocketImpl::read);
			this->close();
			throw ThreadException(e.what());
		}
		
		this->shutdown(SocketImpl::read);
		this->close();

		return 0;
	}
};

class MyServerSocket :
	public ServerSocket<MyServerWorker<MyServerSocket>,
	CollectableThreadGroup, 10>
{
protected:
	typedef MyServerWorker<MyServerSocket> WorkerClass;
	typedef ServerSocket<MyServerWorker<MyServerSocket>,
						 CollectableThreadGroup, 10> parent_t;

	virtual void createNewWorker(SocketHandle handle,
								 const IP& info)
		throw(std::bad_alloc, ThreadException)
	{
		WorkerClass* childThread = new WorkerClass(handle);
		childThread->setParent(this);
		childThread->setConnectTarget(info);

  		parent_t::threadManager.attach(childThread);
  		childThread->start();
	}

};

int main()
{
	SocketModule theSocket;

	MyServerSocket server;
	server.listen(IP(INADDR_ANY, 5432));

	server.accept();

	return 0;
}
