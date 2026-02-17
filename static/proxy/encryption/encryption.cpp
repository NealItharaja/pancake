#include "encryption.h"
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <stdexcept>

using namespace CryptoPP;
using namespace std;

encryption_engine::encryption_engine() {
    AutoSeededRandomPool prng;
    aes_key_ = SecByteBlock(AES::DEFAULT_KEYLENGTH * 2);
    prng.GenerateBlock(aes_key_, aes_key_.size());
}

string encryption_engine::encrypt(const string& plain_text) {
    AutoSeededRandomPool prng;
    SecByteBlock iv(AES::BLOCKSIZE);
    prng.GenerateBlock(iv, iv.size());
    string cipher;
    CBC_Mode<AES>::Encryption enc;
    enc.SetKeyWithIV(aes_key_, aes_key_.size(), iv);

    StringSource ss(plain_text, true,
        new StreamTransformationFilter(
            enc,
            new StringSink(cipher)
        )
    );

    string output(reinterpret_cast<const char*>(iv.data()), iv.size());
    output += cipher;
    string encoded;
    StringSource(output, true, new Base64Encoder(new StringSink(encoded), false));
    return encoded;
}

string encryption_engine::decrypt(const string& cipher_text) {
    string decoded;
    StringSource(cipher_text, true, new Base64Decoder(new StringSink(decoded)));

    if (decoded.size() < AES::BLOCKSIZE) {
        throw runtime_error("ciphertext too short");
    }

    SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(decoded.data()), AES::BLOCKSIZE);
    string cipher = decoded.substr(AES::BLOCKSIZE);
    string recovered;
    CBC_Mode<AES>::Decryption dec;
    dec.SetKeyWithIV(aes_key_, aes_key_.size(), iv);

    StringSource ss(
        cipher,
        true,
        new StreamTransformationFilter(dec, new StringSink(recovered))
    );
    return recovered;
}

string encryption_engine::hmac(const string& message) {
    HMAC<SHA256> hmac(aes_key_, aes_key_.size());
    string mac;
    StringSource(message, true, new HashFilter(hmac, new StringSink(mac)));
    string encoded;
    StringSource(mac, true, new HexEncoder(new StringSink(encoded), false));
    return encoded;
}
