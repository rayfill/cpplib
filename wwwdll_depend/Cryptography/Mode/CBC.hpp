#ifndef CBC_HPP_
#define CBC_HPP_

#include <vector>
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <iterator>

/**
 * �r���I�_���a�t�@���N�^
 */
struct ExclusiveOr
{
public:
	ExclusiveOr()
	{}

	/**
	 * �r���I�_���a�t�@���N�^
	 * @param lhs ���Ӓl
	 * @param rhs �E�Ӓl
	 * @param ���ӂ̔r���I�_���a
	 */
	unsigned char operator()(unsigned char lhs, unsigned char rhs)
	{
		return lhs ^ rhs;
	}
};

/**
 * �Í��u���b�N�A��
 * �Í����A���S���Y�����g���ău���b�N�T�C�Y���ƂɈÍ��u���b�N�A���������{��
 * @param ChiperAlgorithm �Í����A���S���Y��
 */
template <class CipherAlgorithm>
class CipherBlockChain
{
	friend class CBCTest;

public:
	/**
	 * �Í�������
	 * @param plaintext ����
	 * @param ��
	 * @param initialVector �����Í����u���b�N��
	 * @return �Í��u���b�N�A����p���ď������ꂽ�Í���
	 */
	static std::vector<unsigned char>
	encrypt(const std::vector<unsigned char>& plaintext,
			const std::vector<unsigned char>& key,
			const std::vector<unsigned char>& initialVector)
	{
		if (plaintext.size() % initialVector.size() ||
			key.size() < initialVector.size())
			throw std::invalid_argument("invalid argument.");

		CipherAlgorithm cipherAlg;
		const size_t blockSize = cipherAlg.getBlockSize();
		cipherAlg.setKey(key);

		std::vector<unsigned char> plain(plaintext);
		std::vector<unsigned char> exclusiveVector(initialVector);
		assert((exclusiveVector.size() % blockSize) == 0);

		std::vector<unsigned char> result(blockSize);
		for (size_t offset = 0;
			 offset < plain.size();
			 offset += blockSize)
		{
			assert(offset + blockSize <= plain.size());
			std::transform(plain.begin() + offset,
						   plain.begin() + offset + blockSize,
						   exclusiveVector.begin(),
						   result.begin(),
						   ExclusiveOr());

			std::vector<unsigned char> cipher =
				cipherAlg.encrypt(result);
			result.swap(cipher);

			assert(result.size() == blockSize);

			std::copy(result.begin(),
					  result.end(),
					  exclusiveVector.begin());
			std::copy(result.begin(),
					  result.end(),
					  plain.begin() + offset);
		}

		return plain;
	}

	/**
	 * ����������
	 * @param ciphertext �Í���
	 * @param ��
	 * @param initialVector �����Í����u���b�N��
	 * @return �Í��u���b�N�A����p���ď������ꂽ����
	 */
	static std::vector<unsigned char>
	decrypt(const std::vector<unsigned char>& ciphertext,
			const std::vector<unsigned char>& key,
			const std::vector<unsigned char>& initialVector)
	{
		if (ciphertext.size() % initialVector.size() ||
			key.size() < initialVector.size())
			throw std::invalid_argument("invalid argument.");

		CipherAlgorithm cipherAlg;
		const size_t blockSize = cipherAlg.getBlockSize();
		cipherAlg.setKey(key);

		std::vector<unsigned char> cipher(ciphertext);
		std::vector<unsigned char> exclusiveVector(initialVector);
		assert((exclusiveVector.size() % blockSize) == 0);

		std::vector<unsigned char> result(blockSize);
		std::vector<unsigned char> nextIV(blockSize);

		for (size_t offset = 0;
			 offset < cipher.size();
			 offset += blockSize)
		{
			assert(offset + blockSize <= cipher.size());
			std::copy(cipher.begin() + offset,
					  cipher.begin() + offset + blockSize,
					  nextIV.begin());

			std::copy(cipher.begin() + offset,
					  cipher.begin() + offset + blockSize,
					  result.begin());

			std::vector<unsigned char> plain =
				cipherAlg.decrypt(result);
			result.swap(plain);

			assert(result.size() == blockSize);

			std::transform(result.begin(),
						   result.end(),
						   exclusiveVector.begin(),
						   cipher.begin() + offset,
						   ExclusiveOr());

			exclusiveVector.swap(nextIV);
		}

		return cipher;
	}
};

#endif /* CBC_HPP_ */