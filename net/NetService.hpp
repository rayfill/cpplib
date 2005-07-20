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
	 * 現在の結果が次の状態に進んでも良いものかの判定
	 * @return bool 
	 */
	virtual bool isAccepted() const throw() = 0;
};

class NetService
{
private:
	/**
	 * デフォルトポート保持
	 */
	const unsigned short defaultServicePort;

	NetService();

protected:
	/**
	 * コンストラクタ
	 * @arg servicePort サービスの提供されているデフォルトのポート番号
	 */
	NetService(const unsigned short servicePort) throw():
		defaultServicePort(servicePort)
	{}

	/**
	 * デフォルトのサービスポート番号の取得
	 * @return サービスのデフォルトポート番号
	 */
	unsigned short getDefaultServicePort() const throw()
	{
		return defaultServicePort;
	}

public:
	/**
	 * サービスへの接続
	 * @arg serverName_ 接続サーバの名前
	 * @arg servicePort_ サービスの提供されているポート
	 * @return ServiceResult 接続の結果を示すオブジェクト
	 */
	virtual ServiceResult* connectService(
		std::string serverName_, servicePort_) = 0;

	/**
	 * サービスへの接続
	 */
	virtual ServiceResult* connectService(
		std::string serviceName_) = 0;
};

#endif /* NETSERVICE_HPP_ */
