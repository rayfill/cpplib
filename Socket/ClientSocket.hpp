#ifndef CLIENTSOCKET_HPP_
#define CLIENTSOCKET_HPP_

#include <Socket/Socket.hpp>
#include <Thread/ThreadException.hpp>

/**
 * �N���C�A���g�p�\�P�b�g�N���X
 *		  @see Socket
 *		  @see ServerSocket
 */
class ClientSocket : public Socket 
{
public:
	/**
	 * �R���X�g���N�^
	 */
	ClientSocket() throw() : Socket()
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~ClientSocket() throw()
	{
	}

	/**
	 * �\�P�b�g�̐ڑ�
	 * @param ip �ڑ����Internet Protocol���
	 * @exception SocketException �����ɐڑ��ł��Ȃ������ꍇ
	 * @exception NotAddressResolvException �ڑ���̏�񂪉����ł��Ȃ���
	 * ���ꍇ
	 * @see IP
	 */
	void connect(const IP& ip)
	{
		if (socket == 0)
			Socket::open();

		sockaddr_in info = ip.getInetInfo();

		if (::connect(socket, (sockaddr*)&info, sizeof(info)) != 0)
			throw SocketException((std::string("can not connect server:") +
								   ip.getHostname()).c_str());

		isClosed = false;
	}
};

#endif /* CLIENTSOCKET_HPP_ */
