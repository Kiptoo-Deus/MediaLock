#include <iostream>
#include <fstream>
#include <string>
#include <C:/cryptopp/aes.h>
#include <C:/cryptopp/modes.h>
#include <C:/cryptopp/filters.h>
#include <C:/cryptopp/osrng.h>

using namespace CryptoPP;

void createSampleMedia(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "This is a sample video file content.\n";
        file.close();
        std::cout << "Sample media created: " << filename << "\n";
    } else {
        std::cerr << "Error creating sample media.\n";
    }
}

std::string readMedia(const std::string& filename) {
    std::ifstream file(filename);
    std::string content;
    if (file.is_open()) {
        std::getline(file, content, '\0'); // Read entire file
        file.close();
    }
    return content;
}

std::string encryptMedia(const std::string& plaintext, const byte* key, const byte* iv) {
    std::string ciphertext;
    CBC_Mode<AES>::Encryption enc;
    enc.SetKeyWithIV(key, AES::DEFAULT_KEYLENGTH, iv);
    StringSource(plaintext, true, new StreamTransformationFilter(enc, new StringSink(ciphertext)));
    return ciphertext;
}

std::string decryptMedia(const std::string& ciphertext, const byte* key, const byte* iv) {
    std::string recovered;
    CBC_Mode<AES>::Decryption dec;
    dec.SetKeyWithIV(key, AES::DEFAULT_KEYLENGTH, iv);
    StringSource(ciphertext, true, new StreamTransformationFilter(dec, new StringSink(recovered)));
    return recovered;
}

int main() {
    std::cout << "MediaLock DRM Simulator v1.0\n";

    // Create sample media
    std::string filename = "sample_video.txt";
    createSampleMedia(filename);
    std::string content = readMedia(filename);
    std::cout << "Original content: " << content << "\n";

    // Encryption setup
    byte key[AES::DEFAULT_KEYLENGTH];
    byte iv[AES::BLOCKSIZE];
    AutoSeededRandomPool prng;
    prng.GenerateBlock(key, sizeof(key));
    prng.GenerateBlock(iv, sizeof(iv));

    // Encrypt
    std::string encrypted = encryptMedia(content, key, iv);
    std::ofstream encFile("encrypted_video.bin", std::ios::binary);
    encFile << encrypted;
    encFile.close();
    std::cout << "Media encrypted.\n";

    // Decrypt
    std::string decrypted = decryptMedia(encrypted, key, iv);
    std::cout << "Decrypted content: " << decrypted << "\n";

    return 0;
}