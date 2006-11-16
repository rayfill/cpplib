#ifndef RSA_HPP_
#define RSA_HPP_

#include <math/MPI.hpp>
#include <math/theory/AlgebraTheory.hpp>
#include <math/theory/NumberTheory.hpp>
#include <Cryptography/AsymmetricKey.hpp>
#include <Cryptography/SecureRandom.hpp>
#include <IO/Endian.hpp>
#include <iterator>

/**
 * RSA暗号
 */
class RSA
{
	friend class RSATest;

private:
	PublicKey publicKey;
	PrivateKey privateKey;

	static std::vector<unsigned char> 
	makeBigEndianVector(const void* pointer,
						const size_t length,
						const size_t vectorLength)
		throw(std::overflow_error, std::invalid_argument)
	{
		const size_t intSize = sizeof(int);
		if (length > vectorLength - intSize)
			throw std::overflow_error("buffer overflow.");

		if (vectorLength % sizeof(MPInteger::BaseUnit) != 0)
			throw std::invalid_argument(
				"integerLength が 多倍長整数クラスのパラメータ、"
				"BaseUnitの倍数ではありません.");

		std::vector<unsigned char> buffer(vectorLength);

		// マシンのエンディアンからビッグエンディアンへ変換
		EndianConverter converter;
		const size_t contentLength =
			converter.toBig(static_cast<unsigned int>(length));
		for (size_t count = 0; count < intSize; ++count)
			buffer[count] =
				reinterpret_cast<const unsigned char*>(&contentLength)[count];

		// 0xffでパディング埋め
		const size_t paddingSize = vectorLength - length - intSize;
		for (size_t offset = 0; offset < paddingSize; ++offset)
			buffer[intSize+offset] = 0xff;

		// データのコピー
		for (size_t offset = 0; offset < length; ++offset)
			buffer[intSize+paddingSize+offset] =
				reinterpret_cast<const unsigned char*>(pointer)[offset];

		return buffer;
	}

public:
	RSA(const PublicKey& publicKey_) throw()
		: publicKey(publicKey_), 
		  privateKey()
	{}

	RSA(const PrivateKey& privateKey_) throw()
		: publicKey(),
		  privateKey(privateKey_)
	{}

	RSA(const PublicKey& publicKey_,
		const PrivateKey& privateKey_) throw()
		: publicKey(publicKey_),
		  privateKey(privateKey_)
	{}

	RSA(const KeyPair& pair) throw()
		: publicKey(pair.getPublicKey()),
		  privateKey(pair.getPrivateKey())
	{}

	MPInteger encrypt(const MPInteger& plaintext)
	{
		if(plaintext >= publicKey.getModulus())
			throw std::invalid_argument(
				"平文の長さがブロックサイズを超えています.");

		return modulusExponential(
			plaintext,
			publicKey.getEncryptExponent(),
			publicKey.getModulus());
	}

	MPInteger decrypt(const MPInteger& ciphertext)
	{
		if(ciphertext >= privateKey.getModulus())
			throw std::invalid_argument(
				"暗号文の長さがブロックサイズを超えています.");

		if (privateKey.isCRTSupport())
			return crtModulusExponential(ciphertext,
										 privateKey.getDecryptExponent(),
										 privateKey.getPrimeP(),
										 privateKey.getPrimeQ(),
										 privateKey.getCRTP(),
										 privateKey.getCRTQ(),
										 privateKey.getModulus());
		return modulusExponential(
			ciphertext,
			privateKey.getDecryptExponent(),
			privateKey.getModulus());
	}

	static std::vector<unsigned char> mpiToVector(const MPInteger& integer)
	{
		std::vector<unsigned char> byteVector = integer.toBigEndianMemory();

		EndianConverter converter;
		int vectorSize =
			converter.fromBig(*reinterpret_cast<int*>(&byteVector.front()));

		const size_t headOffset = byteVector.size() - vectorSize;
		return std::vector<unsigned char>(byteVector.begin() + headOffset,
										  byteVector.end());
	}

	static bool mpiToMemory(const MPInteger& integer,
							void* memory,
							size_t& writedMemorySize)
	{
		std::vector<unsigned char> byteVector = integer.toBigEndianMemory();

		EndianConverter converter;
		const size_t memorySize =
			converter.fromBig(*reinterpret_cast<int*>(&byteVector.front()));
		if (memorySize > writedMemorySize)
		{
			writedMemorySize = memorySize;
			return false;
		}

		const size_t headOffset = byteVector.size() - memorySize;
		std::copy(byteVector.begin() + headOffset,
				  byteVector.end(),
				  reinterpret_cast<unsigned char*>(memory));
		writedMemorySize = memorySize;
		return true;
		
	}

	static std::string mpiToString(const MPInteger& integer)
	{
		std::vector<unsigned char> byteVector = integer.toBigEndianMemory();

		EndianConverter converter;
		int stringSize =
			converter.fromBig(*reinterpret_cast<int*>(&byteVector.front()));

		const size_t headOffset = byteVector.size() - stringSize;

		return std::string(
			reinterpret_cast<char*>(&byteVector[headOffset]),
			stringSize);
	}

	static MPInteger memoryToMPI(void* addressToHead, const size_t length,
								 const size_t vectorLength = 1024 / 8)
	{
		return MPInteger().fromBigEndianMemory(
			makeBigEndianVector(addressToHead, length, vectorLength));
	}

	static MPInteger vectorToMPI(const std::vector<unsigned char>& vec,
								 const size_t vectorLength = 1024 / 8)
	{
		return MPInteger().fromBigEndianMemory(
			makeBigEndianVector(&vec.front(), vec.size(), vectorLength));
	}

	static MPInteger stringToMPI(const std::string& str,
								 const size_t vectorLength = 1024 / 8)
		throw(std::logic_error,
			  std::overflow_error,
			  std::invalid_argument)
	{
		return MPInteger().fromBigEndianMemory(
			makeBigEndianVector(&str[0], str.size(), vectorLength));
	}

	static KeyPair makeKeyPair(const size_t numberOfKeyBits,
							   const MPInteger& encryptExponent = 65537U)
	{
		assert((numberOfKeyBits % 8) == 0);
		static SecureRandom random;

		MPInteger p;
		do
		{
			p = MPInteger::getProbablePrime(numberOfKeyBits / 2, random);
		} while (gcd(p - 1U, encryptExponent) != MPInteger(1U));

		MPInteger q;
		do
		{
			q =	MPInteger::getProbablePrime(
				numberOfKeyBits-(numberOfKeyBits / 2), random);
		} while (gcd(q - 1U, encryptExponent) != MPInteger(1U));

		return KeyPair(p, q, encryptExponent);
	}

};


#endif /* RSA_HPP_ */
