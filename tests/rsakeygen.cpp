#include <iostream>
#include <Cryptography/RSA.hpp>
#include <util/Notification.hpp>


class eventHandler : public Observer
{
public:
	void notify(Observable* notifier)
	{
		std::cout << ".";
		std::cout.flush();
	}
};

int usage()
{
	std::cout << "rsakeygen is RSA key Generator." << std::endl;
	std::cout << "usage: rsakeygen number_of_key_bits" << std::endl;
	
	return 1;
}

int main(int argc, char** argv)
{
	if (argc != 2)
		return usage();

	int bits = atoi(argv[1]);
	if ((bits < 128) && (bits % 8 != 0))
		return usage();

	try 
	{
		KeyPair pair = RSA::makeKeyPair(bits);

		std::cout << pair.getPublicKey() << std::endl;
		std::cout << pair.getPrivateKey() << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
