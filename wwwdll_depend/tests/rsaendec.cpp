#include <iostream>
#include <cryptography/rsa.hpp>


int main()
{
	try
	{
		MPInteger encryptExponent("00010001");
		MPInteger modulus("cb785082c998a3dbaaae00ca9f5f80af77765a466ba9976d60cb8af8900c146f9212a62c1ce20305ec5ed6f32041d23e68e5d4ca278881d16bac0e28728044064c210d7b0e1a04d637c4f917b08877001c7b503855912363442ed8bfbe7b962a6af48c66d981e5d4985a7dde976d8f9c3c0c0e2ab57106f170a690039880d01b");

		MPInteger decryptExponent("38eb53ee7a71e268d992eab2c429c5c8904d420cd02275b389eff6d4f90b000e8419067671b002337e7fbb76f10fa8be7fbec7abb3aa7ad27861fde5efbe1509043b13a6f1922b815fd9c26cd2fee3bdab808f37d68558ca568d1f4bb12512b1946310306c2b9a5d473862d8990e28b7b758df506efc1eb1c2db4813601b31cd");
		MPInteger p("df5af13c2ff4c53b64e8038de2be2e22744a6d7e3d293de77dc86da38a72ba41e929c9d458ed16b28ef785b17e7494d84b479a82b7f1718db421075a1ae596c7");
		MPInteger q("e9355864a2cc8b507688d6cf780b54910205a16f5bfacb908c4541d7f6e44517dcf531afe5bd54aaeb75e399e546acf9f6e39f811c94950901ff1817d21e980d");
		MPInteger poweredP("64ce14c5de7ae99d1e9eae1069b0eff926808f86b0e9602223b2c7636ce8076cc301acbeb52d59cdd3596289d39a2f2c25fb7fc6354efdf87ced461c1f9d47f730e32bb21b947831fd48a48e085397c4fef2307a64668365533d78a671906b2bbb5585364861c2baf0c5ed581f95782faa2d732d0588110c316a19a3fa68ad0d");
		MPInteger poweredQ("66aa3bbceb1dba3e8c0f52ba35ae90b650f5cabfbac0374b3d18c39523240d02cf10f96d67b4a938190574694ca7a31242ea5503f23983d8eebec80c52e2fc0f1b3de1c8f2858ca43a7c5489a834df3b1d891fbdf12a9ffdf0f160194ceb2afeaf9f073091202319a794908677d8176c91de9afdafe8f5e53f3c765f9e18230f");

		MPInteger plaintext = "abffe123f875b1f45da2b2ca";

		PublicKey pubkey(encryptExponent, modulus);
		PrivateKey privkey(decryptExponent, modulus, p, q, poweredP, poweredQ);

		std::cout << "1024 bit RSA en/decrypt example." << std::endl;
		std::cout << "plaintext: " << plaintext.toString() << std::endl;
		RSA cipher(pubkey);
		MPInteger ciphertext = cipher.encrypt(plaintext);

		std::cout << "ciphertext: " << ciphertext.adjust().toString() << std::endl;
		RSA decrypter(privkey);
		MPInteger decrypttext = decrypter.decrypt(ciphertext);
		
		std::cout << "decrypttext: " << decrypttext.adjust().toString() << std::endl;

	} catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
