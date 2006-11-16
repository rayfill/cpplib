#ifndef ASYMMETRICKEY_HPP_
#define ASYMMETRICKEY_HPP_

#include <math/MPI.hpp>
#include <math/theory/AlgebraTheory.hpp>
#include <math/theory/NumberTheory.hpp>
#include <stdexcept>
#include <ostream>

/**
 * 公開鍵暗号のプライベート鍵
 */
class PrivateKey
{
private:
	MPInteger decryptExponent;
	MPInteger modulus;
	MPInteger primeP;
	MPInteger primeQ;
	MPInteger crtP; // p^(q-1) mod n (中国人剰余定理用)
	MPInteger crtQ; // q^(p-1) mod n (中国人剰余定理用)

public:
	/**
	 * 中国人剰余定理を使った高速化をサポートしているかの判定
	 * @return true: サポートしている false:未サポート
	 */
	bool isCRTSupport() const
	{
		return primeP != primeQ && !crtP.isZero() && !crtQ.isZero();
	}

	/**
	 * コンストラクタ
	 * @param decryptExponent_ 復号用べき乗数
	 * @param modulus_ 除数
	 * @param primeP_ 鍵の元になる素数その1
	 * @param primeQ_ 鍵の元になる素数その2
	 * @param crtP_ 中国人剰余定理に使用するパラメータ1
	 * @param crtQ_ 中国人剰余定理に使用するパラメータ2
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
	 * ストリーム出力用
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
 * 公開鍵暗号の公開鍵
 */
class PublicKey
{
private:
	MPInteger encryptExponent;
	MPInteger modulus;

public:
	/**
	 * コンストラクタ
	 * encryptExponent_ 暗号用べき乗数
	 * modulus_ 除数
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
 * 公開鍵、プライベート鍵のペア
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
	 * コンストラクタ
	 * @param p 鍵の元となる素数1
	 * @param q 鍵の元となる素数2
	 * @param e 暗号化処理時のべき乗数
	 * @param isPrimeCheck 与えられた引数p, q の素数性チェックをするかどうか
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
					throw std::invalid_argument("p は素数ではありません. " +
												p.toString());
			}

			// q's prime check.
			for (int checkCount = 0; checkCount < 4; ++checkCount)
			{
				if (RabinPrimeTest(q, MPInteger(2U)) == false)
					throw std::invalid_argument("q は素数ではありません. " +
						q.toString());
			}
		}

		modulus = p * q;
		decryptExponent = modulusInvert(e, lcm(p-1U, q-1U));
		if (decryptExponent.isZero())
			throw std::invalid_argument("逆元が計算できません.");
		
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
