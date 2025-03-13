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

// File I/O helpers
std::string readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::string content;
    if (file.is_open()) {
        content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    } else {
        std::cerr << "Error: Could not read " << filename << "\n";
    }
    return content;
}

void writeFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write(data.data(), data.size());
        file.close();
    } else {
        std::cerr << "Error: Could not write to " << filename << "\n";
    }
}

// Crypto functions
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

// Simulate machine ID (replace with real hardware ID in production)
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
    std::string secretKeyHex = readFile(filename);
    AutoSeededRandomPool prng;

    if (!secretKeyHex.empty() && secretKeyHex.size() == AES::DEFAULT_KEYLENGTH * 2) {
        return secretKeyHex;
    }

    byte secretKey[AES::DEFAULT_KEYLENGTH];
    prng.GenerateBlock(secretKey, sizeof(secretKey));
    HexEncoder encoder(new StringSink(secretKeyHex));
    encoder.Put(secretKey, sizeof(secretKey));
    encoder.MessageEnd();

    writeFile(filename, secretKeyHex);
    std::cout << "Generated and saved secret key to " << filename << "\n";
    return secretKeyHex;
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

// Validate license
bool validateLicense(const std::string& machineID, const std::string& licenseKey, const std::string& secretKeyHex) {
    std::string expectedKey = generateLicenseKey(machineID, secretKeyHex);
    return expectedKey == licenseKey;
}

int main(int argc, char* argv[]) {
    std::cout << "MediaLock DRM Simulator v1.0\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args]\n"
                  << "Commands:\n"
                  << "  generate-license          Generate a license key\n"
                  << "  encrypt <input> <output>  Encrypt a file\n"
                  << "  decrypt <input> <output>  Decrypt a file\n";
        return 1;
    }

    std::string command = argv[1];
    std::string machineID = getMachineID();
    std::string secretKeyHex = loadOrGenerateSecretKey("secret.key");

    if (command == "generate-license") {
        std::string licenseKey = generateLicenseKey(machineID, secretKeyHex);
        writeFile("license.key", licenseKey);
        std::cout << "License generated for Machine ID: " << machineID << "\n";
        std::cout << "License key saved to license.key\n";
        return 0;
    }

    // Load and validate license for encrypt/decrypt
    std::string loadedLicense = readFile("license.key");
    if (loadedLicense.empty() || !validateLicense(machineID, loadedLicense, secretKeyHex)) {
        std::cerr << "Error: Invalid or missing license key. Run 'generate-license' first.\n";
        return 1;
    }
    std::cout << "License validated successfully.\n";

    // Generate encryption key and IV
    AutoSeededRandomPool prng;
    byte key[AES::DEFAULT_KEYLENGTH];
    byte iv[AES::BLOCKSIZE];
    prng.GenerateBlock(key, sizeof(key));
    prng.GenerateBlock(iv, sizeof(iv));

    if (command == "encrypt") {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " encrypt <input_file> <output_file>\n";
            return 1;
        }
        std::string inputFile = argv[2];
        std::string outputFile = argv[3];
        std::string plaintext = readFile(inputFile);
        if (plaintext.empty()) return 1;

        std::string ciphertext = encryptMedia(plaintext, key, iv);
        // Prepend key and IV to output file (in production, store these securely elsewhere)
        std::string outputData = std::string((char*)key, AES::DEFAULT_KEYLENGTH) +
                                 std::string((char*)iv, AES::BLOCKSIZE) + ciphertext;
        writeFile(outputFile, outputData);
        std::cout << "Encrypted " << inputFile << " to " << outputFile << "\n";
    } else if (command == "decrypt") {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " decrypt <input_file> <output_file>\n";
            return 1;
        }
        std::string inputFile = argv[2];
        std::string outputFile = argv[3];
        std::string encryptedData = readFile(inputFile);
        if (encryptedData.size() < AES::DEFAULT_KEYLENGTH + AES::BLOCKSIZE) {
            std::cerr << "Error: Invalid encrypted file (too short).\n";
            return 1;
        }

        // Extract key, IV, and ciphertext
        const byte* loadedKey = (byte*)encryptedData.data();
        const byte* loadedIV = loadedKey + AES::DEFAULT_KEYLENGTH;
        std::string ciphertext = encryptedData.substr(AES::DEFAULT_KEYLENGTH + AES::BLOCKSIZE);
        std::string decrypted = decryptMedia(ciphertext, loadedKey, loadedIV);
        writeFile(outputFile, decrypted);
        std::cout << "Decrypted " << inputFile << " to " << outputFile << "\n";
    } else {
        std::cerr << "Error: Unknown command '" << command << "'\n";
        return 1;
    }

    return 0;
}