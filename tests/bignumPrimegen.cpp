#include <math/theory/NumberTheory.hpp>
#include <math/MPI.hpp>
#include <Cryptography/SecureRandom.hpp>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
	SecureRandom random;

	const size_t bitLength = std::atoi(argv[1]);
	for (int i = 0; i < 100; ++i)
	{
		std::cout << "gen prime: " <<
			MPInteger::getProbablePrime(bitLength, random).toString() <<
			std::endl;
	}

	return 0;
}
