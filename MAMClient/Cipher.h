#ifndef __CIPHER_H
#define __CIPHER_H

class Cipher {
private:
	BYTE *key1 = nullptr, *key2 = nullptr;
	BYTE seedKey1[256], seedKey2[256];
	int cipherSeed;

	void Cipher::increment();

public:
	int first;
	int second;

	Cipher();
	~Cipher();
	BYTE Cipher::encrypt(BYTE val, bool useSeed);
	BYTE Cipher::decrypt(BYTE val, bool useSeed);
	void Cipher::seed(int newSeed);
	BYTE* Cipher::createSeedArr(int cSeed);
	void Cipher::reset();
};

#endif