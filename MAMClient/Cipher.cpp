#include "stdafx.h"
#include "Cipher.h"

Cipher::Cipher() {
	key1 = new BYTE[256];
	key2 = new BYTE[256];

	//Init Key 1
	int val = 31;
	int i = 0;
	do
	{
		key1[i] = val;
		val = (-3 * val) + (32 * val) % 256 * val + 8;

		++i;
	} while (i < 256);

	//Init Key 2
	int val2 = 63;
	i = 0;
	do
	{
		key2[i] = val2;
		val2 = val2 * (122 * val2 - 49) - 27;
		++i;
	} while (i < 256);

	first = 0;
	second = 0;
}


BYTE Cipher::encrypt(BYTE val, bool useSeed) {
	BYTE encryptVal;
	if (!useSeed) encryptVal = (val ^ key1[first]) ^ key2[second];
	else encryptVal = (val ^ seedKey1[first]) ^ seedKey2[second];

	increment();
	return encryptVal;
}


BYTE Cipher::decrypt(BYTE val, bool useSeed) {
	BYTE decryptVal;
	if (!useSeed) decryptVal = (val ^ key1[first]) ^ key2[second];
	else decryptVal = (val ^ seedKey1[first]) ^ seedKey2[second];

	increment();
	return decryptVal;
}


void Cipher::increment() {
	first++;
	if (first == 256) {
		first = 0;
		second++;
	}
	if (second == 256) {
		second = 0;
	}
}


void Cipher::seed(int newSeed) {
	cipherSeed = newSeed;

	int cipherSeed2 = cipherSeed * cipherSeed;

	BYTE* seedArr;
	BYTE* seedArr2;

	seedArr = createSeedArr(cipherSeed);
	seedArr2 = createSeedArr(cipherSeed2);

	for (int i = 0, n = 3; i < 256; i++, n = (n == 0) ? 3 : --n) {
		seedKey1[i] = (BYTE)(key1[i] ^ seedArr[n]);
		seedKey2[i] = (BYTE)(key2[i] ^ seedArr2[n]);
	}
}


BYTE* Cipher::createSeedArr(int cSeed) {
	BYTE* arr = new BYTE[4];

	arr[0] = cSeed >> 24;
	arr[1] = cSeed >> 16;
	arr[2] = cSeed >> 8;
	arr[3] = cSeed;
	return arr;
}


void Cipher::reset() {
	first = 0;
	second = 0;
}
