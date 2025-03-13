#include <iostream>
#include <fstream>
#include <string>
#include <C:/cryptopp/aes.h>
#include <C:/cryptopp/modes.h>
#include <C:/cryptopp/filters.h>
#include <C:/cryptopp/osrng.h>
#include <C:/cryptopp/sha.h>     // For hashing
#include <C:/cryptopp/hex.h>     // For hex encoding
#include <C:/cryptopp/hmac.h>    // For HMAC
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
    std::ifstream file(filename, std::ios::binary);
    std::string content;
    if (file.is_open()) {
        std::getline(file, content, '\0');
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

// Simulate a machine ID
std::string getMachineID() {
    std::string id;
    AutoSeededRandomPool prng;
    byte buf[16];
    prng.GenerateBlock(buf, sizeof(buf));
    HexEncoder encoder(new StringSink(id));
    encoder.Put(buf, sizeof(buf));
    encoder.MessageEnd();
    return id;
}

// Load or generate secret key
std::string loadOrGenerateSecretKey(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::string secretKey;
    AutoSeededRandomPool prng;

    if (file.is_open()) {
        std::getline(file, secretKey, '\0');
        file.close();
        if (secretKey.size() == AES::DEFAULT_KEYLENGTH * 2) { // Hex-encoded length
            return secretKey;
        }
    }

    // Generate new secret key if file doesn’t exist or is invalid
    byte key[AES::DEFAULT_KEYLENGTH];
    prng.GenerateBlock(key, sizeof(key));
    HexEncoder encoder(new StringSink(secretKey));
    encoder.Put(key, sizeof(key));
    encoder.MessageEnd();

    std::ofstream outFile(filename, std::ios::binary);
    if (outFile.is_open()) {
        outFile << secretKey;
        outFile.close();
        std::cout << "Generated and saved new secret key to " << filename << "\n";
    } else {
        std::cerr << "Error saving secret key.\n";
    }
    return secretKey;
}

// Generate license key with HMAC-SHA256
std::string generateLicenseKey(const std::string& machineID, const std::string& secretKeyHex) {
    std::string digest;
    byte secretKey[AES::DEFAULT_KEYLENGTH];
    HexDecoder decoder;
    decoder.Put((byte*)secretKeyHex.data(), secretKeyHex.size());
    decoder.MessageEnd();
    decoder.Get(secretKey, sizeof(secretKey));

    HMAC<SHA256> hmac(secretKey, sizeof(secretKey));
    StringSource(machineID, true, new HashFilter(hmac, new HexEncoder(new StringSink(digest))));
    return digest;
}

// Validate license key
bool validateLicense(const std::string& machineID, const std::string& licenseKey, const std::string& secretKeyHex) {
    std::string expectedKey = generateLicenseKey(machineID, secretKeyHex);
    return expectedKey == licenseKey;
}

// Save license to file
void saveLicense(const std::string& licenseKey, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << licenseKey;
        file.close();
        std::cout << "License saved to " << filename << "\n";
    } else {
        std::cerr << "Error saving license.\n";
    }
}

// Load license from file
std::string loadLicense(const std::string& filename) {
    std::ifstream file(filename);
    std::string licenseKey;
    if (file.is_open()) {
        std::getline(file, licenseKey);
        file.close();
    }
    return licenseKey;
}

int main() {
    std::cout << "MediaLock DRM Simulator v1.0\n";

    // Load or generate secret key
    std::string secretKeyHex = loadOrGenerateSecretKey("secret.key");
    // std::cout << "Secret Key (hex): " << secretKeyHex << "\n"; // Uncomment for debugging (avoid in production)

    // Simulate machine ID
    std::string machineID = getMachineID();
    std::cout << "Machine ID: " << machineID << "\n";

    // Generate and save license key
    std::string licenseKey = generateLicenseKey(machineID, secretKeyHex);
    saveLicense(licenseKey, "license.key");

    // Load and validate license
    std::string loadedLicense = loadLicense("license.key");
    if (!validateLicense(machineID, loadedLicense, secretKeyHex)) {
        std::cerr << "Invalid license key! Decryption aborted.\n";
        return 1;
    }
    std::cout << "License validated successfully.\n";

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