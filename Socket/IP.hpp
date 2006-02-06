#ifndef IP_HPP_
#define IP_HPP_

#include <string>
#include <cassert>
#include <exception>
#include <Socket/NativeSocket.hpp>

/**
 * �A�h���X�������s��O�N���X
 */
class  NotAddressResolvException: public std::exception
{
};

/**
 * �ڑ��Ώۏ�񖔂̓T�[�o�̃\�P�b�g����
 */
class IP
{
private:

	unsigned long internalRepresentIP; ///< �l�b�g���[�N�o�C�g�I�[�_IP
									   ///�A�h���X
	unsigned short internalRepresentPort; ///< �l�b�g���[�N�o�C�g�I�[
										  ///�_Port�i���o�[

	/**
	 *  �z�X�g������IP�A�h���X(������x�[�X)����l�b�g���[�N�o�C�g�I�[
	 * �_IP�A�h���X�ւ̕ϊ�
	 * @param ipAddress �ϊ�����IP�A�h���X���̓z�X�g��
	 * @exception NotAddressResolvException IP�A�h���X�ɕϊ��ł��Ȃ������ꍇ
	 * @return �ϊ����ꂽ�l�b�g���[�N�o�C�g�I�[�_IP�A�h���X
	 */
	unsigned long translateIp(const char* ipAddress)
		throw(NotAddressResolvException)
	{
		HostEnt* hostEntry = gethostbyname(ipAddress);
		if (hostEntry == NULL)
		{
			throw NotAddressResolvException();
		}

		assert(hostEntry->h_length == 4);

		return *(unsigned long*)(hostEntry->h_addr_list[0]);
	}

	/**
	 * �l�b�g���[�N�o�C�g�I�[�_IP�A�h���X����FQDN���ւ̕ϊ�
	 * @param ipAddress �l�b�g���[�N�o�C�g�I�[�_IP�A�h���X
	 * @return FQDN��
	 * @exception NotAddressResolvException �z�X�g���ɕϊ��ł��Ȃ������ꍇ
	 */
	std::string translateIp(const unsigned long ipAddress)
	{
		HostEnt* hostEntry =
			gethostbyaddr(reinterpret_cast<const char*>(&ipAddress),
						  sizeof(ipAddress), AF_INET);

		if (hostEntry == NULL)
			throw NotAddressResolvException();

		return std::string(hostEntry->h_name);
	}
  
public:
	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	IP() throw() :
		internalRepresentIP(), internalRepresentPort()
	{
	}

	/**
	 * �������Z�b�g�t�R���X�g���N�^
	 * @param ipAddress �}�V����
	 * @param port �|�[�g�ԍ�
	 * @exception NotAddressResolvException �}�V�����������ł��Ȃ������ꍇ
	 */
	IP(const char* ipAddress, const short port)
		throw(NotAddressResolvException):
		internalRepresentIP(), internalRepresentPort()
	{
		internalRepresentIP = translateIp(ipAddress);
		internalRepresentPort = htons(port);
	}

	/**
	 * �������Z�b�g�t�R���X�g���N�^
	 * @param ipAddress IP�A�h���X(�l�b�g���[�N�o�C�g�I�[�_)
	 * @param port �|�[�g�ԍ�(�z�X�g�o�C�g�I�[�_)
	 * @exception NotAddressResolvException �}�V�����������ł��Ȃ������ꍇ
	 */
	IP(unsigned long ipAddress, const short port)
		throw(NotAddressResolvException):
		internalRepresentIP(), internalRepresentPort() 
	{
		internalRepresentIP = ipAddress;
		internalRepresentPort = htons(port);
	}

	/**
	 * �������Z�b�g�t�R���X�g���N�^
	 * @param addrInfo sockaddr_in�\����
	 */
	IP(const sockaddr_in& addrInfo) throw()
	{
		internalRepresentIP = addrInfo.sin_addr.s_addr;
		internalRepresentPort = addrInfo.sin_port;
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~IP() throw()
	{
	}

	/**
	 * sockaddr_in �\���̂�Ԃ�
	 * @return ������񂩂�쐬���ꂽ sockaddr_in �\���́B
	 */
	sockaddr_in getInetInfo() const throw()
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = this->internalRepresentPort;
		memcpy(&addr.sin_addr,
			   &this->internalRepresentIP,
			   sizeof(this->internalRepresentIP));

		return addr;
	}

	/**
	 * �z�X�g�o�C�g�I�[�_IP�A�h���X�擾
	 * @return �z�X�g�o�C�g�I�[�_ip�A�h���X
	 */
	unsigned long getIp() throw()
	{
		return ntohl(this->internalRepresentIP);
	}

	/**
	 * IP�A�h���X���當����ւ̕ϊ�
	 * @param �z�X�g�o�C�g�I�[�_IP�A�h���X�l @see getIp()
	 * @return �ϊ����ꂽ������I�u�W�F�N�g
	 */
	static std::string getIpString(unsigned long hostSideIpReps) throw()
	{
		unsigned char ip[4];
		ip[0] = static_cast<char>((hostSideIpReps >> 24) & 0x000000ff);
		ip[1] = static_cast<char>((hostSideIpReps >> 16) & 0x000000ff);
		ip[2] = static_cast<char>((hostSideIpReps >> 8) & 0x000000ff);
		ip[3] = static_cast<char>((hostSideIpReps) & 0x000000ff);

		char buffer[16];
		sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		return std::string(buffer);
	}

	/**
	 * �z�X�g���擾
	 * @return �z�X�g��
	 * @exception NotAddressResolvException �z�X�g���������ł��Ȃ������ꍇ
	 */
	std::string getHostname() throw(NotAddressResolvException)
	{
		return translateIp(this->internalRepresentIP);
	}

	/**
	 * ����IP�A�h���X�̐ݒ�
	 * @param IP�A�h���X���̓}�V����
	 * @exception NotAddressResolvException �z�X�g���������ł��Ȃ������ꍇ
	 */
	void setIp(const char* address) throw (NotAddressResolvException)
	{
		this->internalRepresentIP = translateIp(address);
	}

	/**
	 * �|�[�g�ԍ��̎擾(�z�X�g�o�C�g�I�[�_)
	 * @return �z�X�g�o�C�g�I�[�_Port�ԍ�
	 */
	unsigned short getPort() throw()
	{
		return ntohs(this->internalRepresentPort);
	}

	/**
	 * �����|�[�g�ԍ��̐ݒ�
	 * @param Port�ԍ�(�z�X�g�o�C�g�I�[�_)
	 */
	void setPort(const unsigned short port) throw()
	{
		this->internalRepresentPort = htons(port);
	}
};

#endif /* IP_HPP_ */
