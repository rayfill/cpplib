#ifndef NETSERVICE_HPP_
#define NETSERVICE_HPP_

#include <string>

class ServiceResult
{
private:
	std::string result;
	std::string reasonMessage;

public:
	ServiceResult(const std::string& resultMessage) const throw()
	{
		size_t separetePosition = resultMessage.find(' ');

		result = resultMessage.substr(0, separetePosition);
		if (separetePosition != std::string::npos)
			reasonMessage = reasonMessage.substr(separetePosition + 1);
	}

	std::string getResult() const throw()
	{
		return result;
	}

	std::string getReason() const throw()
	{
		return reasonMessage;
	}

	/**
	 * ���݂̌��ʂ����̏�Ԃɐi��ł��ǂ����̂��̔���
	 * @return bool 
	 */
	virtual bool isAccepted() const throw() = 0;
};

class NetService
{
private:
	/**
	 * �f�t�H���g�|�[�g�ێ�
	 */
	const unsigned short defaultServicePort;

	NetService();

protected:
	/**
	 * �R���X�g���N�^
	 * @arg servicePort �T�[�r�X�̒񋟂���Ă���f�t�H���g�̃|�[�g�ԍ�
	 */
	NetService(const unsigned short servicePort) throw():
		defaultServicePort(servicePort)
	{}

	/**
	 * �f�t�H���g�̃T�[�r�X�|�[�g�ԍ��̎擾
	 * @return �T�[�r�X�̃f�t�H���g�|�[�g�ԍ�
	 */
	unsigned short getDefaultServicePort() const throw()
	{
		return defaultServicePort;
	}

public:
	/**
	 * �T�[�r�X�ւ̐ڑ�
	 * @arg serverName_ �ڑ��T�[�o�̖��O
	 * @arg servicePort_ �T�[�r�X�̒񋟂���Ă���|�[�g
	 * @return ServiceResult �ڑ��̌��ʂ������I�u�W�F�N�g
	 */
	virtual ServiceResult* connectService(
		std::string serverName_, servicePort_) = 0;

	/**
	 * �T�[�r�X�ւ̐ڑ�
	 */
	virtual ServiceResult* connectService(
		std::string serviceName_) = 0;
};

#endif /* NETSERVICE_HPP_ */
