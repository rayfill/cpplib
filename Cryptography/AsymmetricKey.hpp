#ifndef ASYMMETRICKEY_HPP_
#define ASYMMETRICKEY_HPP_

#include <math/MPI.hpp>
#include <math/theory/AlgebraTheory.hpp>
#include <math/theory/NumberTheory.hpp>
#include <stdexcept>
#include <ostream>

class PrivateKey
{
private:
	MPInteger decryptExponent;
	MPInteger modulus;
	MPInteger primeP;
	MPInteger primeQ;
	MPInteger crtP; // as p^(q-1) mod n
	MPInteger crtQ; // as q^(p-1) mod n

public:
	bool isCRTSupport() const
	{
		return primeP != primeQ && !crtP.isZero() && !crtQ.isZero();
	}

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

class PublicKey
{
private:
	MPInteger encryptExponent;
	MPInteger modulus;

public:
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
			if (gcd(e, (p-1) * (q-1)) != 1)
				throw std::invalid_argument(
					"gcd((e, euler(p-1, q-1)) != 1.");

			// p's prime check.
			for (int checkCount = 0; checkCount < 4; ++checkCount)
			{
				if (RabinPrimeTest(p, MPInteger(2U)) == false)
					throw std::invalid_argument("p is not prime.");
			}

			// q's prime check.
			for (int checkCount = 0; checkCount < 4; ++checkCount)
			{
				if (RabinPrimeTest(q, MPInteger(2U)) == false)
					throw std::invalid_argument("q is not prime.");
			}
		}

		modulus = p * q;
		decryptExponent = modulusInvert(e, lcm(p-1U, q-1U));
		if (decryptExponent.isZero())
			throw std::invalid_argument("can not calc decrypt number.");
		
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
						  modulusExponential(primeP, primeQ-1, modulus),
						  modulusExponential(primeQ, primeP-1, modulus));
	}
};



#endif /* ASYMMETRICKEY_HPP_ */
