#ifndef ASYMMETRICKEY_HPP_
#define ASYMMETRICKEY_HPP_

#include <math/MPI.hpp>
#include <math/theory/AlgebraTheory.hpp>
#include <math/theory/NumberTheory.hpp>
#include <stdexcept>
#include <ostream>

/**
 * ���J���Í��̃v���C�x�[�g��
 */
class PrivateKey
{
private:
	MPInteger decryptExponent;
	MPInteger modulus;
	MPInteger primeP;
	MPInteger primeQ;
	MPInteger crtP; // p^(q-1) mod n (�����l��]�藝�p)
	MPInteger crtQ; // q^(p-1) mod n (�����l��]�藝�p)

public:
	/**
	 * �����l��]�藝���g�������������T�|�[�g���Ă��邩�̔���
	 * @return true: �T�|�[�g���Ă��� false:���T�|�[�g
	 */
	bool isCRTSupport() const
	{
		return primeP != primeQ && !crtP.isZero() && !crtQ.isZero();
	}

	/**
	 * �R���X�g���N�^
	 * @param decryptExponent_ �����p�ׂ��搔
	 * @param modulus_ ����
	 * @param primeP_ ���̌��ɂȂ�f������1
	 * @param primeQ_ ���̌��ɂȂ�f������2
	 * @param crtP_ �����l��]�藝�Ɏg�p����p�����[�^1
	 * @param crtQ_ �����l��]�藝�Ɏg�p����p�����[�^2
	 */
	PrivateKey(const MPInteger& decryptExponent_ = 0U,
			   const MPInteger& modulus_ = 0U,
			   const MPInteger& primeP_ = 0U,
			   const MPInteger& primeQ_ =0U,
			   const MPInteger& crtP_ = 0U,
			   const MPInteger& crtQ_ = 0U) throw()
		: decryptExponent(decryptExponent_),
		  modulus(modulus_),
		  primeP(primeP_),
		  primeQ(primeQ_),
		  crtP(crtP_),
		  crtQ(crtQ_)
	{}

	MPInteger getDecryptExponent() const throw()
	{
		return decryptExponent;
	}

	MPInteger getModulus() const throw()
	{
		return modulus;
	}

	MPInteger getPrimeP() const throw()
	{
		return primeP;
	}
	MPInteger getPrimeQ() const throw()
	{
		return primeQ;
	}
	MPInteger getCRTP() const throw()
	{
		return crtP;
	}
	MPInteger getCRTQ() const throw()
	{
		return crtQ;
	}

	/**
	 * �X�g���[���o�͗p
	 */
	friend std::ostream& operator<<(std::ostream& out, const PrivateKey& key)
	{
		out << "private key" << std::endl;
		out << "decrypt exponent: " <<
			key.getDecryptExponent().toString() << std::endl;
		out << "modulus: " <<
			key.getModulus().toString() << std::endl;
		out << "prime1: " <<
			key.getPrimeP().toString() << std::endl;
		out << "prime2: " <<
			key.getPrimeQ().toString() << std::endl;
		out << "CRT factor1: " <<
			key.getCRTP().toString() << std::endl;
		out << "CRT factor2: " <<
			key.getCRTQ().toString() << std::endl;
		
		return out;
	}
};

/**
 * ���J���Í��̌��J��
 */
class PublicKey
{
private:
	MPInteger encryptExponent;
	MPInteger modulus;

public:
	/**
	 * �R���X�g���N�^
	 * encryptExponent_ �Í��p�ׂ��搔
	 * modulus_ ����
	 */
	PublicKey(const MPInteger& encryptExponent_ = 0U,
			  const MPInteger& modulus_ = 0U) throw()
		: encryptExponent(encryptExponent_),
		  modulus(modulus_)
	{}

	MPInteger getEncryptExponent() const throw()
	{
		return encryptExponent;
	}

	MPInteger getModulus() const throw()
	{
		return modulus;
	}

	friend std::ostream& operator<<(std::ostream& out, const PublicKey& key)
	{
		out << "public key" << std::endl;
		out << "encrypt exponent: " <<
			key.getEncryptExponent().toString() << std::endl;
		out << "modulus: " <<
			key.getModulus().toString() << std::endl;
		
		return out;
	}
};

/**
 * ���J���A�v���C�x�[�g���̃y�A
 */
class KeyPair
{
	friend class AsymmetricKeyTest;

private:
	MPInteger encryptExponent;
	MPInteger decryptExponent;
	MPInteger modulus;
	MPInteger primeP;
	MPInteger primeQ;
public:

	/**
	 * �R���X�g���N�^
	 * @param p ���̌��ƂȂ�f��1
	 * @param q ���̌��ƂȂ�f��2
	 * @param e �Í����������ׂ̂��搔
	 * @param isPrimeCheck �^����ꂽ����p, q �̑f�����`�F�b�N�����邩�ǂ���
	 */
	KeyPair(const MPInteger& p,
			const MPInteger& q,
			const MPInteger& e = MPInteger(65537U),
			bool isPrimeCheck = true)
		: encryptExponent(e), decryptExponent(),
		  modulus(p*q), primeP(p), primeQ(q)
	{
		if (isPrimeCheck)
		{
			// e's prime check.
			if (gcd(e, (p-1U) * (q-1U)) != 1U)
				throw std::invalid_argument(
					"gcd((e, euler(p-1, q-1)) != 1.");

			// p's prime check.
			for (int checkCount = 0; checkCount < 4; ++checkCount)
			{
				if (RabinPrimeTest(p, MPInteger(2U)) == false)
					throw std::invalid_argument("p �͑f���ł͂���܂���. " +
												p.toString());
			}

			// q's prime check.
			for (int checkCount = 0; checkCount < 4; ++checkCount)
			{
				if (RabinPrimeTest(q, MPInteger(2U)) == false)
					throw std::invalid_argument("q �͑f���ł͂���܂���. " +
						q.toString());
			}
		}

		modulus = p * q;
		decryptExponent = modulusInvert(e, lcm(p-1U, q-1U));
		if (decryptExponent.isZero())
			throw std::invalid_argument("�t�����v�Z�ł��܂���.");
		
		encryptExponent.adjust();
		decryptExponent.adjust();
		modulus.adjust();
	}

	PublicKey getPublicKey() const
	{
		return PublicKey(encryptExponent, modulus);
	}

	PrivateKey getPrivateKey() const
	{
		return PrivateKey(decryptExponent, modulus, primeP, primeQ,
						  modulusExponential(primeP, primeQ-1U, modulus),
						  modulusExponential(primeQ, primeP-1U, modulus));
	}
};



#endif /* ASYMMETRICKEY_HPP_ */
