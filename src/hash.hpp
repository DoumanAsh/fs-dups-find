#pragma once

#include <cstddef>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

class Hasher {
    mutable CryptoPP::SHA256 hasher;

    public:
        std::string calculate(const char* data, std::size_t size) const {
            CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
            hasher.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(data), size);

            CryptoPP::HexEncoder encoder;
            std::string output;

            encoder.Put(digest, sizeof(digest));
            encoder.MessageEnd();

            output.resize(encoder.MaxRetrievable());
            encoder.Get(reinterpret_cast<CryptoPP::byte*>(&output[0]), size);

            hasher.Restart();

            return output;
        }

        std::string calculate_file(const char* path) const {
            std::string result;

            //CryptoPP doesn't require you to delete these pointers
            CryptoPP::FileSource f(path, true, new CryptoPP::HashFilter(hasher, new CryptoPP::HexEncoder(new CryptoPP::StringSink(result))));

            return result;
        }
};
