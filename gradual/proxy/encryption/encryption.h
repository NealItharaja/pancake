#ifndef PANCAKE_ENCRYPTION_H
#define PANCAKE_ENCRYPTION_H

#include <string>
#include <vector>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>

class encryption_engine {
public:
    encryption_engine();
    std::string encrypt(const std::string& plain_text);
    std::string decrypt(const std::string& cipher_text);
    std::string hmac(const std::string& key);

private:
    CryptoPP::SecByteBlock aes_key_;
    CryptoPP::SecByteBlock iv_;
};

#endif //PANCAKE_ENCRYPTION_H