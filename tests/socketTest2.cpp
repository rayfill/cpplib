#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <iostream>
#include <Socket/Socket.hpp>
#include <Socket/ServerSocket.hpp>
#include <Thread/Thread.hpp>
#include <windows.h>

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
//			this->shutdown(SocketImpl::write);

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
					std::cout << "end server request." << std::endl;
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

class InputWatcher :
	public Thread
{
private:
	MyServerSocket& server;
	
public:
	InputWatcher(MyServerSocket& server_):
		server(server_)
	{}

	virtual unsigned run() throw(ThreadException)
	{
		std::cout << "hit any key to quit." << std::endl;
		std::cin.get();
		server.setFinalize();
		std::cout << "server shutdown progress..." << std::endl;
		for (int i = 0; i < 30; ++i)
		{
			Thread::sleep(1000);
			std::cout << ".";
			std::cout.flush();
		}
		std::cout << std::endl;
		return 0;
	}
};

int main()
{
	SocketModule theSocket;

	MyServerSocket server;
	InputWatcher watcher(server);
	watcher.start();

	server.listen(IP(INADDR_ANY, 65432));
	server.accept();

	watcher.join();

	return 0;
}
