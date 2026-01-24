//
// Created by neali on 1/24/2026.
//

#ifndef PANCAKE_ENCRYPTION_H
#define PANCAKE_ENCRYPTION_H

#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>

CryptoPP::HMAC<CryptoPP::SHA256> hmac(key, keySize); // Using Crypto++

#endif //PANCAKE_ENCRYPTION_H